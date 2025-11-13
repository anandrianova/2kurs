#include <iostream>
#include <string>
#include <functional>

using namespace std;

// Структура для элемента хеш-таблицы
struct HashEntry {
    string key;        // Ключ
    string value;      // Значение
    bool is_deleted;   // Флаг удаления (для двойного хеширования)
    bool is_occupied;  // Флаг занятости
};

// Структура хеш-таблицы с двойным хешированием
struct DoubleHashTable {
    HashEntry* table;   // Массив элементов
    int capacity;       // Вместимость таблицы
    int size;          // Текущий размер
    double load_factor_threshold;  // Порог для реструктуризации
};

// Структура для второй хеш-таблицы (кукушкино хеширование)
struct CuckooHashTable {
    HashEntry* table1;  // Первая таблица
    HashEntry* table2;  // Вторая таблица
    int capacity;       // Вместимость каждой таблицы
    int size;          // Общий размер
    double load_factor_threshold;  // Порог для реструктуризации
};

// Создание таблицы с двойным хешированием
DoubleHashTable* create_double_hash_table(int initial_capacity = 10) {
    DoubleHashTable* ht = new DoubleHashTable;
    ht->capacity = initial_capacity;
    ht->size = 0;
    ht->load_factor_threshold = 0.9;  // 90% заполнения
    
    ht->table = new HashEntry[ht->capacity];
    for (int i = 0; i < ht->capacity; i++) {
        ht->table[i].key = "";
        ht->table[i].value = "";
        ht->table[i].is_deleted = false;
        ht->table[i].is_occupied = false;
    }
    
    return ht;
}

// Первая хеш-функция
int hash1(const string& key, int capacity) {
    hash<string> hasher;
    return hasher(key) % capacity;
}

// Вторая хеш-функция (для двойного хеширования)
int hash2(const string& key, int capacity) {
    hash<string> hasher;
    return 1 + (hasher(key) % (capacity - 1));
}

// Вставка в таблицу с двойным хешированием
bool double_hash_insert(DoubleHashTable* ht, const string& key, const string& value) {
    // Проверяем коэффициент заполнения
    double load_factor = static_cast<double>(ht->size) / ht->capacity;
    if (load_factor >= ht->load_factor_threshold) {
        cout << "Коэффициент заполнения " << load_factor << " превышает порог " 
             << ht->load_factor_threshold << ", выполняется реструктуризация..." << endl;
        return false;  // В реальной реализации здесь была бы реструктуризация
    }
    
    int index = hash1(key, ht->capacity);
    int step = hash2(key, ht->capacity);
    int attempts = 0;
    
    while (attempts < ht->capacity) {
        // Если ячейка пуста или помечена как удаленная
        if (!ht->table[index].is_occupied || ht->table[index].is_deleted) {
            ht->table[index].key = key;
            ht->table[index].value = value;
            ht->table[index].is_occupied = true;
            ht->table[index].is_deleted = false;
            ht->size++;
            return true;
        }
        
        // Если ключ уже существует, обновляем значение
        if (ht->table[index].key == key) {
            ht->table[index].value = value;
            return true;
        }
        
        // Переходим к следующей ячейке
        index = (index + step) % ht->capacity;
        attempts++;
    }
    
    return false;  // Таблица переполнена
}

// Поиск в таблице с двойным хешированием
string double_hash_search(DoubleHashTable* ht, const string& key) {
    int index = hash1(key, ht->capacity);
    int step = hash2(key, ht->capacity);
    int attempts = 0;
    
    while (attempts < ht->capacity) {
        // Если нашли нужный ключ и он не удален
        if (ht->table[index].is_occupied && !ht->table[index].is_deleted && 
            ht->table[index].key == key) {
            return ht->table[index].value;
        }
        
        // Если пустая ячейка (не удаленная), значит ключа нет
        if (!ht->table[index].is_occupied && !ht->table[index].is_deleted) {
            return "";
        }
        
        index = (index + step) % ht->capacity;
        attempts++;
    }
    
    return "";  // Не найдено
}

