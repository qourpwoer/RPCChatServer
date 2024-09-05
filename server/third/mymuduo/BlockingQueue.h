#pragma once

#include <condition_variable>
#include <mutex>

#include <deque>
#include <assert.h>

template <typename T>
class BlockingQueue : noncopyable
{
public:
    BlockingQueue()
        : mutex_(),
          notEmpty_(),
          queue_()
    {
    }

    void put(const T &x)
    {
        unique_lock<mutex> lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify_one();
    }

    void put(T &&x)
    {
        unique_lock<mutex> lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify_one();
    }

    T take()
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.empty())
        {
            notEmpty_.wait(lock);
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();
        return std::move(front);
    }

    size_t size() const
    {
        unique_lock<mutex> lock(mutex_);
        return queue_.size();
    }

private:
    mutable mutex mutex_;
    condition_variable notEmpty_;//GUARDED_BY(mutex_);
    std::deque<T> queue_;//GUARDED_BY(mutex_);
};
