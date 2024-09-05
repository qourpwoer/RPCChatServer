#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

// EventLoop: ChannelList Poller
Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj; // weak_ptr观察shared_ptr
    tied_ = true;
}

/*
*  当改变channel所表示的fd的events事件后，update负责再poller里面更改
*   EventLoop -> ChannelList Poller
*/
void Channel::update()
{
    // 通过channel所属的EventLoop，调用poller的响应方法
    loop_->updateChannel(this);
}

//  在channel所属的EventLoop中，把当前的channel删除掉
void Channel::remove()
{
    // add code
    loop_->removeChannel(this);
}

// 得到poller通知以后，处理事件
void Channel::handleEvent(Timestamp receiveTime)
{
    if(tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();//提升指针
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

// 根据poller通知的channel发生的具体事件，由channel负责调用具有的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    //LOG_INFO("channel handleEvent revents:%d\n", revents_);
    LOG_INFO << "channel handleEvent revents:" << revents_ << " \n";
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) //异常
    {   
        if(closeCallback_)
        {
            closeCallback_();
        }
    }

    if(revents_ & EPOLLERR)
    {
        if(errorCallback_)
        {
            errorCallback_();
        }
    }

    if(revents_ & (EPOLLIN | EPOLLPRI))
    {
        if(readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if(revents_ & EPOLLOUT)
    {
        if(writeCallback_)
        {
            writeCallback_();
        }
    }
}