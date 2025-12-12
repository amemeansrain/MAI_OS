#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

// Определение типов функций
typedef float (*SquareFunc)(float, float);
typedef int* (*SortFunc)(int*);

int main() {
    void* handle = NULL;
    SquareFunc square_func = NULL;
    SortFunc sort_func = NULL;
    
    int current_lib = 1;  // 1 - libimpl1, 2 - libimpl2
    char* lib_names[] = {"Прямоугольник", "Прямоугольный треугольник"};
    char* sort_names[] = {"Пузырьковая", "Быстрая (Хоара)"};
    char command[100];
    
    printf("Программа с динамической загрузкой библиотек (Вариант 35)\n");
    printf("Доступные команды:\n");
    printf("  '0' - переключить реализацию\n");
    printf("  '1 A B' - вычисление площади фигуры по сторонам A и B\n");
    printf("  '2 n a1 a2 ... an' - сортировка массива из n элементов\n");
    printf("  'exit' - выход из программы\n\n");
    
    // Загружаем первую библиотеку
    handle = dlopen("./libimpl1.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }
    
    square_func = (SquareFunc)dlsym(handle, "Square");
    sort_func = (SortFunc)dlsym(handle, "Sort");
    
    if (!square_func || !sort_func) {
        fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }
    
    printf("Текущая реализация: %s (площадь), %s (сортировка)\n\n", 
           lib_names[current_lib - 1], sort_names[current_lib - 1]);
    
    while (1) {
        printf("[lib%d]> ", current_lib);
        if (fgets(command, sizeof(command), stdin) == NULL) break;
        
        // Удаляем символ новой строки
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "0") == 0) {
            // Переключаем библиотеку
            dlclose(handle);
            
            if (current_lib == 1) {
                handle = dlopen("./libimpl2.so", RTLD_LAZY);
                current_lib = 2;
            } else {
                handle = dlopen("./libimpl1.so", RTLD_LAZY);
                current_lib = 1;
            }
            
            if (!handle) {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }
            
            square_func = (SquareFunc)dlsym(handle, "Square");
            sort_func = (SortFunc)dlsym(handle, "Sort");
            
            if (!square_func || !sort_func) {
                fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
                dlclose(handle);
                return 1;
            }
            
            printf("Переключено на реализацию: %s (площадь), %s (сортировка)\n\n", 
                   lib_names[current_lib - 1], sort_names[current_lib - 1]);
            
        } else if (command[0] == '1') {
            float A, B;
            if (sscanf(command, "1 %f %f", &A, &B) == 2) {
                float result = square_func(A, B);
                printf("Площадь %s со сторонами %.2f и %.2f: %.2f\n", 
                       lib_names[current_lib - 1], A, B, result);
            } else {
                printf("Ошибка ввода. Используйте формат: 1 A B\n");
            }
            
        } else if (command[0] == '2') {
            int n;
            if (sscanf(command, "2 %d", &n) != 1) {
                printf("Ошибка ввода. Используйте формат: 2 n a1 a2 ... an\n");
                continue;
            }
            
            int size = n + 1;
            int* array = (int*)malloc(size * sizeof(int));
            if (!array) {
                printf("Ошибка выделения памяти\n");
                continue;
            }
            
            array[0] = n;
            int index = 1;
            char* ptr = command;
            
            // Пропускаем "2 n"
            for (int i = 0; i < 2; i++) {
                while (*ptr != ' ' && *ptr != '\0') ptr++;
                if (*ptr == '\0') break;
                ptr++;
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
                int* result = sort_func(array);
                
                printf("Отсортированный массив (%s сортировка): ", 
                       sort_names[current_lib - 1]);
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
        } else if (strlen(command) > 0) {
            printf("Неизвестная команда. Используйте: 0, 1, 2 или exit\n");
        }
    }
    
    if (handle) dlclose(handle);
    printf("Программа завершена.\n");
    return 0;
}