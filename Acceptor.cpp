#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

static int createNonBlocking()
{
    //创建sockfd
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOG_FATAL("%s:%s:%d listen socket create err:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    : loop_(loop)
    , acceptSocket_(createNonBlocking())
    , acceptChannel_(loop_, acceptSocket_.fd())
    , listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenAddr);//绑定地址
    /*
        如果Tcp::start()->如果有新用户连接，执行一个回调
        accept->connfd->打包成channel->唤醒一个sucloop

        mainloop监听到有事件发生->acceptChannle_执行该回调函数
    */
    acceptChannel_.setReadCallback(
        std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll(); // 把从Poller中感兴趣的事件删除掉
    acceptChannel_.remove(); // 调用EventLoop->removeChannel => Poller->removeChannel 把Poller的ChannelMap对应的部分删除
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading(); //acceptChannel注册Poller上
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0)
    {
        if(NewConnectionCallback_)
        {
            // 轮询找到subLoop 唤醒并分发当前的新客户端的Channel
            NewConnectionCallback_(connfd, peerAddr);
        }
        else{
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("%s:%s:%d accept err:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        if (errno == EMFILE)
        {
            LOG_ERROR("%s:%s:%d sockfd reached limit\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}