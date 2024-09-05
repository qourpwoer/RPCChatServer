#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

#include "LogStream.h"
#include "BlockingQueue.h"
#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "Thread.h"

class AsyncLogging : noncopyable
{
public:
    AsyncLogging(const string &basename,
                 off_t rollSize,
                 int flushInterval = 3);
    ~AsyncLogging()
    {
        if (running_)
        {
            stop();
        }
    }

    void append(const char *logline, int len);

    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() //NO_THREAD_SAFETY_ANALYSIS
    {
        running_ = false;
        cond_.notify_one();
        thread_.join();
    }

private:
    void threadFunc();

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    const int flushInterval_;
    std::atomic<bool> running_;
    const string basename_;
    const off_t rollSize_;
    Thread thread_;
    CountDownLatch latch_;
    mutex mutex_;
    condition_variable cond_; //GUARDED_BY(mutex_);
    BufferPtr currentBuffer_; //GUARDED_BY(mutex_);
    BufferPtr nextBuffer_; //GUARDED_BY(mutex_);
    BufferVector buffers_; //GUARDED_BY(mutex_);
};