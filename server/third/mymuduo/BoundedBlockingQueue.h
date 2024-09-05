#pragma once

#include <condition_variable>
#include <mutex>
#include <assert.h>
#include "CircularQueue.h"

template <typename T>
class BoundedBlockingQueue : noncopyable
{
public:
    explicit BoundedBlockingQueue(int maxSize)
        : mutex_(),
          notEmpty_(mutex_),
          notFull_(mutex_),
          queue_(maxSize)
    {
    }

    void put(const T &x)
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.full())
        {
            notFull_.wait(lock);
        }
        assert(!queue_.full());
        queue_.push_back(x);
        notEmpty_.notify_one();
    }

    void put(T &&x)
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.full())
        {
            notFull_.wait(lock);
        }
        assert(!queue_.full());
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
        notFull_.notify_one();
        return std::move(front);
    }

    bool empty() const
    {
        unique_lock<mutex> lock(mutex_);
        return queue_.empty();
    }

    bool full() const
    {
        unique_lock<mutex> lock(mutex_);
        return queue_.full();
    }

    size_t size() const
    {
        unique_lock<mutex> lock(mutex_);
        return queue_.size();
    }

    size_t capacity() const
    {
        unique_lock<mutex> lock(mutex_);
        return queue_.capacity();
    }

private:
    mutable mutex mutex_;
    condition_variable notEmpty_; //GUARDED_BY(mutex_);
    condition_variable notFull_; //GUARDED_BY(mutex_);
    CircularQueue<T> queue_ ;//GUARDED_BY(mutex_);
};