#pragma once
#include <string>
#include <vector>
#include <chrono>

namespace utils {
    // Разделение строки по разделителю
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Обрезка пробелов
    std::string trim(const std::string& str);
    
    // Проверка существования файла
    bool fileExists(const std::string& path);
    
    // Запуск команды и получение результата
    int executeCommand(const std::string& command, std::string& output);
    
    // Текущее время в миллисекундах
    long long currentTimeMillis();
}