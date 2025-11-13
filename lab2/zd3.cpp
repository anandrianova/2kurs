#include <iostream>
#include <string>
#include <cmath>
#include <climits>

using namespace std;

// Структура для динамического массива чисел
struct IntArray {
    int* data;      // Указатель на массив чисел
    int capacity;   // Вместимость массива
    int size;       // Текущий размер
};

// Создание массива
IntArray* create_int_array(int initial_capacity = 10) {
    IntArray* arr = new IntArray;
    arr->data = new int[initial_capacity];
    arr->capacity = initial_capacity;
    arr->size = 0;
    return arr;
}

// Добавление элемента в массив
void array_push_back(IntArray* arr, int value) {
    if (arr->size >= arr->capacity) {
        // Увеличиваем вместимость
        int new_capacity = arr->capacity * 2;
        int* new_data = new int[new_capacity];
        
        for (int i = 0; i < arr->size; i++) {
            new_data[i] = arr->data[i];
        }
        
        delete[] arr->data;
        arr->data = new_data;
        arr->capacity = new_capacity;
    }
    
    arr->data[arr->size] = value;
    arr->size++;
}

// Получение элемента по индексу
int array_get(const IntArray* arr, int index) {
    if (index < 0 || index >= arr->size) {
        return 0;
    }
    return arr->data[index];
}

// Вывод массива
void array_print(const IntArray* arr) {
    if (arr->size == 0) {
        cout << "массив пуст";
        return;
    }
    
    cout << "{";
    for (int i = 0; i < arr->size; i++) {
        cout << arr->data[i];
        if (i < arr->size - 1) {
            cout << ", ";
        }
    }
    cout << "}";
}

// Освобождение памяти массива
void array_free(IntArray* arr) {
    delete[] arr->data;
    delete arr;
}

// Функция для разбиения множества на два подмножества с минимальной разницей сумм
void find_min_difference_partition(IntArray* original, IntArray* subset1, IntArray* subset2) {
    int n = original->size;
    int total_sum = 0;
    
    // Вычисляем общую сумму всех элементов
    for (int i = 0; i < n; i++) {
        total_sum += array_get(original, i);
    }
    
    // Создаем DP таблицу: dp[i][j] = можно ли получить сумму j используя первые i элементов
    // Используем только одномерный массив для экономии памяти
    bool* dp_prev = new bool[total_sum + 1];
    bool* dp_curr = new bool[total_sum + 1];
    
    // Инициализация
    for (int j = 0; j <= total_sum; j++) {
        dp_prev[j] = false;
    }
    dp_prev[0] = true;  // Нулевую сумму можно получить всегда
    
    // Заполнение DP таблицы
    for (int i = 1; i <= n; i++) {
        int current_val = array_get(original, i - 1);
        
        for (int j = 0; j <= total_sum; j++) {
            dp_curr[j] = dp_prev[j];  // Не берем текущий элемент
            if (j >= current_val) {
                dp_curr[j] = dp_curr[j] || dp_prev[j - current_val];  // Берем текущий элемент
            }
        }
        
        // Обновляем предыдущую строку
        bool* temp = dp_prev;
        dp_prev = dp_curr;
        dp_curr = temp;
    }
    
    // Находим разбиение с минимальной разницей
    int min_diff = INT_MAX;
    int sum1 = 0;
    
    // Ищем максимальную сумму, не превышающую total_sum/2
    for (int j = total_sum / 2; j >= 0; j--) {
        if (dp_prev[j]) {
            min_diff = total_sum - 2 * j;
            sum1 = j;
            break;
        }
    }
    
    // Восстанавливаем подмножества
    int remaining_sum = sum1;
    bool* used = new bool[n]();  // Массив для отметки использованных элементов
    
    // Восстанавливаем первое подмножество
    int current_sum = sum1;
    for (int i = n - 1; i >= 0 && current_sum > 0; i--) {
        int current_val = array_get(original, i);
        if (current_sum >= current_val && dp_prev[current_sum - current_val]) {
            array_push_back(subset1, current_val);
            used[i] = true;
            current_sum -= current_val;
        }
    }
    
    // Второе подмножество - все неиспользованные элементы
    for (int i = 0; i < n; i++) {
        if (!used[i]) {
            array_push_back(subset2, array_get(original, i));
        }
    }
    
    // Выводим результаты
    cout << "Исходное множество: ";
    array_print(original);
    cout << endl;
    
    cout << "Подмножество 1: ";
    array_print(subset1);
    cout << " (сумма: " << sum1 << ")" << endl;
    
    cout << "Подмножество 2: ";
    array_print(subset2);
    cout << " (сумма: " << (total_sum - sum1) << ")" << endl;
    
    cout << "Минимальная разница сумм: " << min_diff << endl;
    
    // Освобождаем память
    delete[] dp_prev;
    delete[] dp_curr;
    delete[] used;
}

int main() {
    IntArray* original = create_int_array();
    int n, value;
    
    cout << "=== Разбиение множества на схожие подмножества ===" << endl;
    cout << "Введите количество элементов в множестве: ";
    cin >> n;
    
    cout << "Введите " << n << " натуральных чисел:" << endl;
    for (int i = 0; i < n; i++) {
        cin >> value;
        array_push_back(original, value);
    }
    
    IntArray* subset1 = create_int_array();
    IntArray* subset2 = create_int_array();
    
    find_min_difference_partition(original, subset1, subset2);
    
    array_free(original);
    array_free(subset1);
    array_free(subset2);
    
    return 0;
}