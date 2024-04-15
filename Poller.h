#pragma once

#include <vector>
#include <unordered_map>

#include "NoCopyable.h"
#include "TimeStamp.h"
#include "Channel.h"

class Channel;
class EventLoop;

//nuduo库中多路事件分发器的核心IO复用模块
class Poller{
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop* loop) : ownerLoop_(loop){}
    virtual ~Poller() = default;
    
    //给所有IO复用保留统一的接口
    virtual TimeStamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    //判断参数channel是否在当前的Poller当中
    bool hasChannel(Channel* channel) const;

    //EventLoop可以通过该接口获得默认的IO复用的具体实现
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    //负责记录文件描述符fd到Channel的映射，也帮忙保管所有注册在这个Poller上的Channel。
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_; //定义Poller所属的事件循环EventLoop
};

/*
    Poller和EpollPoller负责监听文件描述符是否触发以及返回的事件以及具体事件的模块
    一个Poller就是一个事件监听器（有说EventLoop是事件监听器）
    Poller是一个抽象类，EpollPoller是具体类
    一个Poll可以监听多个channel(文件描述符)，当监听到了会返回一个activeChannels
    activeChannel就是当前监听中活跃的Channel(有对应事件)
    但是你这个Channel是不是在我这个Poller中保存呢？
    所以需要一个ChannelMap(fd, Channel*)来记录当前的监听的文件描述符集

    poll其实就是封装了epoll_wait，
    当调用一次poll时，会获得这个监听器上的发生事件的fd,和对应的事件，
    因为那个fd是通过一个Channel类封装的，所通过ChannelMap可以找到对应的Channel对象，
    然后将该对象的revents_修改成对应的事件，同时将这个Channel写回到activeChannel
*/