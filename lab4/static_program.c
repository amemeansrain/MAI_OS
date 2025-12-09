#include <stdio.h>
#include <stdlib.h>
#include "contract.h"

int main() {
    printf("Программа со статическим связыванием (библиотека 1)\n");
    printf("Доступные команды:\n");
    printf("  '1 A B' - подсчет простых чисел на отрезке [A, B]\n");
    printf("  '2 n a1 a2 ... an' - сортировка массива из n элементов\n");
    printf("  'exit' - выход из программы\n\n");
    
    char command[100];
    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) break;
        
        if (command[0] == '1') {
            int A, B;
            if (sscanf(command, "1 %d %d", &A, &B) == 2) {
                int result = PrimeCount(A, B);
                printf("Количество простых чисел на отрезке [%d, %d]: %d\n", A, B, result);
            } else {
                printf("Ошибка ввода. Используйте формат: 1 A B\n");
            }
        } else if (command[0] == '2') {
            int n;
            if (sscanf(command, "2 %d", &n) != 1) {
                printf("Ошибка ввода. Используйте формат: 2 n a1 a2 ... an\n");
                continue;
            }
            
            // Размер массива + элементы
            int size = n + 1;
            int* array = (int*)malloc(size * sizeof(int));
            if (!array) {
                printf("Ошибка выделения памяти\n");
                continue;
            }
            
            array[0] = n;  // Первый элемент - размер
            int index = 1;
            char* ptr = command;
            
            // Пропускаем "2 n"
            for (int i = 0; i < 2; i++) {
                while (*ptr != ' ' && *ptr != '\0') ptr++;
                if (*ptr == '\0') break;
                ptr++;  // Пропускаем пробел
            }
            
            // Читаем элементы массива
            for (int i = 0; i < n; i++) {
                if (sscanf(ptr, "%d", &array[index]) != 1) {
                    printf("Ошибка чтения элемента %d\n", i + 1);
                    free(array);
                    break;
                }
                index++;
                
                while (*ptr != ' ' && *ptr != '\0') ptr++;
                if (*ptr != '\0') ptr++;
            }
            
            if (index == n + 1) {
                // Вызываем функцию сортировки
                int* result = Sort(array);
                
                printf("Отсортированный массив: ");
                for (int i = 1; i <= n; i++) {
                    printf("%d ", result[i]);
                }
                printf("\n");
            }
            
            free(array);
        } else if (strncmp(command, "exit", 4) == 0) {
            break;
        } else {
            printf("Неизвестная команда\n");
        }
    }
    
    return 0;
}