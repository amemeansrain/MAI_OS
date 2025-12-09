#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <array>

namespace utils {
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    std::string trim(const std::string& str) {
        auto start = str.begin();
        while (start != str.end() && std::isspace(*start)) {
            start++;
        }
        
        auto end = str.end();
        do {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));
        
        return std::string(start, end + 1);
    }
    
    bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    int executeCommand(const std::string& command, std::string& output) {
        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        
        output.clear();
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            output += buffer.data();
        }
        
        return pclose(pipe.release());
    }
    
    long long currentTimeMillis() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
}