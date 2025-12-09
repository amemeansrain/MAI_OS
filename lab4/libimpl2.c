#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Решето Эратосфена для подсчета простых чисел
int PrimeCount(int A, int B) {
    if (B < 2) return 0;
    if (A < 2) A = 2;
    
    int* sieve = (int*)malloc((B + 1) * sizeof(int));
    if (!sieve) return -1;
    
    // Инициализация решета
    for (int i = 0; i <= B; i++) {
        sieve[i] = 1;
    }
    sieve[0] = sieve[1] = 0;
    
    // Решето Эратосфена
    for (int i = 2; i * i <= B; i++) {
        if (sieve[i]) {
            for (int j = i * i; j <= B; j += i) {
                sieve[j] = 0;
            }
        }
    }
    
    // Подсчет простых чисел на отрезке [A, B]
    int count = 0;
    for (int i = A; i <= B; i++) {
        if (sieve[i]) count++;
    }
    
    free(sieve);
    return count;
}

// Быстрая сортировка Хоара
void quicksort(int* arr, int low, int high) {
    if (low < high) {
        int pivot = arr[(low + high) / 2];
        int i = low, j = high;
        
        while (i <= j) {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            if (i <= j) {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
                i++;
                j--;
            }
        }
        
        if (low < j) quicksort(arr, low, j);
        if (i < high) quicksort(arr, i, high);
    }
}

// Сортировка Хоара (быстрая сортировка)
int* Sort(int* array) {
    if (!array) return NULL;
    
    int size = array[0];
    if (size <= 0) return array;
    
    // Сортируем массив (элементы с индексами 1..size)
    quicksort(array, 1, size);
    
    return array;
}