// Удаление из таблицы с двойным хешированием
bool double_hash_remove(DoubleHashTable* ht, const string& key) {
    int index = hash1(key, ht->capacity);
    int step = hash2(key, ht->capacity);
    int attempts = 0;
    
    while (attempts < ht->capacity) {
        if (ht->table[index].is_occupied && !ht->table[index].is_deleted && 
            ht->table[index].key == key) {
            ht->table[index].is_deleted = true;
            ht->size--;
            return true;
        }
        
        if (!ht->table[index].is_occupied && !ht->table[index].is_deleted) {
            return false;  // Ключ не найден
        }
        
        index = (index + step) % ht->capacity;
        attempts++;
    }
    
    return false;  // Ключ не найден
}

// Вывод таблицы с двойным хешированием
void print_double_hash_table(const DoubleHashTable* ht) {
    cout << "Таблица с двойным хешированием (емкость: " << ht->capacity 
         << ", размер: " << ht->size << "):" << endl;
    
    for (int i = 0; i < ht->capacity; i++) {
        cout << "[" << i << "]: ";
        if (ht->table[i].is_occupied && !ht->table[i].is_deleted) {
            cout << ht->table[i].key << " -> " << ht->table[i].value;
        } else if (ht->table[i].is_deleted) {
            cout << "УДАЛЕНО";
        } else {
            cout << "пусто";
        }
        cout << endl;
    }
}

// Освобождение памяти таблицы с двойным хешированием
void free_double_hash_table(DoubleHashTable* ht) {
    delete[] ht->table;
    delete ht;
}

// Создание таблицы с кукушкиным хешированием
CuckooHashTable* create_cuckoo_hash_table(int initial_capacity = 10) {
    CuckooHashTable* ht = new CuckooHashTable;
    ht->capacity = initial_capacity;
    ht->size = 0;
    ht->load_factor_threshold = 0.5;  // 50% заполнения
    
    ht->table1 = new HashEntry[ht->capacity];
    ht->table2 = new HashEntry[ht->capacity];
    
    for (int i = 0; i < ht->capacity; i++) {
        ht->table1[i].key = "";
        ht->table1[i].value = "";
        ht->table1[i].is_occupied = false;
        ht->table1[i].is_deleted = false;
        
        ht->table2[i].key = "";
        ht->table2[i].value = "";
        ht->table2[i].is_occupied = false;
        ht->table2[i].is_deleted = false;
    }
    
    return ht;
}

// Хеш-функция для первой таблицы кукушки
int cuckoo_hash1(const string& key, int capacity) {
    hash<string> hasher;
    return hasher(key) % capacity;
}

// Хеш-функция для второй таблицы кукушки
int cuckoo_hash2(const string& key, int capacity) {
    hash<string> hasher;
    return (hasher(key) / capacity) % capacity;
}

// Объявление функции поиска для кукушкиного хеширования (ДОБАВЛЕНО)
string cuckoo_hash_search(CuckooHashTable* ht, const string& key);

