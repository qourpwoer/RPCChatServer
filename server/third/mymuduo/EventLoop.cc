#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory>

// 防止一个线程创建多个EventLoop    thread_local,一个线程一个拷贝
__thread EventLoop *t_loopInThisThread = nullptr;

// 定义默认的Poller IO复用接口的超时时间
const int kPollTimeMs = 10000;

// 创建wakeupfd，用来notify唤醒subReactor
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
    {   
        //LOG_FATAL("eventfd error:%d \n", errno);
        LOG_FATAL << "eventfd error:" << errno << " \n";
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupFd_(createEventfd())
    , wakeupChannel_(new Channel(this, wakeupFd_))
{
    //LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_ << " \n";
    if(t_loopInThisThread)
    {
        //LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_); 
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_ << " \n";
    }
    else
    {
        t_loopInThisThread = this;
    }

    //  设置wakeupfd的事件类型以及发生事件后的回调操作
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    // 每一个eventLoop都将监听wakeupchannel的EPOLL读事件
    wakeupChannel_->enableReading();
}
EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if(n != sizeof one)
    {
        //LOG_ERROR("EventLoop::handleRead() reads %lu bytes insteads of 8", n);
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes insteads of 8" << "\n";
    }
}
//  开启事件循环
void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    //LOG_INFO("EventLoop %p start looping \n", this);
    LOG_INFO << "EventLoop " << this << " start looping \n";
    
    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for(Channel *channel : activeChannels_)
        {
            // Poller监听哪些channel发生事件了，然后上报EventLoop
            channel->handleEvent(pollReturnTime_);
        }
        // 执行当前EventLoop事件循环需要处理的回调操作
        doPendingFunctors();
    }

    //LOG_INFO("EventLoop %p stop looping \n", this);
    LOG_INFO << "EventLoop " << this << " stop looping \n";
    looping_ = false;
}
// 退出事件循环
void EventLoop::quit()
{
    quit_ = true;

    if(!isInLoopThread()) // 如果是在其它线程中
    {
        wakeup();
    }
}
// 在当前loop中执行cb
void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread()) // 在当前的loop线程中，执行cb
    {
        cb();
    }
    else   //在当前loop线程中执行cb，就需要唤醒loop所在线程，执行cb
    {
        queueInLoop(cb);
    }
}
// 把cb放入队列中，唤醒loop所在的线程执行cb 
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    // 唤醒相应的，需要执行上面回调操作的loop的线程
    if(!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup(); // 唤醒loop所在线程
    }
}
 // 用来唤醒loop所在的线程 向wakeupfd_写一个数据
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof one);
    if(n != sizeof one)
    {
        //LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8 \n";
    }
}

// EventLoop的方法 =》 Poller的方法
void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(const Functor &functor : functors)
    {
        functor(); // 执行当前loop需要执行的回调操作
    }
    
    callingPendingFunctors_ = false;
}