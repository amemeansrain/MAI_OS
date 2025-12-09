#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include "job.hpp"

class DAG {
public:
    DAG();
    
    // Добавление задач
    void addJob(std::shared_ptr<Job> job);
    
    // Построение графа зависимостей
    void buildDependencies();
    
    // Проверки корректности
    bool hasCycles() const;
    bool hasSingleConnectedComponent() const;
    bool hasStartAndEndJobs() const;
    bool isValid() const;
    
    // Получение информации о графе
    std::vector<std::shared_ptr<Job>> getStartJobs() const;
    std::vector<std::shared_ptr<Job>> getReadyJobs() const;
    std::shared_ptr<Job> getJob(const std::string& id) const;
    std::vector<std::shared_ptr<Job>> getAllJobs() const;
    
    // Обновление состояния после выполнения задачи
    void onJobCompleted(const std::string& job_id);
    
    // Остановка всех задач
    void cancelAllJobs();
    
private:
    std::unordered_map<std::string, std::shared_ptr<Job>> jobs_;
    std::unordered_map<std::string, std::vector<std::string>> adjacency_list_;
    std::unordered_map<std::string, std::vector<std::string>> reverse_adjacency_;
    
    // Вспомогательные методы для проверки циклов
    bool dfsCycleCheck(const std::string& node, 
                      std::unordered_map<std::string, bool>& visited,
                      std::unordered_map<std::string, bool>& rec_stack) const;
    
    // Поиск в глубину для проверки связности
    void dfsConnectivity(const std::string& node,
                        std::unordered_map<std::string, bool>& visited) const;
};