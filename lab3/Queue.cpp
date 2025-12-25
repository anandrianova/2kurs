#include "Queue.h"

#include <fstream>
#include <iostream>

using namespace std;

Queue::Queue(int initial_capacity) {
    data = new string[initial_capacity];
    capacity = initial_capacity;
    front = 0;
    rear = -1;
    size = 0;
}

Queue::~Queue() {
    clear();
}

Queue::Queue(const Queue& other) {
    capacity = other.capacity;
    size = other.size;
    front = other.front;
    rear = other.rear;
    data = new string[capacity];

    for (int i = 0; i < size; ++i) {
        int index = (front + i) % capacity;
        data[index] = other.data[index];
    }
}

Queue& Queue::operator=(const Queue& other) {
    if (this != &other) {
        clear();
        capacity = other.capacity;
        size = other.size;
        front = other.front;
        rear = other.rear;
        data = new string[capacity];

        for (int i = 0; i < size; ++i) {
            int index = (front + i) % capacity;
            data[index] = other.data[index];
        }
    }
    return *this;
}

void Queue::resize(int new_capacity) {
    string* new_data = new string[new_capacity];

    for (int i = 0; i < size; ++i) {
        new_data[i] = data[(front + i) % capacity];
    }

    delete[] data;
    data = new_data;
    capacity = new_capacity;
    front = 0;
    rear = size - 1;
}

void Queue::clear() {
    delete[] data;
    data = nullptr;
    capacity = 0;
    front = 0;
    rear = -1;
    size = 0;
}

bool Queue::push(const string& value) {
    if (size >= capacity) {
        resize(capacity * 2);
    }

    rear = (rear + 1) % capacity;
    data[rear] = value;
    size++;
    return true;
}

string Queue::pop() {
    if (size == 0) {
        return "";
    }

    string value = data[front];
    front = (front + 1) % capacity;
    size--;
    return value;
}

string Queue::peek() const {
    if (size == 0) {
        return "";
    }
    return data[front];
}

bool Queue::is_empty() const {
    return size == 0;
}

int Queue::get_size() const {
    return size;
}

void Queue::print() const {
    if (size == 0) {
        cout << "очередь пуста" << endl;
        return;
    }

    cout << "Очередь (начало -> конец) [" << size << "]: ";
    for (int i = 0; i < size; i++) {
        int index = (front + i) % capacity;
        cout << data[index];
        if (i < size - 1) {
            cout << " -> ";
        }
    }
    cout << endl;
}

bool Queue::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (int i = 0; i < size; ++i) {
        int index = (front + i) % capacity;
        size_t str_size = data[index].size();
        file.write(reinterpret_cast<const char*>(&str_size), sizeof(str_size));
        file.write(data[index].c_str(), str_size);
    }

    return true;
}

bool Queue::deserialize_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int new_size;
    file.read(reinterpret_cast<char*>(&new_size), sizeof(new_size));

    clear();
    data = new string[new_size];
    capacity = new_size;
    front = 0;
    rear = -1;
    size = 0;

    for (int i = 0; i < new_size; ++i) {
        size_t str_size;
        file.read(reinterpret_cast<char*>(&str_size), sizeof(str_size));

        string value(str_size, '\0');
        file.read(&value[0], str_size);
        push(value);
    }

    return true;
}

bool Queue::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << size << endl;

    for (int i = 0; i < size; ++i) {
        int index = (front + i) % capacity;
        file << data[index] << endl;
    }

    return true;
}

bool Queue::deserialize_text(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int new_size;
    file >> new_size;
    file.ignore();

    clear();
    data = new string[new_size];
    capacity = new_size;
    front = 0;
    rear = -1;
    size = 0;

    for (int i = 0; i < new_size; ++i) {
        string value;
        getline(file, value);
        if (!file.good() && !file.eof()) {
            return false;
        }
        push(value);
    }

    return true;
}