#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>  // Добавлен для exit()
#include <cstring>  // Добавлен для работы со строками

int main() {
    int pipe1[2], pipe2[2];  // Добавлена точка с запятой

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("Pipe failed");
        exit(1);
    }

    std::string file1, file2;

    std::cout << "Введите имя первого файла: ";
    std::getline(std::cin, file1);

    std::cout << "Введите имя второго файла: ";
    std::getline(std::cin, file2);

    pid_t pid1 = fork();
    
    if (pid1 == 0) {  // Исправлено условие
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        dup2(pipe1[0], 0);
        close(pipe1[0]);

        execl("./child1", "child1", file1.c_str(), NULL);  // Добавлен filename
        perror("execl child1");
        return 1;
    }
    
    pid_t pid2 = fork();

    if (pid2 == 0) {  // Исправлено условие
        close(pipe2[1]);  // Исправлено: pipe2[1] вместо pid2[1]
        close(pipe1[0]);  // Исправлено: pipe1[0] вместо pid1[0]
        close(pipe1[1]);  // Исправлено: pipe1[1] вместо pid1[1]

        dup2(pipe2[0], 0);
        close(pipe2[0]);

        execl("./child2", "child2", file2.c_str(), NULL);  // Добавлен filename
        perror("execl child2");
        return 1;
    }

    close(pipe1[0]);
    close(pipe2[0]);

    std::cout << "Введите строки (для выхода введите пустую строку): " << std::endl;

    std::string inputString;

    while(true) {
        std::cout << "> ";  // Добавлен prompt для удобства
        std::getline(std::cin, inputString);

        if (inputString.empty()) {  // Более идиоматичная проверка
            break;
        }

        if (inputString.length() < 10) {
            write(pipe1[1], inputString.c_str(), inputString.length());  // Добавлен .c_str()
            write(pipe1[1], "\n", 1);
        } else {
            write(pipe2[1], inputString.c_str(), inputString.length());  // Добавлен .c_str()
            write(pipe2[1], "\n", 1);
        }
    }

    close(pipe1[1]);
    close(pipe2[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    std::cout << "Родительский процесс завершен." << std::endl;
    return 0;
}