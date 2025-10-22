#ifndef STACK_H
#define STACK_H

#include <string>
using namespace std;

// Структура стека на основе массива
struct Stack {
    string* data;// Данные стека
    int capacity;// Вместимость
    int top;// Индекс верхнего элемента
};

// Создание стека
Stack* create_stack(int initial_capacity = 10);

bool stack_push(Stack* stack, const string& value);// Добавление элемента (push)
string stack_pop(Stack* stack);// Удаление элемента (pop)
string stack_peek(const Stack* stack);// Чтение верхнего элемента без удаления
bool stack_is_empty(const Stack* stack);// Проверка на пустоту
int stack_size(const Stack* stack);// Получение размера
void stack_print(const Stack* stack);// Чтение всего стека
void stack_free(Stack* stack);// Освобождение памяти

#endif