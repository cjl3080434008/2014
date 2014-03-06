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

3. #define DISABLE_COPY_AND_ASSIGN(TypeName)\
		TypeName(const TypeName &);			\
		const TypeName &operator=(const TypeName &)

4.union {

            //  Sent to I/O thread to let it know that it should
            //  terminate itself.
            struct {
            } stop;

            //  Sent to I/O object to make it register with its I/O thread.
            struct {
            } plug;

            //  Sent to socket to let it know about the newly created object.
            struct {
                zmq::own_t *object;
            } own;

            //  Attach the engine to the session. If engine is NULL, it informs
            //  session that the connection have failed.
            struct {
                struct i_engine *engine;
            } attach;

            //  Sent from session to socket to establish pipe(s) between them.
            //  Caller have used inc_seqnum beforehand sending the command.
            struct {
                zmq::pipe_t *pipe;
            } bind;

            //  Sent by pipe writer to inform dormant pipe reader that there
            //  are messages in the pipe.
            struct {
            } activate_read;

            //  Sent by pipe reader to inform pipe writer about how many
            //  messages it has read so far.
            struct {
                uint64_t msgs_read;
            } activate_write;

            //  Sent by pipe reader to writer after creating a new inpipe.
            //  The parameter is actually of type pipe_t::upipe_t, however,
            //  its definition is private so we'll have to do with void*.
            struct {
                void *pipe;
            } hiccup;

            //  Sent by pipe reader to pipe writer to ask it to terminate
            //  its end of the pipe.
            struct {
            } pipe_term;

            //  Pipe writer acknowledges pipe_term command.
            struct {
            } pipe_term_ack;

            //  Sent by I/O object ot the socket to request the shutdown of
            //  the I/O object.
            struct {
                zmq::own_t *object;
            } term_req;

            //  Sent by socket to I/O object to start its shutdown.
            struct {
                int linger;
            } term;

            //  Sent by I/O object to the socket to acknowledge it has
            //  shut down.
            struct {
            } term_ack;

            //  Transfers the ownership of the closed socket
            //  to the reaper thread.
            struct {
                zmq::socket_base_t *socket;
            } reap;

            //  Closed socket notifies the reaper that it's already deallocated.
            struct {
            } reaped;

            //  Sent by reaper thread to the term thread when all the sockets
            //  are successfully deallocated.
            struct {
            } done;

    } args;


5.
static inline uint32_t size_of_pow2(uint32_t size)
{
    if(is_pow2(size)) return size;
	size = size-1;
	size = size | (size>>1);
	size = size | (size>>2);
	size = size | (size>>4);
	size = size | (size>>8);
	size = size | (size>>16);
	return size + 1;
}






