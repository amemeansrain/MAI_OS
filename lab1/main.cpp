#include <iostream>
#include <unistd.h>

int main {
    int pipe1[2], pipe2[2]

    if (pipe1 < 0 || pipe2 < 0) {
        perror("Pipe failed");
        exit(1);
    }

    std::string file1, file2;

    std::cout << "Введите имя первого файла: "
    std::getline(std::cin, file1);

    std::cout << "Введите имя второго файла: "
    std::getline(std::cin, file2);

    pid_t pid1 = fork();
    
    if (!pid1) {
        close pid1[1];
        close pid2[0];
        close pid2[1];

        dup2(pipe1[0], 0);
        close(pipe1[0]);

        execl("./child1", "child1", NULL);
        perror("execl child1");
        return 1;
    }
    
    pid_t pid2 = fork();

    if (!pid2) {
        close pid2[1];
        close pid1[0];
        close pid1[1];

        dup2(pipe2[0], 0);
        close(pipe2[0]);

        execl("./child2", "child2", NULL);
        perror("execl child2");
        return 1;
    }

    close(pipe1[0]);
    close(pipe2[0]);

    std::cout << "Введите строки (для выхода введите пустую строку): " << std::endl;

    std::string inputString;

    while(true) {
        std::getline(std::cin, inputString);

        if (!inputString.length()) {
            break;
        }

        if (inputString.length() < 10) {
            write(pipe1[1], inputString, inputString.length());
            write(pipe1[1], "\n", 1);
        } else {
            write(pipe2[1], inputString, inputString.length());
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