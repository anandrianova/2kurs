#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <fstream>
#include <functional>
#include <string>
#include <iostream>

using namespace std;

struct HashEntry {
    string key;
    string value;
    bool is_deleted;
    bool is_occupied;

    HashEntry();
};

class DoubleHashTable {
private:
    HashEntry* table;
    int capacity;
    int size;
    double load_factor_threshold;

    int hash1(const string& key) const;
    int hash2(const string& key) const;

    void clear();
    void copy_from(const DoubleHashTable& other);

public:
    DoubleHashTable(int initial_capacity = 10);
    ~DoubleHashTable();
    DoubleHashTable(const DoubleHashTable& other);
    DoubleHashTable& operator=(const DoubleHashTable& other);

    bool insert(const string& key, const string& value);
    string search(const string& key) const;
    bool remove(const string& key);
    void print() const;
    void restructure();

    bool serialize_binary(const string& filename) const;
    bool deserialize_binary(const string& filename);
    bool serialize_text(const string& filename) const;
    bool deserialize_text(const string& filename);

    int get_capacity() const { return capacity; }
    int get_size() const { return size; }
    double get_load_factor() const { return static_cast<double>(size) / capacity; }
};

#endif