1.
#define DECLARE_SINGLETON_MEMBER(_Ty)	\
	template <> _Ty* Singleton<_Ty>::_instance = NULL;   // 特化版本的宏

2.
struct rwlock
{
	int read;
	int write;
};
static inline void
rwlock_rlock(struct rwlock *lock) {
	for (;;) {
		while(lock->write) {
			__sync_synchronize();
		}
		__sync_add_and_fetch(&lock->read,1);
		if (lock->write) {		// 这里是一个比较忽略的点，在给lock->read加一之前已经提前进入write。
			__sync_sub_and_fetch(&lock->read,1);
		} else {
			break;
		}
	}
}
