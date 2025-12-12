#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contract.h"

int main() {
    printf("Программа со статическим связыванием (библиотека 1)\n");
    printf("Доступные команды:\n");
    printf("  '1 A B' - вычисление площади фигуры по сторонам A и B\n");
    printf("  '2 n a1 a2 ... an' - сортировка массива из n элементов\n");
    printf("  'exit' - выход из программы\n");
    printf("Текущая реализация: Прямоугольник (A * B)\n\n");
    
    char command[100];
    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) break;
        
        // Удаляем символ новой строки
        command[strcspn(command, "\n")] = 0;
        
        if (command[0] == '1') {
            float A, B;
            if (sscanf(command, "1 %f %f", &A, &B) == 2) {
                float result = Square(A, B);
                printf("Площадь фигуры (прямоугольник) со сторонами %.2f и %.2f: %.2f\n", A, B, result);
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
            int read_count = 0;
            for (int i = 0; i < n; i++) {
                if (sscanf(ptr, "%d", &array[index]) == 1) {
                    read_count++;
                    index++;
                }
                
                while (*ptr != ' ' && *ptr != '\0') ptr++;
                if (*ptr != '\0') ptr++;
            }
            
            if (read_count == n) {
                // Вызываем функцию сортировки
                int* result = Sort(array);
                
                printf("Отсортированный массив (пузырьковая сортировка): ");
                for (int i = 1; i <= n; i++) {
                    printf("%d ", result[i]);
                }
                printf("\n");
            } else {
                printf("Ошибка: прочитано %d элементов из %d\n", read_count, n);
            }
            
            free(array);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Неизвестная команда. Используйте: 1, 2 или exit\n");
        }
    }
    
    printf("Программа завершена.\n");
    return 0;
}