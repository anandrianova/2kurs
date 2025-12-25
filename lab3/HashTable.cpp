#include "HashTable.h"

#include <fstream>
#include <iostream>

using namespace std;

HashEntry::HashEntry() {
    key = "";
    value = "";
    is_deleted = false;
    is_occupied = false;
}

DoubleHashTable::DoubleHashTable(int initial_capacity) {
    capacity = initial_capacity;
    size = 0;
    load_factor_threshold = 0.9;

    table = new HashEntry[capacity];
}

DoubleHashTable::~DoubleHashTable() {
    clear();
}

DoubleHashTable::DoubleHashTable(const DoubleHashTable& other) {
    copy_from(other);
}

DoubleHashTable& DoubleHashTable::operator=(const DoubleHashTable& other) {
    if (this != &other) {
        clear();
        copy_from(other);
    }
    return *this;
}

void DoubleHashTable::clear() {
    delete[] table;
    table = nullptr;
    capacity = 0;
    size = 0;
}

void DoubleHashTable::copy_from(const DoubleHashTable& other) {
    capacity = other.capacity;
    size = other.size;
    load_factor_threshold = other.load_factor_threshold;

    table = new HashEntry[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = other.table[i];
    }
}

int DoubleHashTable::hash1(const string& key) const {
    hash<string> hasher;
    return hasher(key) % capacity;
}

int DoubleHashTable::hash2(const string& key) const {
    hash<string> hasher;
    return 1 + (hasher(key) % (capacity - 1));
}

bool DoubleHashTable::insert(const string& key, const string& value) {
    double load_factor = get_load_factor();
    if (load_factor >= load_factor_threshold) {
        cout << "Коэффициент заполнения " << load_factor << " превышает порог "
             << load_factor_threshold << ", выполняется реструктуризация..." << endl;
        restructure();
    }

    int index = hash1(key);
    int step = hash2(key);
    int attempts = 0;

    while (attempts < capacity) {
        if (!table[index].is_occupied || table[index].is_deleted) {
            table[index].key = key;
            table[index].value = value;
            table[index].is_occupied = true;
            table[index].is_deleted = false;
            size++;
            return true;
        }

        if (table[index].key == key) {
            table[index].value = value;
            return true;
        }

        index = (index + step) % capacity;
        attempts++;
    }

    return false;
}

string DoubleHashTable::search(const string& key) const {
    int index = hash1(key);
    int step = hash2(key);
    int attempts = 0;

    while (attempts < capacity) {
        if (table[index].is_occupied && !table[index].is_deleted && table[index].key == key) {
            return table[index].value;
        }

        if (!table[index].is_occupied && !table[index].is_deleted) {
            return "";
        }

        index = (index + step) % capacity;
        attempts++;
    }

    return "";
}

bool DoubleHashTable::remove(const string& key) {
    int index = hash1(key);
    int step = hash2(key);
    int attempts = 0;

    while (attempts < capacity) {
        if (table[index].is_occupied && !table[index].is_deleted && table[index].key == key) {
            table[index].is_deleted = true;
            size--;
            return true;
        }

        if (!table[index].is_occupied && !table[index].is_deleted) {
            return false;
        }

        index = (index + step) % capacity;
        attempts++;
    }

    return false;
}

void DoubleHashTable::print() const {
    cout << "Таблица с двойным хешированием (емкость: " << capacity << ", размер: " << size
         << "):" << endl;

    for (int i = 0; i < capacity; i++) {
        cout << "[" << i << "]: ";
        if (table[i].is_occupied && !table[i].is_deleted) {
            cout << table[i].key << " -> " << table[i].value;
        } else if (table[i].is_deleted) {
            cout << "УДАЛЕНО";
        } else {
            cout << "пусто";
        }
        cout << endl;
    }
}

void DoubleHashTable::restructure() {
    cout << "=== Реструктуризация таблицы с двойным хешированием ===" << endl;

    HashEntry* old_table = table;
    int old_capacity = capacity;

    capacity *= 2;
    size = 0;
    table = new HashEntry[capacity];

    for (int i = 0; i < old_capacity; i++) {
        if (old_table[i].is_occupied && !old_table[i].is_deleted) {
            insert(old_table[i].key, old_table[i].value);
        }
    }

    cout << "Реструктуризация завершена. Новая вместимость: " << capacity << endl;

    delete[] old_table;
}

bool DoubleHashTable::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (int i = 0; i < capacity; ++i) {
        file.write(reinterpret_cast<const char*>(&table[i].is_occupied), sizeof(bool));
        file.write(reinterpret_cast<const char*>(&table[i].is_deleted), sizeof(bool));

        if (table[i].is_occupied && !table[i].is_deleted) {
            size_t key_size = table[i].key.size();
            file.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
            file.write(table[i].key.c_str(), key_size);

            size_t value_size = table[i].value.size();
            file.write(reinterpret_cast<const char*>(&value_size), sizeof(value_size));
            file.write(table[i].value.c_str(), value_size);
        }
    }

    return true;
}

bool DoubleHashTable::deserialize_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    clear();

    file.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    table = new HashEntry[capacity];

    for (int i = 0; i < capacity; ++i) {
        file.read(reinterpret_cast<char*>(&table[i].is_occupied), sizeof(bool));
        file.read(reinterpret_cast<char*>(&table[i].is_deleted), sizeof(bool));

        if (table[i].is_occupied && !table[i].is_deleted) {
            size_t key_size;
            file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));

            table[i].key.resize(key_size);
            file.read(&table[i].key[0], key_size);

            size_t value_size;
            file.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));

            table[i].value.resize(value_size);
            file.read(&table[i].value[0], value_size);
        }
    }

    return true;
}

bool DoubleHashTable::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << capacity << endl;
    file << size << endl;

    for (int i = 0; i < capacity; ++i) {
        file << table[i].is_occupied << " " << table[i].is_deleted << endl;

        if (table[i].is_occupied && !table[i].is_deleted) {
            file << table[i].key << endl;
            file << table[i].value << endl;
        }
    }

    return true;
}

bool DoubleHashTable::deserialize_text(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    clear();

    file >> capacity;
    file >> size;
    file.ignore();

    table = new HashEntry[capacity];

    for (int i = 0; i < capacity; ++i) {
        int occupied, deleted;
        file >> occupied >> deleted;
        file.ignore();

        table[i].is_occupied = occupied;
        table[i].is_deleted = deleted;

        if (table[i].is_occupied && !table[i].is_deleted) {
            getline(file, table[i].key);
            getline(file, table[i].value);
        }
    }

    return true;
}