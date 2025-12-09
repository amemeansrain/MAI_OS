#include "scheduler.hpp"
#include <iostream>
#include <chrono>

Scheduler::Scheduler(int max_concurrent_jobs) 
    : max_concurrent_jobs_(max_concurrent_jobs), 
      running_(false), 
      has_errors_(false),
      active_jobs_(0) {}

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::schedule(std::unique_ptr<DAG> dag) {
    // Проверяем корректность DAG
    if (!dag->isValid()) {
        std::cerr << "[ERROR] Invalid DAG configuration!" << std::endl;
        
        if (dag->hasCycles()) {
            std::cerr << "  - DAG contains cycles" << std::endl;
        }
        if (!dag->hasSingleConnectedComponent()) {
            std::cerr << "  - DAG has multiple connected components" << std::endl;
        }
        if (!dag->hasStartAndEndJobs()) {
            std::cerr << "  - DAG missing start or end jobs" << std::endl;
        }
        
        throw std::runtime_error("Invalid DAG configuration");
    }
    
    dag_ = std::move(dag);
    running_ = true;
    has_errors_ = false;
    
    // Запускаем основной поток планировщика
    scheduler_thread_ = std::thread(&Scheduler::run, this);
}

void Scheduler::stop() {
    if (running_) {
        running_ = false;
        cv_.notify_all();
        
        if (scheduler_thread_.joinable()) {
            scheduler_thread_.join();
        }
        
        // Отменяем все задачи
        if (dag_) {
            dag_->cancelAllJobs();
        }
        
        // Ждём завершения всех потоков задач
        for (auto& thread : job_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        job_threads_.clear();
    }
}

void Scheduler::run() {
    std::cout << "[INFO] Scheduler started with max concurrent jobs: " 
              << max_concurrent_jobs_ << std::endl;
    
    while (running_ && !has_errors_) {
        // Проверяем готовые задачи
        auto ready_jobs = dag_->getReadyJobs();
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            for (auto& job : ready_jobs) {
                ready_queue_.push(job);
            }
        }
        
        // Запускаем задачи, если есть свободные слоты
        while (active_jobs_ < max_concurrent_jobs_ && !ready_queue_.empty()) {
            std::shared_ptr<Job> job;
            
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                if (!ready_queue_.empty()) {
                    job = ready_queue_.front();
                    ready_queue_.pop();
                }
            }
            
            if (job && job->canRun()) {
                active_jobs_++;
                
                // Запускаем задачу в отдельном потоке
                std::thread job_thread(&Scheduler::runJob, this, job);
                
                std::lock_guard<std::mutex> lock(jobs_mutex_);
                job_threads_.push_back(std::move(job_thread));
            } else if (job) {
                // Задача не может быть запущена сейчас (нет семафора)
                // Возвращаем её в очередь
                std::lock_guard<std::mutex> lock(queue_mutex_);
                ready_queue_.push(job);
            }
        }
        
        // Проверяем, все ли задачи завершены
        bool all_completed = true;
        for (const auto& job : dag_->getAllJobs()) {
            auto status = job->getStatus();
            if (status != JobStatus::SUCCESS && 
                status != JobStatus::FAILED && 
                status != JobStatus::CANCELLED) {
                all_completed = false;
                break;
            }
        }
        
        if (all_completed) {
            std::cout << "[INFO] All jobs completed" << std::endl;
            break;
        }
        
        // Небольшая задержка для уменьшения нагрузки на CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    if (has_errors_) {
        std::cerr << "[ERROR] Scheduler stopped due to job failure" << std::endl;
        stop();
    }
    
    running_ = false;
}

void Scheduler::runJob(std::shared_ptr<Job> job) {
    try {
        job->execute();
        onJobCompleted(job);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in job " << job->getId() << ": " << e.what() << std::endl;
        job->setStatus(JobStatus::FAILED);
        has_errors_ = true;
        onJobCompleted(job);
    }
    
    active_jobs_--;
}

void Scheduler::onJobCompleted(std::shared_ptr<Job> job) {
    if (job->getStatus() == JobStatus::FAILED) {
        has_errors_ = true;
        cv_.notify_all();
    } else {
        dag_->onJobCompleted(job->getId());
    }
}