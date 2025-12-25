#include "Array.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

Array::Array(int initial_capacity) {
    data = new string[initial_capacity];
    capacity = initial_capacity;
    size = 0;
}

Array::~Array() {
    delete[] data;
}

Array::Array(const Array& other) {
    capacity = other.capacity;
    size = other.size;
    data = new string[capacity];
    for (int i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

Array& Array::operator=(const Array& other) {
    if (this != &other) {
        delete[] data;
        capacity = other.capacity;
        size = other.size;
        data = new string[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

void Array::resize(int new_capacity) {
    string* new_data = new string[new_capacity];
    for (int i = 0; i < size; ++i) {
        new_data[i] = data[i];
    }
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

void Array::push_back(const string& value) {
    if (size >= capacity) {
        resize(capacity * 2);
    }
    data[size] = value;
    size++;
}

bool Array::insert(int index, const string& value) {
    if (index < 0 || index > size) {
        return false;
    }

    if (size >= capacity) {
        resize(capacity * 2);
    }

    for (int i = size; i > index; --i) {
        data[i] = data[i - 1];
    }
    data[index] = value;
    size++;
    return true;
}

string Array::get(int index) const {
    if (index < 0 || index >= size) {
        return "";
    }
    return data[index];
}

bool Array::remove(int index) {
    if (index < 0 || index >= size) {
        return false;
    }

    for (int i = index; i < size - 1; ++i) {
        data[i] = data[i + 1];
    }
    size--;
    return true;
}

bool Array::replace(int index, const string& value) {
    if (index < 0 || index >= size) {
        return false;
    }
    data[index] = value;
    return true;
}

int Array::length() const {
    return size;
}

void Array::print() const {
    if (size == 0) {
        cout << "массив пуст" << endl;
        return;
    }

    cout << "массив [" << size << "]: ";
    for (int i = 0; i < size; ++i) {
        cout << data[i];
        if (i < size - 1) {
            cout << ", ";
        }
    }
    cout << endl;
}

bool Array::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (int i = 0; i < size; ++i) {
        size_t str_size = data[i].size();
        file.write(reinterpret_cast<const char*>(&str_size), sizeof(str_size));
        file.write(data[i].c_str(), str_size);
    }

    return true;
}

bool Array::deserialize_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int new_size;
    file.read(reinterpret_cast<char*>(&new_size), sizeof(new_size));

    if (new_size > capacity) {
        resize(new_size);
    }

    size = 0;
    for (int i = 0; i < new_size; ++i) {
        size_t str_size;
        file.read(reinterpret_cast<char*>(&str_size), sizeof(str_size));

        string value(str_size, '\0');
        file.read(&value[0], str_size);
        push_back(value);
    }

    return true;
}

bool Array::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << size << endl;
    for (int i = 0; i < size; ++i) {
        file << data[i] << endl;
    }

    return true;
}

bool Array::deserialize_text(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int new_size;
    file >> new_size;
    file.ignore();

    size = 0;
    for (int i = 0; i < new_size; ++i) {
        string value;
        getline(file, value);
        if (!file.good() && !file.eof()) {
            return false;
        }
        push_back(value);
    }

    return true;
}

vector<string> Array::to_vector() const {
    vector<string> result;
    for (int i = 0; i < size; ++i) {
        result.push_back(data[i]);
    }
    return result;
}