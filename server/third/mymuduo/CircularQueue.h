#pragma once
using namespace std;


// 循环队列
template<class T>
class CircularQueue {
private:
    T *arr_;  // 存储队列元素的数组
    int front_; // 队首索引
    int rear_;  // 队尾索引
    int maxSize_; // 队列的最大容量
    int currentSize_; // 当前队列中元素个数
 
public:
    CircularQueue(int size) {
        maxSize_ = size;
        front_ = rear_ = -1;
        currentSize_ = 0;
        arr_ = new T[maxSize_];
    }
 
    ~CircularQueue() {
        delete[] arr_;
    }
 
    bool empty() {
        return currentSize_ == 0;
    }
 
    bool full() {
        return currentSize_ == maxSize_;
    }
 
    bool push_back(T&& data) {
        if (full()) {
            return false;
        }
 
        if (front_ == -1) {  // 首次入队操作
            front_ = 0;
        }
 
        rear_ = (rear_ + 1) % maxSize_;
        arr_[rear_] = data;
        currentSize_++;
        
        return true;
    }
 
    bool pop_front() {
        if (empty()) {
            return false;
        }
 
        if (front_ == rear_) {  // 最后一个元素出队
            front_ = rear_ = -1;
        } else {
            front_ = (front_ + 1) % maxSize_;
        }
 
        currentSize_--;
        return true;
    }
 
    T front() {
        if (empty()) {
            return -1;
        }
 
        return arr_[front_];
    }
 
    T rear() {
        if (empty()) {
            return -1;
        }
 
        return arr_[rear_];
    }

    size_t size()
    {
        return currentSize_;
    }

    size_t capacity()
    {
        return maxSize_;
    }
};
