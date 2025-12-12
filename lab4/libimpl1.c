#include <stddef.h>
#include <stdlib.h>

// Площадь прямоугольника
float Square(float A, float B) {
    return A * B;  // Площадь прямоугольника
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