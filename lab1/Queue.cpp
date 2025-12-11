#include "Queue.h"
#include <iostream>

//создание очереди
Queue* create_queue(int initial_capacity) {
    Queue* queue = new Queue;
    queue->data = new string[initial_capacity];
    queue->capacity = initial_capacity;
    queue->front = 0; //начало
    queue->rear = -1;//конец
    queue->size = 0;//пустая
    return queue;
}

//добавление
bool queue_push(Queue* queue, const string& value) {
    if (queue->size >= queue->capacity) {
        // Увеличиваем вместимость
        int new_capacity = queue->capacity * 2;
        string* new_data = new string[new_capacity];
        
        // Копируем элементы 
        for (int i = 0; i < queue->size; i++) {
            new_data[i] = queue->data[(queue->front + i) % queue->capacity];
        }
        
        // Освобождаем старый массив и заменяем его новым
        delete[] queue->data;
        queue->data = new_data;
        queue->capacity = new_capacity;
        queue->front = 0;
        queue->rear = queue->size - 1;
    }
    
    
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->data[queue->rear] = value;
    queue->size++;
    return true;
}

//удаление и возвращение первого элемента 
string queue_pop(Queue* queue) {
    if (queue->size == 0) {
        return ""; // Очередь пуста
    }
    
    //сохраниение элемента
    string value = queue->data[queue->front];
    //сдвиг 
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--; //уменьшение
    return value;
}

//вывод первого элемента
string queue_peek(const Queue* queue) {
    if (queue->size == 0) {
        return ""; // Очередь пуста
    }
    return queue->data[queue->front];//вывод первого элемента
}

//проверка пустоты
bool queue_is_empty(const Queue* queue) {
    return queue->size == 0;
}

//кол-во
int queue_size(const Queue* queue) {
    return queue->size;
}


//вывод элементов
void queue_print(const Queue* queue) {
    if (queue->size == 0) {
        cout << "очередь пуста" << endl;
        return;
    }
    
    cout << "Очередь (начало -> конец) [" << queue->size << "]: ";
    for (int i = 0; i < queue->size; i++) {
        int index = (queue->front + i) % queue->capacity;
        cout << queue->data[index];
        if (i < queue->size - 1) {
            cout << " -> ";
        }
    }
    cout << endl;
}

//очистка
void queue_free(Queue* queue) {
    delete[] queue->data;
    delete queue;
}