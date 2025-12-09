#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>

#define BUFFER_SIZE 1024

// Структура для разделяемой памяти
struct SharedData {
    char data[BUFFER_SIZE];      // Данные для передачи
    int data_ready;              // Флаг готовности данных (1 - готово, 0 - прочитано)
    int terminate;               // Флаг завершения процесса
};

int main() {
    std::string file1, file2;
    
    std::cout << "Введите имя первого файла: ";
    std::getline(std::cin, file1);
    
    std::cout << "Введите имя второго файла: ";
    std::getline(std::cin, file2);
    
    // Создание разделяемой памяти для child1
    int shm_fd1 = shm_open("/shm_child1", O_CREAT | O_RDWR, 0666);
    if (shm_fd1 == -1) {
        perror("shm_open child1");
        exit(1);
    }
    
    if (ftruncate(shm_fd1, sizeof(SharedData)) == -1) {
        perror("ftruncate child1");
        exit(1);
    }
    
    SharedData* shm_ptr1 = (SharedData*)mmap(NULL, sizeof(SharedData), 
                                           PROT_READ | PROT_WRITE, 
                                           MAP_SHARED, shm_fd1, 0);
    if (shm_ptr1 == MAP_FAILED) {
        perror("mmap child1");
        exit(1);
    }
    
    // Создание разделяемой памяти для child2
    int shm_fd2 = shm_open("/shm_child2", O_CREAT | O_RDWR, 0666);
    if (shm_fd2 == -1) {
        perror("shm_open child2");
        exit(1);
    }
    
    if (ftruncate(shm_fd2, sizeof(SharedData)) == -1) {
        perror("ftruncate child2");
        exit(1);
    }
    
    SharedData* shm_ptr2 = (SharedData*)mmap(NULL, sizeof(SharedData), 
                                           PROT_READ | PROT_WRITE, 
                                           MAP_SHARED, shm_fd2, 0);
    if (shm_ptr2 == MAP_FAILED) {
        perror("mmap child2");
        exit(1);
    }
    
    // Инициализация структур shared memory
    memset(shm_ptr1, 0, sizeof(SharedData));
    memset(shm_ptr2, 0, sizeof(SharedData));
    
    // Создание первого дочернего процесса
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // child1
        execl("./child1", "child1", file1.c_str(), "/shm_child1", NULL);
        perror("execl child1");
        return 1;
    }
    
    // Создание второго дочернего процесса
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // child2
        execl("./child2", "child2", file2.c_str(), "/shm_child2", NULL);
        perror("execl child2");
        return 1;
    }
    
    // Даем время дочерним процессам инициализироваться
    sleep(1);
    
    std::cout << "Введите строки (для выхода введите пустую строку): " << std::endl;
    
    std::string inputString;
    
    while(true) {
        std::cout << "> ";
        std::getline(std::cin, inputString);
        
        if (inputString.empty()) {
            break;
        }
        
        if (inputString.length() < 10) {
            // Отправляем в child1
            // Ждем, пока child1 прочитает предыдущие данные
            while (shm_ptr1->data_ready == 1) {
                usleep(1000);
            }
            
            strncpy(shm_ptr1->data, inputString.c_str(), BUFFER_SIZE - 1);
            shm_ptr1->data[BUFFER_SIZE - 1] = '\0';
            shm_ptr1->data_ready = 1;
            
            std::cout << "Отправлено в child1: " << inputString << std::endl;
        } else {
            // Отправляем в child2
            // Ждем, пока child2 прочитает предыдущие данные
            while (shm_ptr2->data_ready == 1) {
                usleep(1000);
            }
            
            strncpy(shm_ptr2->data, inputString.c_str(), BUFFER_SIZE - 1);
            shm_ptr2->data[BUFFER_SIZE - 1] = '\0';
            shm_ptr2->data_ready = 1;
            
            std::cout << "Отправлено в child2: " << inputString << std::endl;
        }
    }
    
    // Сигнализируем дочерним процессам о завершении
    // Ждем, пока процессы прочитают последние данные
    while (shm_ptr1->data_ready == 1) {
        usleep(1000);
    }
    while (shm_ptr2->data_ready == 1) {
        usleep(1000);
    }
    
    // Устанавливаем флаги завершения
    shm_ptr1->terminate = 1;
    shm_ptr2->terminate = 1;
    
    // Устанавливаем флаги готовности, чтобы процессы увидели флаг завершения
    shm_ptr1->data_ready = 1;
    shm_ptr2->data_ready = 1;
    
    // Ожидание завершения дочерних процессов
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    // Освобождение ресурсов shared memory
    munmap(shm_ptr1, sizeof(SharedData));
    munmap(shm_ptr2, sizeof(SharedData));
    close(shm_fd1);
    close(shm_fd2);
    shm_unlink("/shm_child1");
    shm_unlink("/shm_child2");
    
    std::cout << "Родительский процесс завершен." << std::endl;
    return 0;
}