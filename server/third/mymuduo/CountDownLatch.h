#pragma once
#include <condition_variable>
#include <mutex>
#include "noncopyable.h"

using namespace std;
class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable mutex mutex_;
    condition_variable condition_;//GUARDED_BY(mutex_);
    int count_;//GUARDED_BY(mutex_);
};
