#include <stdlib.h>
#include "mq.h"
#include <pthread.h>
#include "sync.h"
#include "double_link.h"
#include "link_list.h"
#include <signal.h>
#include "thread.h"
#include "SysTime.h"

// 线程内部数据 包括一个读队列和一个写队列
struct per_thread_struct
{
	list_node   next;
	struct double_link_node block;
	struct link_list *local_push_q;
	struct link_list *local_pop_q;
	condition_t cond;
	//标记是否已经被添加到thread_mq中，当一个线程第一次执行mq_push的时候，会将此mq添加到thread_mq中，并设置此标记
	int8_t      is_associate;
	uint32_t    push_size;
};

struct mq
{
	uint32_t           push_size;
	pthread_key_t      t_key;
	mutex_t            mtx;
	struct double_link blocks;
	struct link_list  *share_list;			// 多线程共享队列
	struct link_list  *local_lists;			// 所有线程内部message queue组成的链表
	item_destroyer _item_destroyer;
};


// 为了再全局中找到message queue而做的辅助数据结构
struct mq_system
{
	pthread_key_t       t_key;
	mutex_t             mtx;
	struct link_list   *thread_mqs;
};

static struct mq_system *g_mq_system;


struct thread_mq_element
{
	list_node next;
	mq_t _mq;
};

/*线程mq管理结构，每线程一个,当线程第一次对一个mq执行mq_push时，会将这个mq添加到
* mqs中.
*/
struct thread_associate_mqs
{
	list_node   next;
	struct link_list *mqs;
};


void init_mq_system()
{
	g_mq_system = (struct mq_system *)calloc(1,sizeof(*g_mq_system));
	g_mq_system->mtx = mutex_create();
	g_mq_system->thread_mqs = LINK_LIST_CREATE();
	pthread_key_create(&g_mq_system->t_key,0);
}


static struct per_thread_struct *per_thread_create()
{
	struct per_thread_struct *pts = calloc(1,sizeof(*pts));
	pts->local_push_q = LINK_LIST_CREATE();
	pts->local_pop_q = LINK_LIST_CREATE();
	pts->cond = condition_create();
	return pts;
}

static void per_thread_destroy(struct per_thread_struct **pts)
{
	LINK_LIST_DESTROY(&(*pts)->local_push_q);
	LINK_LIST_DESTROY(&(*pts)->local_pop_q);
	condition_destroy(&(*pts)->cond);
	free(*pts);
	*pts = NULL;
}


void default_mq_item_destroyer(void* item)
{
	free(item);
}


mq_t create_mq(uint32_t push_size,item_destroyer _item_destroyer)
{
	mq_t m = calloc(1,sizeof(*m));
	m->mtx = mutex_create();
	pthread_key_create(&m->t_key,0);
	m->share_list = LINK_LIST_CREATE();
	m->local_lists = LINK_LIST_CREATE();
	double_link_clear(&m->blocks);
	m->push_size = push_size;
	m->_item_destroyer = _item_destroyer;
	return m; 	
}

static inline struct per_thread_struct* mq_get_pts(mq_t m)
{
	struct per_thread_struct *pts = (struct per_thread_struct*)pthread_getspecific(m->t_key);
	if(!pts)
	{
		pts = per_thread_create();
		pts->push_size = m->push_size;
		LINK_LIST_PUSH_BACK(m->local_lists,pts);
		pthread_setspecific(m->t_key,(void*)pts);
	}
	return pts;
}

void mq_set_push_size(mq_t m,uint32_t size)
{
	struct per_thread_struct *pts = mq_get_pts(m);
	pts->push_size = size;
}


static inline void destroy_q_item(struct link_list *l,item_destroyer _item_destroyer)
{
	if(_item_destroyer == NULL)
		return;
	list_node *n;//link_list_head((*m)->local_lists);
	while((n = LINK_LIST_POP(list_node*,l))!=NULL)
		_item_destroyer((void*)n);
}

void destroy_mq(mq_t *m)
{
	mutex_destroy(&(*m)->mtx);
	destroy_q_item((*m)->share_list,(*m)->_item_destroyer);//销毁share_list中所有元素
	LINK_LIST_DESTROY(&(*m)->share_list);
	list_node *l = link_list_head((*m)->local_lists);
	while(l)
	{
		struct per_thread_struct *pts = (struct per_thread_struct*)l;
		destroy_q_item(pts->local_push_q,(*m)->_item_destroyer);
		destroy_q_item(pts->local_pop_q,(*m)->_item_destroyer);
		l = l->next;
		per_thread_destroy(&pts);
	}	
	LINK_LIST_DESTROY(&(*m)->local_lists);
	free(*m);
	*m = NULL;		
}

