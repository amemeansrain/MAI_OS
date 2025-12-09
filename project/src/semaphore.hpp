#pragma once
#include <string>
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore(const std::string& name, int count);
    
    // Основные операции
    void acquire();
    bool tryAcquire();
    void release();
    
    // Getters
    std::string getName() const { return name_; }
    int getCount() const { return count_; }
    int getMaxCount() const { return max_count_; }
    
private:
    std::string name_;
    int count_;
    int max_count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};