#ifndef SINGLELIST_H
#define SINGLELIST_H

#include <fstream>
#include <string>

using namespace std;

class SNode {
public:
    string data;
    SNode* next;

    SNode(const string& value = "") : data(value), next(nullptr) {}
};

class SingleList {
private:
    SNode* head;
    SNode* tail;
    int size;

    void print_backward_helper(SNode* node) const;
    void clear();

public:
    SingleList();
    ~SingleList();
    SingleList(const SingleList& other);
    SingleList& operator=(const SingleList& other);

    void push_front(const string& value);
    void push_back(const string& value);
    bool insert_before(const string& target, const string& value);
    bool insert_after(const string& target, const string& value);
    bool pop_front();
    bool pop_back();
    bool remove_before(const string& target);
    bool remove_after(const string& target);
    bool remove_value(const string& value);
    SNode* find(const string& value);
    SNode* find_first() const { return head; }
    SNode* find_next(SNode* current) const { return current ? current->next : nullptr; }
    void print_forward() const;
    void print_backward() const;
    int get_size() const;

    bool serialize_binary(const string& filename) const;
    bool deserialize_binary(const string& filename);
    bool serialize_text(const string& filename) const;
    bool deserialize_text(const string& filename);
};

#endif