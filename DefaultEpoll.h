#include <stdlib.h>

#include "Poller.h"
#include "EpollPoller.h"

Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; // 生成poll的实例，主流流行的是epoll，poll这里不做实现
    }
    else
    {
        return new EpollPoller(loop); // 生成epoll的实例
    }
}