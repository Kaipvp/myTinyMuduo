#pragma once

#include "NoCopyable.h"
#include "Channel.h"
#include <memory>
#include "Poller.h"

class Poller;

class EventLoop : NoCopyable{
public:

    using Functor = std::function<void()>;

    void runInLoop(Functor cb);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    // bool hasChannel(Channel *channel);

private:

    std::unique_ptr<Poller> poller_;
};

/*
    调用一次会获得一次监听结果（有事件的fd及对应的事件）
    循环调用Poller::poll方法去不断获得发生事件的Channel，
    装填到activeChannel,然后调用这些Channel里面保管的不同类型事件的处理函数（Channel::handleEvent）
    EventLoop就是负责循环，封装了Poller和Channel,并向上提供自己的接口来使用
*/
