#ifndef ARRAY_H
#define ARRAY_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Array {
private:
    string* data;
    int capacity;
    int size;

    void resize(int new_capacity);

public:
    Array(int initial_capacity = 10);
    ~Array();
    Array(const Array& other);
    Array& operator=(const Array& other);

    void push_back(const string& value);
    bool insert(int index, const string& value);
    string get(int index) const;
    bool remove(int index);
    bool replace(int index, const string& value);
    int length() const;
    void print() const;

    bool serialize_binary(const string& filename) const;
    bool deserialize_binary(const string& filename);
    bool serialize_text(const string& filename) const;
    bool deserialize_text(const string& filename);

    vector<string> to_vector() const;
};

#endif