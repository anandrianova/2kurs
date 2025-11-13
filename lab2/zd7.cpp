#include <iostream>
#include <string>
#include <chrono>

using namespace std;

// Структура для узла LFU кэша
struct LFUNode {
    string key;        
    string value;      
    int frequency;     
    long long timestamp; // Время добавления/последнего доступа
    LFUNode* next;     
    LFUNode* prev;     
};

// Структура для двусвязного списка узлов с одинаковой частотой
struct FrequencyList {
    LFUNode* head;     
    LFUNode* tail;     
    int size;          
};

// Структура LFU кэша
struct LFUCache {
    int capacity;      
    int size;          
    int min_frequency; 
    LFUNode** key_map; 
    FrequencyList* frequency_map; 
    int key_map_size;  
    int freq_map_size; 
};

// Создание пустого списка частот
FrequencyList* create_frequency_list() {
    FrequencyList* list = new FrequencyList;
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
    return list;
}

// Добавление узла в начало списка частот
void list_add_front(FrequencyList* list, LFUNode* node) {
    node->next = list->head;
    node->prev = nullptr;
    
    if (list->head != nullptr) {
        list->head->prev = node;
    }
    
    list->head = node;
    
    if (list->tail == nullptr) {
        list->tail = node;
    }
    
    list->size++;
}

// Удаление узла из списка частот
void list_remove(FrequencyList* list, LFUNode* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    
    list->size--;
}

// Получение текущего времени в миллисекундах
long long get_current_time() {
    return chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()
    ).count();
}

// Создание LFU кэша
LFUCache* create_lfu_cache(int capacity) {
    LFUCache* cache = new LFUCache;
    cache->capacity = capacity;
    cache->size = 0;
    cache->min_frequency = 0;
    
    cache->key_map_size = capacity * 2;
    cache->key_map = new LFUNode*[cache->key_map_size];
    for (int i = 0; i < cache->key_map_size; i++) {
        cache->key_map[i] = nullptr;
    }
    
    cache->freq_map_size = capacity + 1;
    cache->frequency_map = new FrequencyList[cache->freq_map_size];
    for (int i = 0; i < cache->freq_map_size; i++) {
        cache->frequency_map[i].head = nullptr;
        cache->frequency_map[i].tail = nullptr;
        cache->frequency_map[i].size = 0;
    }
    
    return cache;
}

// Простая хеш-функция для ключа
int hash_key(const string& key, int size) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % size;
    }
    return hash;
}

// Найти самый старый узел с минимальной частотой
LFUNode* find_oldest_min_frequency_node(LFUCache* cache) {
    // Ищем непустой список с минимальной частотой
    int freq = cache->min_frequency;
    while (freq < cache->freq_map_size && cache->frequency_map[freq].size == 0) {
        freq++;
    }
    
    if (freq >= cache->freq_map_size) {
        return nullptr; // Кэш пуст
    }
    
    // В списке с минимальной частотой ищем самый старый узел (tail)
    return cache->frequency_map[freq].tail;
}

// Удаление наименее часто используемого элемента (самого старого)
void evict_lfu(LFUCache* cache) {
    LFUNode* node_to_remove = find_oldest_min_frequency_node(cache);
    
    if (node_to_remove != nullptr) {
        // Удаляем из key_map
        int hash_index = hash_key(node_to_remove->key, cache->key_map_size);
        LFUNode* current = cache->key_map[hash_index];
        LFUNode* prev = nullptr;
        
        while (current != nullptr) {
            if (current == node_to_remove) {
                if (prev == nullptr) {
                    cache->key_map[hash_index] = current->next;
                } else {
                    prev->next = current->next;
                }
                break;
            }
            prev = current;
            current = current->next;
        }
        
        // Удаляем из списка частот
        FrequencyList* list = &cache->frequency_map[node_to_remove->frequency];
        list_remove(list, node_to_remove);
        
        // Обновляем min_frequency если нужно
        if (list->size == 0 && node_to_remove->frequency == cache->min_frequency) {
            cache->min_frequency++;
        }
        
        // Освобождаем память и уменьшаем размер
        delete node_to_remove;
        cache->size--;
    }
}

