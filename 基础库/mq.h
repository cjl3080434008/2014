/*	
    Copyright (C) <2012>  <huangweilook@21cn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
*  一个线程安全的单向消息队列,提供多线程对队列的N读N写.
*  每个队列有一个由所有线程共享的公共队列,同时每个线程有自己私有的push队列和pop队列
*  一个线程往消息队列中push消息时,首先将消息push到自己私有的push队列中,然后判断当前
*  私有push队列的消息数量是否超过了预定的阀值,如果超过就将私有队列中的消息全部同步到
*  共享公共队列中.
*  当一个线程从消息队列中pop消息时,首先查看自己的私人pop队列中是否有消息,如果有直接从
*  私有消息队列中pop一个出来,如果私有消息队列中没有消息,则尝试从共享队列中将所有消息
*  同步到私有队列.
*  
*  通过对私有和共享队列的划分,大量的避免了对消息队列操作过程中锁的使用.
*/

#ifndef _MQ_H
#define _MQ_H

#include <stdint.h>
#include "link_list.h"

typedef struct mq *mq_t;
typedef void (*item_destroyer)(void*);
void default_mq_item_destroyer(void* item);

#define MQ_DEFAULT_ITEM_DESTROYER default_mq_item_destroyer

/* 创建一个消息队列,设定默认push阀值为push_size
*  如果要将当前线程的push阀值设为跟默认值不同
*  使用mq_set_push_size
*/
mq_t create_mq(uint32_t push_size,item_destroyer);
void destroy_mq(mq_t*);

void mq_set_push_size(mq_t,uint32_t push_size);
void mq_push(mq_t,struct list_node*);
void mq_push_now(mq_t,struct list_node*);

/*
* 将消息队列中的消息全部pop到l中,如果消息队列为空最多等待timeout 
*/
void mq_pop2list(mq_t m,struct link_list *l,uint32_t timeout);
struct list_node* mq_pop(mq_t,uint32_t timeout);

void  init_mq_system();
void  mq_flush();
#endif
