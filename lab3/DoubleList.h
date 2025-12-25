#ifndef DOUBLELIST_H
#define DOUBLELIST_H

#include <fstream>
#include <string>

using namespace std;

class DNode {
public:
    string data;
    DNode* prev;
    DNode* next;

    DNode(const string& value = "") : data(value), prev(nullptr), next(nullptr) {}
};

class DoubleList {
private:
    DNode* head;
    DNode* tail;
    int size;

    void clear();

public:
    DoubleList();
    ~DoubleList();
    DoubleList(const DoubleList& other);
    DoubleList& operator=(const DoubleList& other);

    void push_front(const string& value);
    void push_back(const string& value);
    bool insert_before(const string& target, const string& value);
    bool insert_after(const string& target, const string& value);
    bool pop_front();
    bool pop_back();
    bool remove_before(const string& target);
    bool remove_after(const string& target);
    bool remove_value(const string& value);
    DNode* find(const string& value);
    DNode* find_first() const { return head; }
    DNode* find_next(DNode* current) const { return current ? current->next : nullptr; }
    void print_forward() const;
    void print_backward() const;
    int get_size() const;

    bool serialize_binary(const string& filename) const;
    bool deserialize_binary(const string& filename);
    bool serialize_text(const string& filename) const;
    bool deserialize_text(const string& filename);
};

#endif