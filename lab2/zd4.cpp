#include <iostream>
#include <string>

using namespace std;

// Структура для динамического массива
struct Array {
    int* data;      // Указатель на массив чисел
    int capacity;   // Вместимость массива
    int size;       // Текущий размер
};

// Структура для хранения информации о подмассивах
struct SubarrayInfo {
    int start;      // Начальный индекс
    int end;        // Конечный индекс
    int sum;        // Сумма подмассива
};

// Создание массива
Array* create_array(int initial_capacity = 10) {
    Array* arr = new Array;
    arr->data = new int[initial_capacity];
    arr->capacity = initial_capacity;
    arr->size = 0;
    return arr;
}

// Добавление элемента в массив
void array_push_back(Array* arr, int value) {
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
int array_get(const Array* arr, int index) {
    if (index < 0 || index >= arr->size) {
        return 0;
    }
    return arr->data[index];
}

// Вывод массива
void array_print(const Array* arr) {
    if (arr->size == 0) {
        cout << "массив пуст";
        return;
    }
    
    cout << "[";
    for (int i = 0; i < arr->size; i++) {
        cout << arr->data[i];
        if (i < arr->size - 1) {
            cout << ", ";
        }
    }
    cout << "]";
}

// Вывод подмассива
void print_subarray(const Array* arr, int start, int end) {
    cout << "[";
    for (int i = start; i <= end; i++) {
        cout << array_get(arr, i);
        if (i < end) {
            cout << ", ";
        }
    }
    cout << "]";
}

// Освобождение памяти массива
void array_free(Array* arr) {
    delete[] arr->data;
    delete arr;
}

// Функция для поиска всех подмассивов с заданной суммой
void find_subarrays_with_sum(Array* arr, int target_sum) {
    int n = arr->size;
    int found_count = 0;
    
    cout << "Поиск подмассивов с суммой " << target_sum << ":" << endl;
    
    // Метод 1: Перебор всех возможных подмассивов (O(n^2))
    for (int start = 0; start < n; start++) {
        int current_sum = 0;
        
        for (int end = start; end < n; end++) {
            current_sum += array_get(arr, end);
            
            if (current_sum == target_sum) {
                found_count++;
                cout << "Подмассив " << found_count << ": ";
                print_subarray(arr, start, end);
                cout << " (индексы " << start << "-" << end << ")" << endl;
            }
        }
    }
    
    if (found_count == 0) {
        cout << "Подмассивов с суммой " << target_sum << " не найдено" << endl;
    } else {
        cout << "Всего найдено подмассивов: " << found_count << endl;
    }
}

// Функция для поиска подмассивов с использованием хеширования (более эффективный метод)
void find_subarrays_with_sum_efficient(Array* arr, int target_sum) {
    int n = arr->size;
    int found_count = 0;
    
    // Создаем структуру для хранения префиксных сумм
    struct PrefixEntry {
        int sum;        // Префиксная сумма
        int index;      // Индекс, на котором заканчивается префикс
        PrefixEntry* next;  // Указатель на следующий элемент (для разрешения коллизий)
    };
    
    // Создаем хеш-таблицу для префиксных сумм
    int hash_size = n + 1;
    PrefixEntry** hash_table = new PrefixEntry*[hash_size]();
    
    // Добавляем нулевую префиксную сумму (перед первым элементом)
    PrefixEntry* zero_entry = new PrefixEntry;
    zero_entry->sum = 0;
    zero_entry->index = -1;
    zero_entry->next = nullptr;
    hash_table[0] = zero_entry;
    
    int prefix_sum = 0;
    
    cout << "Эффективный поиск подмассивов с суммой " << target_sum << ":" << endl;
    
    for (int i = 0; i < n; i++) {
        prefix_sum += array_get(arr, i);
        
        // Вычисляем хеш для текущей префиксной суммы
        int hash_index = abs(prefix_sum) % hash_size;
        
        // Добавляем текущую префиксную сумму в хеш-таблицу
        PrefixEntry* new_entry = new PrefixEntry;
        new_entry->sum = prefix_sum;
        new_entry->index = i;
        new_entry->next = hash_table[hash_index];
        hash_table[hash_index] = new_entry;
        
        // Ищем префиксную сумму, которая дает target_sum при вычитании
        int target_prefix = prefix_sum - target_sum;
        int target_hash = abs(target_prefix) % hash_size;
        
        // Проверяем все записи с таким хешем
        PrefixEntry* current = hash_table[target_hash];
        while (current != nullptr) {
            if (current->sum == target_prefix) {
                found_count++;
                cout << "Подмассив " << found_count << ": ";
                print_subarray(arr, current->index + 1, i);
                cout << " (индексы " << current->index + 1 << "-" << i << ")" << endl;
            }
            current = current->next;
        }
    }
    
    if (found_count == 0) {
        cout << "Подмассивов с суммой " << target_sum << " не найдено" << endl;
    } else {
        cout << "Всего найдено подмассивов: " << found_count << endl;
    }
    
    // Освобождаем память хеш-таблицы
    for (int i = 0; i < hash_size; i++) {
        PrefixEntry* current = hash_table[i];
        while (current != nullptr) {
            PrefixEntry* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] hash_table;
}

int main() {
    Array* arr = create_array();
    int n, value, target_sum;
    
    cout << "=== Поиск подмассивов с заданной суммой ===" << endl;
    cout << "Введите количество элементов в массиве: ";
    cin >> n;
    
    cout << "Введите " << n << " целых чисел:" << endl;
    for (int i = 0; i < n; i++) {
        cin >> value;
        array_push_back(arr, value);
    }
    
    cout << "Введите целевую сумму: ";
    cin >> target_sum;
    
    cout << "Исходный массив: ";
    array_print(arr);
    cout << endl;
    
    // Простой метод (для демонстрации)
    find_subarrays_with_sum(arr, target_sum);
    
    cout << endl;
    
    // Эффективный метод
    find_subarrays_with_sum_efficient(arr, target_sum);
    
    array_free(arr);
    return 0;
}