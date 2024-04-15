#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"

Socket::~Socket()
{
    ::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localaddr)
{
    if(0 != ::bind(sockfd_, (const sockaddr*)localaddr.getSockAddr(), sizeof(sockaddr_in)))
    {
        LOG_FATAL("bind sockfd:%d fail\n", sockfd_);
    }
}

void Socket::listen()
{
    if(0 != ::listen(sockfd_, 1024))
    {
        LOG_FATAL("listen sockfd:%d fail\n", sockfd_);
    }
}
    
int Socket::accept(InetAddress* peeraddr)
{
    sockaddr_in clitAddr;
    socklen_t clitAddrLen = sizeof(clitAddr);
    ::memset(&clitAddr, 0, sizeof(clitAddr));
    //这里使用accept4
    int connfd = ::accept4(sockfd_, (sockaddr*)&clitAddr, &clitAddrLen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd >= 0)
    {
        peeraddr->setSockAddr(clitAddr);
    }
    else if(connfd < 0)
    {
        LOG_FATAL("accept fail\n");
    }
    return connfd;
    
}

void Socket::shutdownWrite()
{
    if(::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("shutdownWrite error");
    }
} //设置半关闭

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    // IPPROTO_TCP包含在头文件 <netinet/tcp.h>
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
} //设置Nagle算法

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}//设置地址复用

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)); 
} //设置端口复用

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
} //设置长连接