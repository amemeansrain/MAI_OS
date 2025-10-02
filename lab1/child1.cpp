#include <iostream>
#include <fstream>  // Добавьте этот заголовочный файл
#include <string>

// Временная реализация функции removeVowels
std::string removeVowels(const std::string& str) {
    std::string result;
    std::string vowels = "aeiouyAEIOUYаеёиоуыэюяАЕЁИОУЫЭЮЯ";
    
    for (char c : str) {
        if (vowels.find(c) == std::string::npos) {
            result += c;
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    
    std::ofstream file(argv[1]);  // Теперь это должно работать
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << argv[1] << std::endl;
        return 1;
    }
    
    std::cout << "Child1: файл " << argv[1] << " открыт для записи" << std::endl;
    
    std::string line;

    while (std::getline(std::cin, line)) {
        std::string processed = removeVowels(line);
        
        std::cout << "Child1 получено: \"" << line << "\" -> \"" << processed << "\"" << std::endl;
        
        file << processed << std::endl;
        file.flush();
    }
    
    file.close();
    std::cout << "Child1 завершил работу" << std::endl;
    return 0;
}