#pragma once

#include "NoCopyable.h"

class InetAddress;

class Socket : NoCopyable{
public:
    explicit Socket(int sockfd) : sockfd_(sockfd){}
    ~Socket();

    int fd() { return sockfd_; } //获取sockfd
    void bindAddress(const InetAddress& localaddr);//调用bind绑定服务器IP和端口
    void listen();//设置最大监听上限
    int accept(InetAddress* peeraddr);//调用accept接收新客户连接请求 只不过这里封装的是accept4

    void shutdownWrite(); //半关闭

    void setTcpNoDelay(bool on); //设置Nagle算法
    void setReuseAddr(bool on); //设置地址复用
    void setReusePort(bool on); //设置端口复用
    void setKeepAlive(bool on); //设置长连接

private:
    const int sockfd_; //监听套接字
};

/*
    Socket对象管理socket的bind,listen,accept等方法，
    但是创建一个socket是在Acceptor里面创建的,
    也就是说Socket虽然存储了sockfd，但是初始化是在Acceptor里面创建的
*/