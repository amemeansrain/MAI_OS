#include "dag.hpp"
#include <iostream>
#include <queue>
#include <algorithm>

DAG::DAG() {}

void DAG::addJob(std::shared_ptr<Job> job) {
    jobs_[job->getId()] = job;
}

void DAG::buildDependencies() {
    adjacency_list_.clear();
    reverse_adjacency_.clear();
    
    // Строим списки смежности
    for (const auto& pair : jobs_) {
        const auto& job = pair.second;
        adjacency_list_[job->getId()] = {};
        
        for (const auto& dep : job->getDependencies()) {
            adjacency_list_[dep].push_back(job->getId());
            reverse_adjacency_[job->getId()].push_back(dep);
        }
    }
}

bool DAG::dfsCycleCheck(const std::string& node,
                       std::unordered_map<std::string, bool>& visited,
                       std::unordered_map<std::string, bool>& rec_stack) const {
    if (!visited[node]) {
        visited[node] = true;
        rec_stack[node] = true;
        
        for (const auto& neighbor : adjacency_list_.at(node)) {
            if (!visited[neighbor] && dfsCycleCheck(neighbor, visited, rec_stack)) {
                return true;
            } else if (rec_stack[neighbor]) {
                return true;
            }
        }
    }
    
    rec_stack[node] = false;
    return false;
}

bool DAG::hasCycles() const {
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, bool> rec_stack;
    
    for (const auto& pair : jobs_) {
        visited[pair.first] = false;
        rec_stack[pair.first] = false;
    }
    
    for (const auto& pair : jobs_) {
        if (dfsCycleCheck(pair.first, visited, rec_stack)) {
            return true;
        }
    }
    
    return false;
}

void DAG::dfsConnectivity(const std::string& node,
                         std::unordered_map<std::string, bool>& visited) const {
    visited[node] = true;
    
    // Проверяем прямые зависимости
    if (adjacency_list_.count(node)) {
        for (const auto& neighbor : adjacency_list_.at(node)) {
            if (!visited[neighbor]) {
                dfsConnectivity(neighbor, visited);
            }
        }
    }
    
    // Проверяем обратные зависимости
    if (reverse_adjacency_.count(node)) {
        for (const auto& neighbor : reverse_adjacency_.at(node)) {
            if (!visited[neighbor]) {
                dfsConnectivity(neighbor, visited);
            }
        }
    }
}

bool DAG::hasSingleConnectedComponent() const {
    if (jobs_.empty()) return true;
    
    std::unordered_map<std::string, bool> visited;
    for (const auto& pair : jobs_) {
        visited[pair.first] = false;
    }
    
    // Начинаем с первого узла
    auto first_node = jobs_.begin()->first;
    dfsConnectivity(first_node, visited);
    
    // Проверяем, все ли узлы посещены
    for (const auto& pair : visited) {
        if (!pair.second) {
            return false;
        }
    }
    
    return true;
}

bool DAG::hasStartAndEndJobs() const {
    bool has_start = false;
    bool has_end = false;
    
    for (const auto& pair : jobs_) {
        const auto& job = pair.second;
        
        // Стартовая задача: нет зависимостей
        if (job->getDependencies().empty()) {
            has_start = true;
        }
        
        // Конечная задача: никто от неё не зависит
        bool is_end = true;
        for (const auto& other_pair : jobs_) {
            const auto& other_job = other_pair.second;
            if (other_job->hasDependency(job->getId())) {
                is_end = false;
                break;
            }
        }
        
        if (is_end) {
            has_end = true;
        }
        
        if (has_start && has_end) {
            return true;
        }
    }
    
    return has_start && has_end;
}

bool DAG::isValid() const {
    return !hasCycles() && hasSingleConnectedComponent() && hasStartAndEndJobs();
}

std::vector<std::shared_ptr<Job>> DAG::getStartJobs() const {
    std::vector<std::shared_ptr<Job>> start_jobs;
    
    for (const auto& pair : jobs_) {
        const auto& job = pair.second;
        if (job->getDependencies().empty()) {
            start_jobs.push_back(job);
        }
    }
    
    return start_jobs;
}

std::vector<std::shared_ptr<Job>> DAG::getReadyJobs() const {
    std::vector<std::shared_ptr<Job>> ready_jobs;
    
    for (const auto& pair : jobs_) {
        const auto& job = pair.second;
        if (job->getStatus() == JobStatus::PENDING) {
            // Проверяем, все ли зависимости выполнены
            bool all_deps_completed = true;
            for (const auto& dep : job->getDependencies()) {
                auto dep_job = getJob(dep);
                if (dep_job && 
                    dep_job->getStatus() != JobStatus::SUCCESS && 
                    dep_job->getStatus() != JobStatus::CANCELLED) {
                    all_deps_completed = false;
                    break;
                }
            }
            
            if (all_deps_completed) {
                job->setStatus(JobStatus::READY);
                ready_jobs.push_back(job);
            }
        } else if (job->getStatus() == JobStatus::READY) {
            ready_jobs.push_back(job);
        }
    }
    
    return ready_jobs;
}

std::shared_ptr<Job> DAG::getJob(const std::string& id) const {
    auto it = jobs_.find(id);
    if (it != jobs_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Job>> DAG::getAllJobs() const {
    std::vector<std::shared_ptr<Job>> all_jobs;
    for (const auto& pair : jobs_) {
        all_jobs.push_back(pair.second);
    }
    return all_jobs;
}

void DAG::onJobCompleted(const std::string& job_id) {
    auto job = getJob(job_id);
    if (job) {
        // Обновляем статусы зависимых задач будет в getReadyJobs
    }
}

void DAG::cancelAllJobs() {
    for (const auto& pair : jobs_) {
        auto job = pair.second;
        if (job->getStatus() == JobStatus::PENDING || 
            job->getStatus() == JobStatus::READY ||
            job->getStatus() == JobStatus::RUNNING) {
            job->setStatus(JobStatus::CANCELLED);
            
            // Освобождаем семафор, если был захвачен
            if (job->getSemaphore()) {
                job->getSemaphore()->release();
            }
        }
    }
}