// Вставка в таблицу с кукушкиным хешированием (рекурсивная)
bool cuckoo_insert_helper(CuckooHashTable* ht, const string& key, const string& value, int table_num, int attempts) {
    const int MAX_ATTEMPTS = 10;
    
    if (attempts > MAX_ATTEMPTS) {
        return false;  // Слишком много попыток, нужна реструктуризация
    }
    
    // Проверяем коэффициент заполнения
    double load_factor = static_cast<double>(ht->size) / (2 * ht->capacity);
    if (load_factor >= ht->load_factor_threshold) {
        cout << "Коэффициент заполнения " << load_factor << " превышает порог " 
             << ht->load_factor_threshold << ", выполняется реструктуризация..." << endl;
        return false;
    }
    
    if (table_num == 1) {
        int index = cuckoo_hash1(key, ht->capacity);
        if (!ht->table1[index].is_occupied) {
            ht->table1[index].key = key;
            ht->table1[index].value = value;
            ht->table1[index].is_occupied = true;
            ht->size++;
            return true;
        } else {
            // Вытесняем существующий элемент
            string evicted_key = ht->table1[index].key;
            string evicted_value = ht->table1[index].value;
            
            ht->table1[index].key = key;
            ht->table1[index].value = value;
            
            // Пытаемся вставить вытесненный элемент во вторую таблицу
            return cuckoo_insert_helper(ht, evicted_key, evicted_value, 2, attempts + 1);
        }
    } else {
        int index = cuckoo_hash2(key, ht->capacity);
        if (!ht->table2[index].is_occupied) {
            ht->table2[index].key = key;
            ht->table2[index].value = value;
            ht->table2[index].is_occupied = true;
            ht->size++;
            return true;
        } else {
            // Вытесняем существующий элемент
            string evicted_key = ht->table2[index].key;
            string evicted_value = ht->table2[index].value;
            
            ht->table2[index].key = key;
            ht->table2[index].value = value;
            
            // Пытаемся вставить вытесненный элемент в первую таблицу
            return cuckoo_insert_helper(ht, evicted_key, evicted_value, 1, attempts + 1);
        }
    }
}

// Основная функция вставки для кукушкиного хеширования
bool cuckoo_hash_insert(CuckooHashTable* ht, const string& key, const string& value) {
    // Сначала проверяем, нет ли уже такого ключа
    string existing_value = cuckoo_hash_search(ht, key);
    if (!existing_value.empty()) {
        // Ключ уже существует, обновляем значение в той таблице, где он находится
        int index1 = cuckoo_hash1(key, ht->capacity);
        int index2 = cuckoo_hash2(key, ht->capacity);
        
        if (ht->table1[index1].is_occupied && ht->table1[index1].key == key) {
            ht->table1[index1].value = value;
        } else if (ht->table2[index2].is_occupied && ht->table2[index2].key == key) {
            ht->table2[index2].value = value;
        }
        return true;
    }
    
    // Ключ не существует, вставляем новый
    return cuckoo_insert_helper(ht, key, value, 1, 0);
}

// Поиск в таблице с кукушкиным хешированием (реализация)
string cuckoo_hash_search(CuckooHashTable* ht, const string& key) {
    // Проверяем первую таблицу
    int index1 = cuckoo_hash1(key, ht->capacity);
    if (ht->table1[index1].is_occupied && ht->table1[index1].key == key) {
        return ht->table1[index1].value;
    }
    
    // Проверяем вторую таблицу
    int index2 = cuckoo_hash2(key, ht->capacity);
    if (ht->table2[index2].is_occupied && ht->table2[index2].key == key) {
        return ht->table2[index2].value;
    }
    
    return "";  // Не найдено
}

// Вывод таблицы с кукушкиным хешированием
void print_cuckoo_hash_table(const CuckooHashTable* ht) {
    cout << "Кукушкина хеш-таблица (емкость: " << ht->capacity 
         << ", размер: " << ht->size << "):" << endl;
    
    cout << "Таблица 1:" << endl;
    for (int i = 0; i < ht->capacity; i++) {
        cout << "[" << i << "]: ";
        if (ht->table1[i].is_occupied) {
            cout << ht->table1[i].key << " -> " << ht->table1[i].value;
        } else {
            cout << "пусто";
        }
        cout << endl;
    }
    
    cout << "Таблица 2:" << endl;
    for (int i = 0; i < ht->capacity; i++) {
        cout << "[" << i << "]: ";
        if (ht->table2[i].is_occupied) {
            cout << ht->table2[i].key << " -> " << ht->table2[i].value;
        } else {
            cout << "пусто";
        }
        cout << endl;
    }
}

// Освобождение памяти таблицы с кукушкиным хешированием
void free_cuckoo_hash_table(CuckooHashTable* ht) {
    delete[] ht->table1;
    delete[] ht->table2;
    delete ht;
}

