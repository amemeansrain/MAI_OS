#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

// Определение типов функций
typedef int (*PrimeCountFunc)(int, int);
typedef int* (*SortFunc)(int*);

int main() {
    void* handle = NULL;
    PrimeCountFunc prime_count = NULL;
    SortFunc sort_func = NULL;
    
    int current_lib = 1;  // 1 - libimpl1, 2 - libimpl2
    char command[100];
    
    printf("Программа с динамической загрузкой библиотек\n");
    printf("Доступные команды:\n");
    printf("  '0' - переключить реализацию (текущая: %d)\n", current_lib);
    printf("  '1 A B' - подсчет простых чисел на отрезке [A, B]\n");
    printf("  '2 n a1 a2 ... an' - сортировка массива из n элементов\n");
    printf("  'exit' - выход из программы\n\n");
    
    // Загружаем первую библиотеку
    handle = dlopen("./libimpl1.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }
    
    prime_count = (PrimeCountFunc)dlsym(handle, "PrimeCount");
    sort_func = (SortFunc)dlsym(handle, "Sort");
    
    if (!prime_count || !sort_func) {
        fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }
    
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
            
            prime_count = (PrimeCountFunc)dlsym(handle, "PrimeCount");
            sort_func = (SortFunc)dlsym(handle, "Sort");
            
            if (!prime_count || !sort_func) {
                fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
                dlclose(handle);
                return 1;
            }
            
            printf("Переключено на библиотеку %d\n", current_lib);
            
        } else if (command[0] == '1') {
            int A, B;
            if (sscanf(command, "1 %d %d", &A, &B) == 2) {
                int result = prime_count(A, B);
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
                int* result = sort_func(array);
                
                printf("Отсортированный массив: ");
                for (int i = 1; i <= n; i++) {
                    printf("%d ", result[i]);
                }
                printf("\n");
            }
            
            free(array);
            
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Неизвестная команда\n");
        }
    }
    
    if (handle) dlclose(handle);
    return 0;
}