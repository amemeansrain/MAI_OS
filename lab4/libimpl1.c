#include <stddef.h>
#include <math.h>
#include <stdlib.h>

// Наивный алгоритм для подсчета простых чисел
int PrimeCount(int A, int B) {
    if (A < 2) A = 2;
    if (B < 2) return 0;
    
    int count = 0;
    for (int i = A; i <= B; i++) {
        int is_prime = 1;
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) count++;
    }
    return count;
}

// Пузырьковая сортировка
int* Sort(int* array) {
    if (!array) return NULL;
    
    // Первый элемент массива - размер
    int size = array[0];
    
    // Если размер не указан или он некорректен
    if (size <= 0) return array;
    
    // Сортируем массив (элементы с индексами 1..size)
    for (int i = 1; i <= size; i++) {
        for (int j = i + 1; j <= size; j++) {
            if (array[i] > array[j]) {
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
    return array;
}