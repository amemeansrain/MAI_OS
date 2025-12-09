#include "parser_json.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::unique_ptr<DAG> JsonParser::parseFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    json j;
    file >> j;
    return parseFromString(j.dump());
}

std::unique_ptr<DAG> JsonParser::parseFromString(const std::string& json_str) {
    auto j = json::parse(json_str);
    auto dag = std::make_unique<DAG>();
    
    // Парсим семафоры
    std::unordered_map<std::string, std::shared_ptr<Semaphore>> semaphores;
    if (j.contains("semaphores") && j["semaphores"].is_object()) {
        parseSemaphores(j["semaphores"], semaphores);
    }
    
    // Парсим задачи
    if (j.contains("jobs") && j["jobs"].is_array()) {
        parseJobs(j, dag, semaphores);
    }
    
    // Строим зависимости
    dag->buildDependencies();
    
    return dag;
}

void JsonParser::parseSemaphores(const json& j, 
                                std::unordered_map<std::string, std::shared_ptr<Semaphore>>& semaphores) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string name = it.key();
        int count = it.value().get<int>();
        
        if (count <= 0) {
            std::cerr << "[WARNING] Semaphore " << name << " has invalid count: " << count 
                      << ". Setting to 1." << std::endl;
            count = 1;
        }
        
        semaphores[name] = std::make_shared<Semaphore>(name, count);
        std::cout << "[INFO] Created semaphore: " << name << " with count: " << count << std::endl;
    }
}

void JsonParser::parseJobs(const json& j, 
                          std::unique_ptr<DAG>& dag,
                          const std::unordered_map<std::string, std::shared_ptr<Semaphore>>& semaphores) {
    for (const auto& job_json : j["jobs"]) {
        std::string id = job_json["id"].get<std::string>();
        std::string command = job_json["command"].get<std::string>();
        
        auto job = std::make_shared<Job>(id, command);
        
        // Парсим зависимости
        if (job_json.contains("deps") && job_json["deps"].is_array()) {
            std::vector<std::string> deps;
            for (const auto& dep : job_json["deps"]) {
                deps.push_back(dep.get<std::string>());
            }
            job->setDependencies(deps);
        }
        
        // Парсим семафор
        if (job_json.contains("semaphore") && job_json["semaphore"].is_string()) {
            std::string sem_name = job_json["semaphore"].get<std::string>();
            auto it = semaphores.find(sem_name);
            if (it != semaphores.end()) {
                job->setSemaphore(it->second);
            } else {
                std::cerr << "[WARNING] Semaphore " << sem_name << " not found for job " << id << std::endl;
            }
        }
        
        dag->addJob(job);
        std::cout << "[INFO] Added job: " << id << " (" << command << ")" << std::endl;
    }
}