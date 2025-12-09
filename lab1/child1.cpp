#include <iostream>
#include <fstream>
#include <string>
#include "./funcs/funcs.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    
    std::ofstream file(argv[1]);
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