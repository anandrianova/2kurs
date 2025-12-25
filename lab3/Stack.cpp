#include "Stack.h"

#include <fstream>
#include <iostream>

using namespace std;

Stack::Stack(int initial_capacity) {
    data = new string[initial_capacity];
    capacity = initial_capacity;
    top = -1;
}

Stack::~Stack() {
    clear();
}

Stack::Stack(const Stack& other) {
    capacity = other.capacity;
    top = other.top;
    data = new string[capacity];
    for (int i = 0; i <= top; ++i) {
        data[i] = other.data[i];
    }
}

Stack& Stack::operator=(const Stack& other) {
    if (this != &other) {
        clear();
        capacity = other.capacity;
        top = other.top;
        data = new string[capacity];
        for (int i = 0; i <= top; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

void Stack::resize(int new_capacity) {
    string* new_data = new string[new_capacity];
    for (int i = 0; i <= top; ++i) {
        new_data[i] = data[i];
    }
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

void Stack::clear() {
    delete[] data;
    data = nullptr;
    capacity = 0;
    top = -1;
}

bool Stack::push(const string& value) {
    if (top >= capacity - 1) {
        resize(capacity * 2);
    }

    top++;
    data[top] = value;
    return true;
}

string Stack::pop() {
    if (top < 0) {
        return "";
    }

    string value = data[top];
    top--;
    return value;
}

string Stack::peek() const {
    if (top < 0) {
        return "";
    }
    return data[top];
}

bool Stack::is_empty() const {
    return top < 0;
}

int Stack::get_size() const {
    return top + 1;
}

void Stack::print() const {
    if (top < 0) {
        cout << "стек пустой" << endl;
        return;
    }

    cout << "Стопка (сверху вниз) [" << (top + 1) << "]: ";
    for (int i = top; i >= 0; i--) {
        cout << data[i];
        if (i > 0) {
            cout << " | ";
        }
    }
    cout << endl;
}

bool Stack::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int stack_size = get_size();
    file.write(reinterpret_cast<const char*>(&stack_size), sizeof(stack_size));

    for (int i = 0; i <= top; ++i) {
        size_t str_size = data[i].size();
        file.write(reinterpret_cast<const char*>(&str_size), sizeof(str_size));
        file.write(data[i].c_str(), str_size);
    }

    return true;
}

bool Stack::deserialize_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int stack_size;
    file.read(reinterpret_cast<char*>(&stack_size), sizeof(stack_size));

    clear();
    data = new string[stack_size];
    capacity = stack_size;
    top = -1;

    for (int i = 0; i < stack_size; ++i) {
        size_t str_size;
        file.read(reinterpret_cast<char*>(&str_size), sizeof(str_size));

        string value(str_size, '\0');
        file.read(&value[0], str_size);
        push(value);
    }

    return true;
}

bool Stack::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int stack_size = get_size();
    file << stack_size << endl;

    for (int i = top; i >= 0; --i) {
        file << data[i] << endl;
    }

    return true;
}

bool Stack::deserialize_text(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int stack_size;
    file >> stack_size;
    file.ignore();

    clear();
    data = new string[stack_size];
    capacity = stack_size;
    top = -1;

    string* temp = new string[stack_size];
    for (int i = 0; i < stack_size; ++i) {
        string value;
        getline(file, value);
        if (!file.good() && !file.eof()) {
            delete[] temp;
            return false;
        }
        temp[i] = value;
    }

    for (int i = stack_size - 1; i >= 0; --i) {
        push(temp[i]);
    }

    delete[] temp;
    return true;
}