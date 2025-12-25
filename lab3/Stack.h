#ifndef STACK_H
#define STACK_H

#include <fstream>
#include <string>

using namespace std;

class Stack {
private:
    string* data;
    int capacity;
    int top;

    void resize(int new_capacity);
    void clear();

public:
    Stack(int initial_capacity = 10);
    ~Stack();
    Stack(const Stack& other);
    Stack& operator=(const Stack& other);

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