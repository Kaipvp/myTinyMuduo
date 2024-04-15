#pragma once

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>

#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "NoCopyable.h"
// #include "EventLoopThreadPool.h"
#include "Callbacks.h"
// #include "TcpConnection.h"
// #include "Buffer.h"

class TcpServer : NoCopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    //端口复用
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const std::string& nameArg,
            Option option = kNoReusePort);
    ~TcpServer();

    // void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }
    // void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
    // void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    // void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

    //设置底层subloop的个数，初始线程大小
    void setThreadNum(int numThreads);

    //开启服务器监听
    void start();

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    EventLoop* loop_; //传进来用户定义的Eventloop
    const std::string ip_port;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_; //运行在mainloop中的监听器

    // ConnectionCallback connectCallback_; //有新的连接的回调
    // MessageCallback messageCallback_; //有读写消息的回调
    // WriteFinishCallback writeFinishCallback_; //消息发送完成的回调
    
    ThreadInitCallback threadInitCallback_; //线程初始化的回调

    std::atomic_int started_;

};