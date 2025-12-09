#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "dag.hpp"

class Scheduler {
public:
    Scheduler(int max_concurrent_jobs = 4);
    ~Scheduler();
    
    // Запуск планировщика
    void schedule(std::unique_ptr<DAG> dag);
    
    // Остановка планировщика
    void stop();
    
    // Статус
    bool isRunning() const { return running_; }
    bool hasErrors() const { return has_errors_; }
    
private:
    // Основной цикл планировщика
    void run();
    
    // Запуск задачи в отдельном потоке
    void runJob(std::shared_ptr<Job> job);
    
    // Обработка завершения задачи
    void onJobCompleted(std::shared_ptr<Job> job);
    
private:
    std::unique_ptr<DAG> dag_;
    int max_concurrent_jobs_;
    
    std::atomic<bool> running_;
    std::atomic<bool> has_errors_;
    std::atomic<int> active_jobs_;
    
    std::thread scheduler_thread_;
    
    // Синхронизация
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::queue<std::shared_ptr<Job>> ready_queue_;
    
    std::mutex jobs_mutex_;
    std::vector<std::thread> job_threads_;
};