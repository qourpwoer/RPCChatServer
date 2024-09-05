#include "TcpServer.h"
#include  "Logger.h"
#include "TcpConnection.h"

#include <strings.h>
#include <functional>

static EventLoop* CheckLoopNotNull(EventLoop *loop)
{
    if(loop == nullptr)
    {
       //LOG_FATAL("%s:%s:%d mainLoop is null \n", __FILE__, __FUNCTION__, __LINE__);
       LOG_FATAL << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << " mainLoop is null \n";
    }
    return loop;
}
    

TcpServer::TcpServer(EventLoop *loop,
                const InetAddress &listenAddr,
                const std::string &nameArg,
                Option option)
                : loop_(loop)
                , name_(nameArg)
                , ipPort_(listenAddr.toIpPort())
                , acceptor_(new Acceptor(loop, listenAddr, option == kReusePort))
                , threadPool_(new EventLoopThreadPool(loop, name_))
                , connectionCallback_()
                , messageCallback_()
                , nextConnId_(1)
                , started_(0)
{
    // 当有新用户连接时，会执行回调
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this,
    std::placeholders::_1, std::placeholders::_2));
}
TcpServer::~TcpServer()
{
    for(auto &item : connections_)
    {
        TcpConnectionPtr conn(item.second); //这个局部的智能指针对象出右括号会自动释放资源
        item.second.reset();

        // 销毁连接
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn)
        );
    }
}

// 设置底层subloop的个数
void TcpServer::setThreadNum(int numThreads)
{
    threadPool_->setThreadNum(numThreads);
}

// 开启服务器监听
void TcpServer::start()
{
    if(started_++ == 0)
    {
        threadPool_->start(threadInitCallback_); // 启动底层loop线程池
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get())); 
    }
}
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    // 轮询算法，选择一个subLoop
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    //LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s \n", 
    //name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());
    LOG_INFO << "TcpServer::newConnection [" << name_.c_str() << " - new connection [" << connName.c_str() << "] from " <<  peerAddr.toIpPort().c_str() << " \n";

    // 通过sockfd获取其绑定的本机的ip地址和端口信息
    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if(::getsockname(sockfd, (sockaddr*)&local, &addrlen) < 0)
    {
        //LOG_ERROR("sockets::getLocalAddr");
        LOG_ERROR << "sockets::getLocalAddr\n";
    }
    InetAddress localAddr(local);

    // 根据连接成功的sockfd,创建TcpConnection对象
    TcpConnectionPtr conn(new TcpConnection(
                            ioLoop,
                            connName,
                            sockfd,
                            localAddr,
                            peerAddr));
    connections_[connName] = conn;
    // 下面的回调都是用户设置的
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)
    );
    // 直接调用
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}
void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn)
    );
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    //LOG_INFO("TcpServer:removeConnectionInLoop [%s] - connection %s\n",
    //    name_.c_str(), conn->name().c_str());
    LOG_INFO << "TcpServer:removeConnectionInLoop [" << name_.c_str() << "] - connection " << conn->name().c_str() << "\n";
    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn)
    );
}