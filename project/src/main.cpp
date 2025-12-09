#include <iostream>
#include <string>
#include <memory>
#include "parser_json.hpp"
#include "scheduler.hpp"
#include "utils.hpp"

void printUsage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " <config_file.json> [max_concurrent_jobs]" << std::endl;
    std::cout << "  config_file.json      - JSON configuration file" << std::endl;
    std::cout << "  max_concurrent_jobs   - Maximum number of concurrent jobs (default: 4)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string config_file = argv[1];
    int max_concurrent_jobs = 4;
    
    if (argc > 2) {
        try {
            max_concurrent_jobs = std::stoi(argv[2]);
            if (max_concurrent_jobs <= 0) {
                std::cerr << "[ERROR] max_concurrent_jobs must be positive" << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Invalid max_concurrent_jobs value: " << e.what() << std::endl;
            return 1;
        }
    }
    
    // Проверяем существование файла
    if (!utils::fileExists(config_file)) {
        std::cerr << "[ERROR] Config file not found: " << config_file << std::endl;
        return 1;
    }
    
    try {
        std::cout << "==========================================" << std::endl;
        std::cout << "   DAG Job Scheduler (JSON + Semaphore)   " << std::endl;
        std::cout << "==========================================" << std::endl;
        
        // Парсим конфигурацию
        std::cout << "[INFO] Parsing configuration from: " << config_file << std::endl;
        auto dag = JsonParser::parseFromFile(config_file);
        
        // Создаем и запускаем планировщик
        Scheduler scheduler(max_concurrent_jobs);
        scheduler.schedule(std::move(dag));
        
        // Ждём завершения планировщика
        while (scheduler.isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        if (scheduler.hasErrors()) {
            std::cout << "[RESULT] DAG execution failed!" << std::endl;
            return 1;
        } else {
            std::cout << "[RESULT] DAG execution completed successfully!" << std::endl;
            return 0;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
}