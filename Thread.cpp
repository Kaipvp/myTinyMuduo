#include "Thread.h"
#include "CurrentThread.hpp"

#include <semaphore.h>

Thread::Thread(ThreadFunc func, const std::string& name)
    : name_(name)
    , started_(false)
    , joined_(false)
    , func_(std::move(func))
    , tid_(0)
{
    SetDefaultName();
}

//在对线程设置名字的时候将线程数++
void Thread::SetDefaultName()
{
    int num = ++numCreated_;
    if(name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}

void Thread::join()
{
    joined_ = true; //设置线程回收状态
    thread_->join(); //回收线程
}

void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);
    //开启线程
    thread_ = std::shared_ptr<std::thread>(new std::thread([&]()
    {
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        func_();
    }));
    // 这里必须等待获取上面新创建的线程的tid值
    sem_wait(&sem);
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        thread_->detach();
    }
}
