#pragma once

#include <vector>
#include <sys/epoll.h>

#include "Poller.h"
#include "TimeStamp.h"
#include "Channel.h"

/*
    epoll使用：
        epoll_create;
        epoll_ctl;//add, mod, del
        epoll_wait;
*/
class Channel;

class EpollPoller : public Poller{
public:
    EpollPoller(EventLoop* loop);
    virtual ~EpollPoller() override;

    TimeStamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    static const int kInitEventListSize = 16;

    //填写当次监听的活跃的Channel
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    //更新channel（可能新加监听节点或者摘下监听节点）,其实就是调用epoll_ctl
    void update(int operation, Channel* channel);

    int epollfd_; //epoll_create创建后的红黑树节点
    
    using EventList = std::vector<struct epoll_event>;
    EventList events_; //保存epoll_wait返回的发生事件的文件描述符集
};