

#include "TcpServer.h"
#include "Logger.h"

// void TcpServer::start()
// {
//     if(started_++ == 0)
//     {
//         thread_pool_->started(threadInitCallback);
//         loop_->runInLoop(bind(&Acceptor::listen, acceptor_.get()));
//     }
// }