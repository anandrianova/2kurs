#include "Array.h"
#include <iostream>
#include <stdexcept>

//создание динамического массива 
Array* create_array(int initial_capacity) {
    Array* arr = new Array; 
    arr->data = new string[initial_capacity];
    arr->capacity = initial_capacity;
    arr->size = 0; //первоночальное кол-во элементов
    return arr;
}

//добавление в конец 
void array_push_back(Array* arr, const string& value) {
    //проверка для увеличения памяти
    if (arr->size >= arr->capacity) {
        int new_capacity = arr->capacity * 2;
        string* new_data = new string[new_capacity];
        
        for (int i = 0; i < arr->size; i++) {
            new_data[i] = arr->data[i];
        }
        
        delete[] arr->data;
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    
    //добавление элементов и увеличение счетчика размера
    arr->data[arr->size] = value;
    arr->size++;
}

//вставка по индексу 
bool array_insert(Array* arr, int index, const string& value) {
    //проверка на значения индекса
    if (index < 0 || index > arr->size) {
        return false; 
    }
    
    //увеличение массива, если заполнен
    if (arr->size >= arr->capacity) {
        int new_capacity = arr->capacity * 2;
        string* new_data = new string[new_capacity];
        
        for (int i = 0; i < index; i++) {
            new_data[i] = arr->data[i];
        }
        
        new_data[index] = value;
        
        for (int i = index; i < arr->size; i++) {
            new_data[i + 1] = arr->data[i];
        }
        
        //замена старого на новый
        delete[] arr->data;
        arr->data = new_data;
        arr->capacity = new_capacity;
    } else {
        // Сдвигаем элементы вправо
        for (int i = arr->size; i > index; i--) {
            arr->data[i] = arr->data[i - 1];
        }
        arr->data[index] = value; //добавление элемента
    }
    
    arr->size++;
    return true;
}

//поиск по индексу 
string array_get(const Array* arr, int index) {
    //проверка индекса
    if (index < 0 || index >= arr->size) {
        return ""; // Пустая строка при неверном индексе
    }
    return arr->data[index];
}

//удаление по индексу
bool array_remove(Array* arr, int index) {
    //проверка индекса
    if (index < 0 || index >= arr->size) {
        return false;
    }
    
    //сдвиг элементов влево
    for (int i = index; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    
    arr->size--; //уменьшение размера
    return true;
}

//вставка по индексу 
bool array_replace(Array* arr, int index, const string& value) {
    //проверка индекса
    if (index < 0 || index >= arr->size) {
        return false;
    }
    
    arr->data[index] = value; //заменв
    return true;
}

//вывод кол-ва элементов 
int array_length(const Array* arr) {
    return arr->size;
}

//вывод массива
void array_print(const Array* arr) {
    if (arr->size == 0) {
        cout << "массив пуст" << endl;
        return;
    }
    
    cout << "массив [" << arr->size << "]: ";
    //вывод элементов
    for (int i = 0; i < arr->size; i++) {
        cout << arr->data[i];
        if (i < arr->size - 1) {
            cout << ", ";
        }
    }
    cout << endl;
}

//очистка памяти
void array_free(Array* arr) {
    delete[] arr->data;
    delete arr;
}