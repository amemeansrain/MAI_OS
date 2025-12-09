#pragma once
#include <string>
#include <vector>
#include <memory>
#include <atomic>

enum class JobStatus {
    PENDING,
    READY,
    RUNNING,
    SUCCESS,
    FAILED,
    CANCELLED
};

class Semaphore;

class Job {
public:
    Job(const std::string& id, const std::string& command);
    
    // Getters
    std::string getId() const { return id_; }
    std::string getCommand() const { return command_; }
    JobStatus getStatus() const { return status_; }
    std::vector<std::string> getDependencies() const { return dependencies_; }
    std::shared_ptr<Semaphore> getSemaphore() const { return semaphore_; }
    int getExitCode() const { return exit_code_; }
    
    // Setters
    void setDependencies(const std::vector<std::string>& deps) { dependencies_ = deps; }
    void setSemaphore(std::shared_ptr<Semaphore> sem) { semaphore_ = sem; }
    
    // Методы управления состоянием
    void setStatus(JobStatus status);
    bool isReady() const;
    bool canRun() const;
    void execute();
    
    // Проверка зависимостей
    void addDependency(const std::string& job_id);
    bool hasDependency(const std::string& job_id) const;
    
private:
    std::string id_;
    std::string command_;
    JobStatus status_;
    std::vector<std::string> dependencies_;
    std::shared_ptr<Semaphore> semaphore_;
    std::atomic<int> exit_code_;
    
    // Время выполнения
    long long start_time_;
    long long end_time_;
};