// Получение значения из кэша
string cache_get(LFUCache* cache, const string& key) {
    if (cache->capacity == 0) return "-1";
    
    // Ищем узел в key_map
    int hash_index = hash_key(key, cache->key_map_size);
    LFUNode* node = cache->key_map[hash_index];
    
    while (node != nullptr) {
        if (node->key == key) {
            break;
        }
        node = node->next;
    }
    
    if (node == nullptr) {
        return "-1";
    }
    
    // Обновляем частоту использования и время
    int old_freq = node->frequency;
    node->frequency++;
    node->timestamp = get_current_time();
    
    // Удаляем из старого списка частот
    if (old_freq < cache->freq_map_size) {
        list_remove(&cache->frequency_map[old_freq], node);
        
        if (old_freq == cache->min_frequency && 
            cache->frequency_map[old_freq].size == 0) {
            cache->min_frequency++;
        }
    }
    
    // Добавляем в новый список частот
    int new_freq = node->frequency;
    if (new_freq < cache->freq_map_size) {
        list_add_front(&cache->frequency_map[new_freq], node);
    }
    
    return node->value;
}

// Установка значения в кэш
void cache_set(LFUCache* cache, const string& key, const string& value) {
    if (cache->capacity == 0) return;
    
    // Сначала проверяем, есть ли уже такой ключ
    int hash_index = hash_key(key, cache->key_map_size);
    LFUNode* node = cache->key_map[hash_index];
    LFUNode* prev = nullptr;
    
    // Ищем существующий узел
    while (node != nullptr) {
        if (node->key == key) {
            // Обновляем значение и frequency
            node->value = value;
            int old_freq = node->frequency;
            node->frequency++;
            node->timestamp = get_current_time();
            
            // Удаляем из старого списка частот
            if (old_freq < cache->freq_map_size) {
                list_remove(&cache->frequency_map[old_freq], node);
                
                if (old_freq == cache->min_frequency && 
                    cache->frequency_map[old_freq].size == 0) {
                    cache->min_frequency++;
                }
            }
            
            // Добавляем в новый список частот
            int new_freq = node->frequency;
            if (new_freq < cache->freq_map_size) {
                list_add_front(&cache->frequency_map[new_freq], node);
            }
            
            return;
        }
        prev = node;
        node = node->next;
    }
    
    // Если кэш полон, удаляем наименее используемый элемент
    if (cache->size >= cache->capacity) {
        evict_lfu(cache);
    }
    
    // Создаем новый узел
    LFUNode* new_node = new LFUNode;
    new_node->key = key;
    new_node->value = value;
    new_node->frequency = 1;
    new_node->timestamp = get_current_time();
    new_node->next = nullptr;
    new_node->prev = nullptr;
    
    // Добавляем в key_map
    new_node->next = cache->key_map[hash_index];
    cache->key_map[hash_index] = new_node;
    
    // Добавляем в список частот 1
    list_add_front(&cache->frequency_map[1], new_node);
    
    // Обновляем минимальную частоту и размер
    cache->min_frequency = 1;
    cache->size++;
}

// Вывод состояния кэша
void print_cache(const LFUCache* cache) {
    cout << "LFU Cache (capacity: " << cache->capacity << ", size: " << cache->size << "):" << endl;
    
    for (int freq = 1; freq < cache->freq_map_size; freq++) {
        if (cache->frequency_map[freq].size > 0) {
            cout << "Frequency " << freq << ": ";
            LFUNode* current = cache->frequency_map[freq].head;
            while (current != nullptr) {
                cout << current->key << ":" << current->value << " ";
                current = current->next;
            }
            cout << endl;
        }
    }
    
    cout << "Min frequency: " << cache->min_frequency << endl;
}

// Освобождение памяти кэша
void free_cache(LFUCache* cache) {
    for (int i = 0; i < cache->key_map_size; i++) {
        LFUNode* current = cache->key_map[i];
        while (current != nullptr) {
            LFUNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
    
    delete[] cache->key_map;
    delete[] cache->frequency_map;
    delete cache;
}

int main() {
    int capacity, choice;
    string key, value;
    
    cout << "=== LFU Cache Implementation ===" << endl;
    cout << "Введите вместимость кэша: ";
    cin >> capacity;
    
    LFUCache* cache = create_lfu_cache(capacity);
    
    while (true) {
        cout << "\nМеню:" << endl;
        cout << "1. SET (добавить/обновить)" << endl;
        cout << "2. GET (получить)" << endl;
        cout << "3. PRINT (вывести кэш)" << endl;
        cout << "0. EXIT (выход)" << endl;
        cout << "Выбор: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Введите ключ и значение: ";
                cin >> key >> value;
                cache_set(cache, key, value);
                cout << "Элемент установлен" << endl;
                break;
                
            case 2:
                cout << "Введите ключ: ";
                cin >> key;
                value = cache_get(cache, key);
                if (value != "-1") {
                    cout << "Значение: " << value << endl;
                } else {
                    cout << "Ключ не найден" << endl;
                }
                break;
                
            case 3:
                print_cache(cache);
                break;
                
            case 0:
                free_cache(cache);
                return 0;
                
            default:
                cout << "Неверный выбор" << endl;
        }
    }
}