static inline void mq_sync_push(mq_t m,struct per_thread_struct *pts)
{
	mutex_lock(m->mtx);
	uint8_t empty = link_list_is_empty(m->share_list);
	link_list_swap(m->share_list,pts->local_push_q);
	if(empty)
	{
		// empty说明可能有人在等待被唤醒然后swap到自己私有线程中进行处理
		struct double_link_node *l = double_link_pop(&m->blocks);
		if(l)
		{
			//if there is block per_thread_struct wake it up
			// 内存偏移操作找到 对应的per_thread_struct的头 -> 强转
			struct per_thread_struct *block_pts = (struct per_thread_struct *)((uint8_t*)l - sizeof(struct list_node));
			mutex_unlock(m->mtx);
			condition_signal(block_pts->cond);
			return;
		}
	}
	mutex_unlock(m->mtx);
}

static inline void mq_sync_pop(mq_t m,struct per_thread_struct *pts,uint32_t timeout)
{
	mutex_lock(m->mtx);
	if(link_list_is_empty(m->share_list))
	{
		if(timeout)
		{	
			while(link_list_is_empty(m->share_list))
			{
				double_link_push(&m->blocks,&pts->block);
				if(0 != condition_timedwait(pts->cond,m->mtx,timeout))
				{
					double_link_remove(&pts->block);
					break;
				}
			}
		}
	}
	link_list_swap(pts->local_pop_q,m->share_list);
	mutex_unlock(m->mtx);
}


static inline struct per_thread_struct* mq_push_local(mq_t m,struct list_node *msg)
{
	struct per_thread_struct *pts = mq_get_pts(m);
	if(0 == pts->is_associate)
	{
		struct thread_associate_mqs *tmq = (struct thread_associate_mqs*)pthread_getspecific(g_mq_system->t_key);
		if(!tmq)
		{
			tmq = (struct thread_associate_mqs*)calloc(1,sizeof(tmq));
			tmq->mqs = LINK_LIST_CREATE();
			mutex_lock(g_mq_system->mtx);
			LINK_LIST_PUSH_BACK(g_mq_system->thread_mqs,tmq);
			mutex_unlock(g_mq_system->mtx);
			pthread_setspecific(g_mq_system->t_key,(void*)tmq);	
		}
		struct thread_mq_element *ele = calloc(1,sizeof(*ele));
		ele->_mq = m;
		LINK_LIST_PUSH_BACK(tmq->mqs,ele);
		pts->is_associate = 1;
	}
	LINK_LIST_PUSH_BACK(pts->local_push_q,msg);
	return pts;
}

void mq_push(mq_t m,struct list_node *msg)
{
	struct per_thread_struct *pts = mq_push_local(m,msg);
	if(link_list_size(pts->local_push_q) >= pts->push_size)
		mq_sync_push(m,pts);			
}

void mq_push_now(mq_t m,struct list_node *msg)
{
	struct per_thread_struct *pts = mq_push_local(m,msg);
	mq_sync_push(m,pts);		
}


struct list_node* mq_pop(mq_t m,uint32_t timeout)
{
	struct per_thread_struct *pts = mq_get_pts(m);
	if(link_list_is_empty(pts->local_pop_q))
		mq_sync_pop(m,pts,timeout);
	return LINK_LIST_POP(struct list_node*,pts->local_pop_q);
}

void   mq_pop_all(mq_t m,struct link_list *l,uint32_t timeout)
{
	struct per_thread_struct *pts = mq_get_pts(m);
	if(link_list_is_empty(pts->local_pop_q))
		mq_sync_pop(m,pts,timeout);
	link_list_swap(l,pts->local_pop_q);
}


static inline void mq_force_sync(mq_t m)
{
	struct per_thread_struct *pts = (struct per_thread_struct *)pthread_getsepcific(m->t_key);
	if (!link_list_is_empty(pts->local_push_q))
		mq_sync_push(m, pts);
}

//冲刷当前线程使用的所有消息队列，将其中遗留的消息同步到share队列中
void mq_flush()
{
	struct thread_associate_mqs *tmq = (struct thread_associate_mqs*)pthread_getspecific(g_mq_system->t_key);
	if(tmq)
	{
		list_node *n = link_list_head(tmq->mqs);
		while(n)
		{
			mq_t _mq = ((struct thread_mq_element *)n)->_mq;
			mq_force_sync(_mq);
			n = n->next;
		}
	}
}

