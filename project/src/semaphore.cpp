#include "semaphore.hpp"
#include <iostream>

Semaphore::Semaphore(const std::string& name, int count) 
    : name_(name), count_(count), max_count_(count) {}

void Semaphore::acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return count_ > 0; });
    count_--;
}

bool Semaphore::tryAcquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (count_ > 0) {
        count_--;
        return true;
    }
    return false;
}

void Semaphore::release() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (count_ < max_count_) {
        count_++;
        cv_.notify_one();
    }
}