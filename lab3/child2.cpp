#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "./funcs/funcs.h"

#define BUFFER_SIZE 1024

struct SharedData {
    char data[BUFFER_SIZE];
    int data_ready;
    int terminate;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <shm_name>" << std::endl;
        return 1;
    }
    
    // Открываем файл для записи
    std::ofstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << argv[1] << std::endl;
        return 1;
    }
    
    std::cout << "Child2: файл " << argv[1] << " открыт для записи" << std::endl;
    
    // Открываем shared memory для чтения
    int shm_fd = shm_open(argv[2], O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Ошибка открытия shared memory: " << argv[2] << std::endl;
        return 1;
    }
    
    SharedData* shm_ptr = (SharedData*)mmap(NULL, sizeof(SharedData), 
                                          PROT_READ | PROT_WRITE, 
                                          MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::cerr << "Ошибка отображения shared memory" << std::endl;
        return 1;
    }
    
    std::cout << "Child2: ожидание данных из shared memory..." << std::endl;
    
    // Основной цикл чтения из shared memory
    while (true) {
        // Ждем, пока появятся данные для обработки
        while (shm_ptr->data_ready == 0) {
            usleep(1000);
            
            // Проверяем флаг завершения
            if (shm_ptr->terminate == 1) {
                std::cout << "Child2: получен сигнал завершения" << std::endl;
                goto cleanup;
            }
        }
        
        // Копируем данные из shared memory
        std::string line(shm_ptr->data);
        
        // Сбрасываем флаг готовности данных
        shm_ptr->data_ready = 0;
        
        // Проверяем флаг завершения
        if (shm_ptr->terminate == 1) {
            std::cout << "Child2: получен сигнал завершения" << std::endl;
            goto cleanup;
        }
        
        // Обрабатываем строку
        std::string processed = removeVowels(line);
        
        std::cout << "Child2 получено: \"" << line << "\" -> \"" << processed << "\"" << std::endl;
        
        // Записываем результат в файл
        file << processed << std::endl;
        file.flush();
    }
    
cleanup:
    // Освобождение ресурсов
    file.close();
    munmap(shm_ptr, sizeof(SharedData));
    close(shm_fd);
    
    std::cout << "Child2 завершил работу" << std::endl;
    return 0;
}