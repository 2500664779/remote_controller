#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
namespace Threads
{
    class IThread
    {
    public:
        enum ThreadStatus
        {
            ThreadSuspended = 0,   //挂起
            ThreadRunning = 1,     //运行
            ThreadWaiting = 2,     //等待
            ThreadTimeWaiting = 3, //限时等待
            ThreadInitial = 4,     //初始化
            ThreadCreated = 5,     //创建
            ThreadFinished = 6     //结束
        };
        IThread();
        virtual ~IThread();
        virtual void Start();
        virtual void Join();
        virtual void Cancel();
        virtual void Stop();
        pthread_t GetThreadId() const;
        ThreadStatus GetThreadStatus() const;

    private:
        static void *_ThreadFunc(void *arg);
        virtual void _Run() = 0;

    private:
        pthread_t _thread;
        bool _threadAttatched;
        ThreadStatus _status;
    };

    class EpollThread : public IThread
    {
    public:
        EpollThread();
        ~EpollThread();
    private:
        void _Run();
    };

    class SqlThread : public IThread
    {
    public:
        SqlThread();
        ~SqlThread();
    private:
        void _Run();
    };

} // namespace Threads

//声明进行epoll_wait的线程
void *epoll_thread(void *arg);
void *sql_thread(void *arg);

#endif