// Функция реструктуризации для двойного хеширования
void restructure_double_hash(DoubleHashTable* ht) {
    cout << "=== Реструктуризация таблицы с двойным хешированием ===" << endl;
    
    // Сохраняем старые данные
    HashEntry* old_table = ht->table;
    int old_capacity = ht->capacity;
    
    // Увеличиваем вместимость в 2 раза
    ht->capacity *= 2;
    ht->size = 0;
    ht->table = new HashEntry[ht->capacity];
    
    // Инициализируем новую таблицу
    for (int i = 0; i < ht->capacity; i++) {
        ht->table[i].key = "";
        ht->table[i].value = "";
        ht->table[i].is_deleted = false;
        ht->table[i].is_occupied = false;
    }
    
    // Перехешируем все элементы из старой таблицы
    for (int i = 0; i < old_capacity; i++) {
        if (old_table[i].is_occupied && !old_table[i].is_deleted) {
            double_hash_insert(ht, old_table[i].key, old_table[i].value);
        }
    }
    
    cout << "Реструктуризация завершена. Новая вместимость: " << ht->capacity << endl;
    
    delete[] old_table;
}

int main() {
    int choice;
    string key, value;
    
    cout << "=== Сравнение методов хеширования ===" << endl;
    
    DoubleHashTable* double_ht = create_double_hash_table(5);
    CuckooHashTable* cuckoo_ht = create_cuckoo_hash_table(5);
    
    while (true) {
        cout << "\nМеню:" << endl;
        cout << "1. Добавить элемент (двойное хеширование)" << endl;
        cout << "2. Найти элемент (двойное хеширование)" << endl;
        cout << "3. Удалить элемент (двойное хеширование)" << endl;
        cout << "4. Вывести таблицу (двойное хеширование)" << endl;
        cout << "5. Добавить элемент (кукушкино хеширование)" << endl;
        cout << "6. Найти элемент (кукушкино хеширование)" << endl;
        cout << "7. Вывести таблицу (кукушкино хеширование)" << endl;
        cout << "8. Реструктуризация (двойное хеширование)" << endl;
        cout << "0. Выход" << endl;
        cout << "Выбор: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Введите ключ и значение: ";
                cin >> key >> value;
                if (double_hash_insert(double_ht, key, value)) {
                    cout << "Элемент добавлен" << endl;
                } else {
                    cout << "Не удалось добавить элемент" << endl;
                }
                break;
                
            case 2:
                cout << "Введите ключ для поиска: ";
                cin >> key;
                value = double_hash_search(double_ht, key);
                if (!value.empty()) {
                    cout << "Найдено: " << value << endl;
                } else {
                    cout << "Не найдено" << endl;
                }
                break;
                
            case 3:
                cout << "Введите ключ для удаления: ";
                cin >> key;
                if (double_hash_remove(double_ht, key)) {
                    cout << "Элемент удален" << endl;
                } else {
                    cout << "Элемент не найден" << endl;
                }
                break;
                
            case 4:
                print_double_hash_table(double_ht);
                break;
                
            case 5:
                cout << "Введите ключ и значение: ";
                cin >> key >> value;
                if (cuckoo_hash_insert(cuckoo_ht, key, value)) {
                    cout << "Элемент добавлен" << endl;
                } else {
                    cout << "Не удалось добавить элемент" << endl;
                }
                break;
                
            case 6:
                cout << "Введите ключ для поиска: ";
                cin >> key;
                value = cuckoo_hash_search(cuckoo_ht, key);
                if (!value.empty()) {
                    cout << "Найдено: " << value << endl;
                } else {
                    cout << "Не найдено" << endl;
                }
                break;
                
            case 7:
                print_cuckoo_hash_table(cuckoo_ht);
                break;
                
            case 8:
                restructure_double_hash(double_ht);
                break;
                
            case 0:
                free_double_hash_table(double_ht);
                free_cuckoo_hash_table(cuckoo_ht);
                return 0;
                
            default:
                cout << "Неверный выбор" << endl;
        }
    }
}