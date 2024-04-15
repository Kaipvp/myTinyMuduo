#pragma once

#include <functional>
#include <memory>
#include <sys/epoll.h>

#include "NoCopyable.h"
#include "TimeStamp.h"
#include "EventLoop.h"
#include "Logger.h"

class EventLoop;

class Channel : NoCopyable{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    //fd得到poller通知之后 处理事件 handleEvent在EventLoop::loop()中调用
    void handleEvent(TimeStamp receiveTime);

    //设置回调函数对象
    void setReadCallback(ReadEventCallback cb){ readCallback_ = std::move(cb);}
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    int fd() const { return fd_; }  //获取文件描述符
    int events() const { return events_; } //获取当前监听事件
    void set_revents(int revt) { revents_ = revt; }
    
    //将Channel中的文件描述符及其感兴趣事件注册事件监听器上或从事件监听器上移除;
    //update本质上调用了epoll_ctl
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    //返回fd当前相应的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    // 防止当channel被手动remove掉 channel还在执行回调操作
    // void tie(const std::shared_ptr<void> &);

    // one loop per thread
    EventLoop *ownerLoop() { return loop_; }

    int index() { return index_; }
    void set_index(int index) { index_ = index; }

    void remove();

private:

    void update();

    //在特定的时间戳receiveTime下处理事件,根据revents_的值来决定执行哪种类型的回调函数。
    void handleEventWithGuard(TimeStamp receiveTime);

    static const int kNoneEvent = 0;
    static const int kReadEvent = EPOLLIN | EPOLLPRI;
    static const int kWriteEvent = EPOLLOUT;
    
    EventLoop* loop_; // fd属于哪个EventLoop对象

    const int fd_; //Channel对象绑定的文件描述符

    int events_;// 注册的事件，表示这个Channel感兴趣的事件，如EPOLLIN（可读）、EPOLLOUT（可写）等。
    int revents_; // 监听成功，Poller返回的发生的事件

    int index_; //Channel在Poller中的状态，-1表示还未添加，1表示已添加，2表示已删除
     
    // std::weak_ptr<void> tie_; //一个弱指针，用于防止当Channel被手动移除时，Channel还在执行回调操作。
    // bool tied_;

    //因为channel通道里可获知fd最终发生的具体事件，
    //所以它负责具体事件的回调操作
    ReadEventCallback readCallback_ ;//读事件的回调函数，当文件描述符可读时，这个函数会被调用。
    EventCallback writeCallback_;//写事件的回调函数，当文件描述符可写时，这个函数会被调用。
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};