#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "dag.hpp"
#include "semaphore.hpp"

class JsonParser {
public:
    static std::unique_ptr<DAG> parseFromFile(const std::string& filename);
    static std::unique_ptr<DAG> parseFromString(const std::string& json_str);
    
private:
    static void parseSemaphores(const nlohmann::json& j, 
                               std::unordered_map<std::string, std::shared_ptr<Semaphore>>& semaphores);
    static void parseJobs(const nlohmann::json& j, 
                         std::unique_ptr<DAG>& dag,
                         const std::unordered_map<std::string, std::shared_ptr<Semaphore>>& semaphores);
};