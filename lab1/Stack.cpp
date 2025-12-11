#include "Stack.h"
#include <iostream>

//создаем стек
Stack* create_stack(int initial_capacity) {
    Stack* stack = new Stack;
    stack->data = new string[initial_capacity];
    stack->capacity = initial_capacity;
    stack->top = -1; // Стек пуст
    return stack;
}

//добавление в стек
bool stack_push(Stack* stack, const string& value) {
    if (stack->top >= stack->capacity - 1) {
        // Увеличиваем вместимость
        int new_capacity = stack->capacity * 2;
        string* new_data = new string[new_capacity];
        
        for (int i = 0; i <= stack->top; i++) {
            new_data[i] = stack->data[i];
        }
        
        //освобождаем старый и заменяем новым
        delete[] stack->data;
        stack->data = new_data;
        stack->capacity = new_capacity;
    }
    
    stack->top++;
    stack->data[stack->top] = value;
    return true;
}

//удаление
string stack_pop(Stack* stack) {
    if (stack->top < 0) {
        return ""; // Стек пуст
    }
    
    //сохранение элемента и уменьшение 
    string value = stack->data[stack->top];
    stack->top--;
    return value;
}

//вывод
string stack_peek(const Stack* stack) {
    if (stack->top < 0) {
        return ""; // Стек пуст
    }
    return stack->data[stack->top];
}

//проверка пустоты
bool stack_is_empty(const Stack* stack) {
    return stack->top < 0;
}

//кол-во
int stack_size(const Stack* stack) {
    return stack->top + 1;
}

//содержимое
void stack_print(const Stack* stack) {
    if (stack->top < 0) {
        cout << "стек пустой" << endl;
        return;
    }
    
    cout << "Стопка (сверху вниз) [" << (stack->top + 1) << "]: ";
    for (int i = stack->top; i >= 0; i--) {
        cout << stack->data[i];
        if (i > 0) {
            cout << " | ";
        }
    }
    cout << endl;
}

//очистка памяти
void stack_free(Stack* stack) {
    delete[] stack->data;
    delete stack;
}