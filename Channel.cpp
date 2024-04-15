#include "Channel.h"

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop)
    , fd_(fd)
    , events_(0)
    , revents_(0)
    , index_(-1)
//    , tied_(false)
{}

Channel::~Channel()
{}

void Channel::handleEvent(TimeStamp receiveTime)
{
    // if (tied_)
    // {
    //     std::shared_ptr<void> guard = tie_.lock();
    //     if (guard)
    //     {
    //         handleEventWithGuard(receiveTime);
    //     }
    //     // 如果提升失败了 就不做任何处理 说明Channel的TcpConnection对象已经不存在了
    // }
    // else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(TimeStamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d\n", revents_);
    
    //关闭
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeCallback_){
            closeCallback_();
        }
    }
    //错误
    if(revents_ & EPOLLERR){
        if(errorCallback_){
            errorCallback_();
        }
    }
    // 读
    if (revents_ & (EPOLLIN | EPOLLPRI)){
        if (readCallback_){
            readCallback_(receiveTime);
        }
    }
    // 写
    if (revents_ & EPOLLOUT){
        if (writeCallback_){
            writeCallback_();
        }
    }

}

// 通过channel所属的eventloop，调用poller的相应方法，注册fd的events事件
void Channel::update()
{
    loop_->updateChannel(this);
}

// 在channel所属的EventLoop中把当前的channel删除掉
void Channel::remove()
{
    loop_->removeChannel(this);
}
