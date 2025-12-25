#ifndef QUEUE_H
#define QUEUE_H

#include <fstream>
#include <string>

using namespace std;

class Queue {
private:
    string* data;
    int capacity;
    int front;
    int rear;
    int size;

    void resize(int new_capacity);
    void clear();

public:
    Queue(int initial_capacity = 10);
    ~Queue();
    Queue(const Queue& other);
    Queue& operator=(const Queue& other);

    bool push(const string& value);
    string pop();
    string peek() const;
    bool is_empty() const;
    int get_size() const;
    void print() const;

    bool serialize_binary(const string& filename) const;
    bool deserialize_binary(const string& filename);
    bool serialize_text(const string& filename) const;
    bool deserialize_text(const string& filename);
};

#endif