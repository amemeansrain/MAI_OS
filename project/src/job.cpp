#include "job.hpp"
#include "semaphore.hpp"
#include "utils.hpp"
#include <iostream>
#include <thread>
#include <chrono>

Job::Job(const std::string& id, const std::string& command) 
    : id_(id), command_(command), status_(JobStatus::PENDING), 
      exit_code_(-1), start_time_(0), end_time_(0) {}

void Job::setStatus(JobStatus status) {
    status_ = status;
    if (status == JobStatus::RUNNING) {
        start_time_ = utils::currentTimeMillis();
    } else if (status == JobStatus::SUCCESS || status == JobStatus::FAILED) {
        end_time_ = utils::currentTimeMillis();
    }
}

bool Job::isReady() const {
    return status_ == JobStatus::READY;
}

bool Job::canRun() const {
    if (!semaphore_) {
        return isReady();
    }
    return isReady() && semaphore_->tryAcquire();
}

void Job::addDependency(const std::string& job_id) {
    dependencies_.push_back(job_id);
}

bool Job::hasDependency(const std::string& job_id) const {
    for (const auto& dep : dependencies_) {
        if (dep == job_id) {
            return true;
        }
    }
    return false;
}

void Job::execute() {
    setStatus(JobStatus::RUNNING);
    std::cout << "[INFO] Starting job: " << id_ << " (" << command_ << ")" << std::endl;
    
    std::string output;
    exit_code_ = utils::executeCommand(command_, output);
    
    if (!output.empty()) {
        std::cout << "[OUTPUT][" << id_ << "]\n" << output << std::endl;
    }
    
    if (exit_code_ == 0) {
        setStatus(JobStatus::SUCCESS);
        std::cout << "[INFO] Job " << id_ << " completed successfully" << std::endl;
    } else {
        setStatus(JobStatus::FAILED);
        std::cerr << "[ERROR] Job " << id_ << " failed with exit code: " << exit_code_ << std::endl;
    }
    
    // Освобождаем семафор
    if (semaphore_) {
        semaphore_->release();
    }
}