#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
    : mutex_(),
      condition_(),
      count_(count)
{
}

void CountDownLatch::wait()
{
    unique_lock<mutex> lock(mutex_);
    while (count_ > 0)
    {
        condition_.wait(lock);
    }
}

void CountDownLatch::countDown()
{
    unique_lock<mutex> lock(mutex_);
    --count_;
    if (count_ == 0)
    {
        condition_.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    unique_lock<mutex> lock(mutex_);
    return count_;
}