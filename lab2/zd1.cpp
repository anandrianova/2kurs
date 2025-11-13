#include <iostream>
#include <string>
#include <cctype>

using namespace std;

// Структура стека на основе массива
struct Stack {
    string* data;     // Указатель на массив элементов
    int capacity;     // Максимальная вместимость стека
    int top;          // Индекс верхнего элемента (-1 если стек пуст)
};

// Создание стека
Stack* create_stack(int initial_capacity = 10) {
    Stack* stack = new Stack;
    stack->data = new string[initial_capacity];
    stack->capacity = initial_capacity;
    stack->top = -1;  // Стек пуст
    return stack;
}

// Добавление элемента в стек
bool stack_push(Stack* stack, const string& value) {
    // Проверяем, не заполнен ли стек
    if (stack->top >= stack->capacity - 1) {
        // Увеличиваем вместимость в 2 раза
        int new_capacity = stack->capacity * 2;
        string* new_data = new string[new_capacity];
        
        // Копируем старые данные
        for (int i = 0; i <= stack->top; i++) {
            new_data[i] = stack->data[i];
        }
        
        // Освобождаем старую память
        delete[] stack->data;
        stack->data = new_data;
        stack->capacity = new_capacity;
    }
    
    // Увеличиваем индекс верхнего элемента и добавляем значение
    stack->top++;
    stack->data[stack->top] = value;
    return true;
}

// Удаление и возврат верхнего элемента
string stack_pop(Stack* stack) {
    if (stack->top < 0) {
        return "";  // Стек пуст
    }
    
    // Сохраняем значение верхнего элемента
    string value = stack->data[stack->top];
    // Уменьшаем индекс верхнего элемента
    stack->top--;
    return value;
}

// Просмотр верхнего элемента без удаления
string stack_peek(const Stack* stack) {
    if (stack->top < 0) {
        return "";  // Стек пуст
    }
    return stack->data[stack->top];
}

// Проверка пустоты стека
bool stack_is_empty(const Stack* stack) {
    return stack->top < 0;
}

// Освобождение памяти стека
void stack_free(Stack* stack) {
    delete[] stack->data;
    delete stack;
}

// Функция для определения приоритета операций
int get_priority(const string& op) {
    if (op == "!") return 3;  // Отрицание - высший приоритет
    if (op == "&") return 2;  // Логическое И
    if (op == "|" || op == "^") return 1;  // Логическое ИЛИ и XOR
    return 0;  // Для скобок и других символов
}

// Функция для выполнения логической операции
string apply_operation(const string& a, const string& b, const string& op) {
    bool val_a = (a == "1");
    bool val_b = (b == "1");
    bool result = false;
    
    if (op == "&") {
        result = val_a && val_b;
    } else if (op == "|") {
        result = val_a || val_b;
    } else if (op == "^") {
        result = val_a != val_b;  // XOR
    }
    
    return result ? "1" : "0";
}

// Функция для выполнения унарной операции (отрицания)
string apply_unary_operation(const string& a, const string& op) {
    if (op == "!") {
        return (a == "1") ? "0" : "1";
    }
    return a;
}

// Основная функция вычисления логического выражения
string evaluate_expression(const string& expression) {
    Stack* values = create_stack();     // Стек для значений (0 и 1)
    Stack* operators = create_stack();  // Стек для операторов
    
    for (size_t i = 0; i < expression.length(); i++) {
        char c = expression[i];
        
        // Пропускаем пробелы
        if (isspace(c)) continue;
        
        // Если цифра 0 или 1 - добавляем в стек значений
        if (c == '0' || c == '1') {
            stack_push(values, string(1, c));
        }
        // Если открывающая скобка - добавляем в стек операторов
        else if (c == '(') {
            stack_push(operators, "(");
        }
        // Если закрывающая скобка
        else if (c == ')') {
            // Обрабатываем все операторы до открывающей скобки
            while (!stack_is_empty(operators) && stack_peek(operators) != "(") {
                string op = stack_pop(operators);
                
                // Обработка унарного оператора !
                if (op == "!") {
                    string val = stack_pop(values);
                    string result = apply_unary_operation(val, op);
                    stack_push(values, result);
                } else {
                    // Бинарные операторы
                    string val2 = stack_pop(values);
                    string val1 = stack_pop(values);
                    string result = apply_operation(val1, val2, op);
                    stack_push(values, result);
                }
            }
            // Удаляем открывающую скобку
            if (!stack_is_empty(operators)) {
                stack_pop(operators);
            }
        }
        // Если оператор
        else if (c == '!' || c == '&' || c == '|' || c == '^') {
            string current_op(1, c);
            
            // Обрабатываем операторы с более высоким или равным приоритетом
            while (!stack_is_empty(operators) && 
                   get_priority(stack_peek(operators)) >= get_priority(current_op)) {
                string op = stack_pop(operators);
                
                if (op == "!") {
                    string val = stack_pop(values);
                    string result = apply_unary_operation(val, op);
                    stack_push(values, result);
                } else {
                    string val2 = stack_pop(values);
                    string val1 = stack_pop(values);
                    string result = apply_operation(val1, val2, op);
                    stack_push(values, result);
                }
            }
            // Добавляем текущий оператор в стек
            stack_push(operators, current_op);
        }
    }
    
    // Обрабатываем оставшиеся операторы
    while (!stack_is_empty(operators)) {
        string op = stack_pop(operators);
        
        if (op == "!") {
            string val = stack_pop(values);
            string result = apply_unary_operation(val, op);
            stack_push(values, result);
        } else {
            string val2 = stack_pop(values);
            string val1 = stack_pop(values);
            string result = apply_operation(val1, val2, op);
            stack_push(values, result);
        }
    }
    
    // Результат - последнее значение в стеке
    string result = stack_pop(values);
    
    // Освобождаем память
    stack_free(values);
    stack_free(operators);
    
    return result;
}

// Функция для преобразования логического значения в текст
string bool_to_text(const string& bool_val) {
    return (bool_val == "1") ? "true" : "false";
}

int main() {
    string expression;
    
    cout << "=== Калькулятор логических выражений ===" << endl;
    cout << "Доступные операции:" << endl;
    cout << "! - отрицание (НЕ)" << endl;
    cout << "& - логическое И" << endl;
    cout << "| - логическое ИЛИ" << endl;
    cout << "^ - исключающее ИЛИ (XOR)" << endl;
    cout << "Пример: 1 & (0 | 1) ^ !0" << endl;
    cout << "Введите логическое выражение: ";
    
    getline(cin, expression);
    
    string result = evaluate_expression(expression);
    
    cout << "Выражение: " << expression << endl;
    cout << "Результат: " << bool_to_text(result) << " (" << result << ")" << endl;
    
    return 0;
}