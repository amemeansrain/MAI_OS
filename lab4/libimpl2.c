#include <stddef.h>
#include <stdlib.h>

// Площадь прямоугольного треугольника
float Square(float A, float B) {
    return (A * B) / 2.0f;  // Площадь прямоугольного треугольника
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