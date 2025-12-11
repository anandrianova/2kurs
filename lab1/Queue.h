#ifndef QUEUE_H
#define QUEUE_H

#include <string>
using namespace std;

// Структура очереди на основе массива
struct Queue {
    string* data;// Данные очереди
    int capacity;// Вместимость
    int front;// Индекс начала
    int rear;// Индекс конца
    int size;// Текущий размер
};

// Создание очереди
Queue* create_queue(int initial_capacity = 10);

bool queue_push(Queue* queue, const string& value);// Добавление элемента (push)
string queue_pop(Queue* queue);// Удаление элемента (pop)
string queue_peek(const Queue* queue);// Чтение первого элемента без удаления
bool queue_is_empty(const Queue* queue);// Проверка на пустоту
int queue_size(const Queue* queue);// Получение размера
void queue_print(const Queue* queue);// Чтение всей очереди
void queue_free(Queue* queue);// Освобождение памяти

#endif