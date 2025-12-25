#include "DoubleList.h"

#include <fstream>
#include <iostream>

using namespace std;

DoubleList::DoubleList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

DoubleList::~DoubleList() {
    clear();
}

DoubleList::DoubleList(const DoubleList& other) {
    head = nullptr;
    tail = nullptr;
    size = 0;

    DNode* current = other.head;
    while (current != nullptr) {
        push_back(current->data);
        current = current->next;
    }
}

DoubleList& DoubleList::operator=(const DoubleList& other) {
    if (this != &other) {
        clear();
        DNode* current = other.head;
        while (current != nullptr) {
            push_back(current->data);
            current = current->next;
        }
    }
    return *this;
}

void DoubleList::clear() {
    DNode* current = head;
    while (current != nullptr) {
        DNode* temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void DoubleList::push_front(const string& value) {
    DNode* new_node = new DNode(value);
    new_node->next = head;

    if (head != nullptr) {
        head->prev = new_node;
    }

    head = new_node;

    if (tail == nullptr) {
        tail = new_node;
    }
    size++;
}

void DoubleList::push_back(const string& value) {
    DNode* new_node = new DNode(value);
    new_node->prev = tail;

    if (tail != nullptr) {
        tail->next = new_node;
    }

    tail = new_node;

    if (head == nullptr) {
        head = new_node;
    }
    size++;
}

bool DoubleList::insert_before(const string& target, const string& value) {
    DNode* current = head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }

    if (current == nullptr)
        return false;

    if (current == head) {
        push_front(value);
        return true;
    }

    DNode* new_node = new DNode(value);
    new_node->prev = current->prev;
    new_node->next = current;

    current->prev->next = new_node;
    current->prev = new_node;

    size++;
    return true;
}

bool DoubleList::insert_after(const string& target, const string& value) {
    DNode* current = head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }

    if (current == nullptr)
        return false;

    if (current == tail) {
        push_back(value);
        return true;
    }

    DNode* new_node = new DNode(value);
    new_node->prev = current;
    new_node->next = current->next;

    current->next->prev = new_node;
    current->next = new_node;

    size++;
    return true;
}

bool DoubleList::pop_front() {
    if (head == nullptr)
        return false;

    DNode* temp = head;
    head = head->next;

    if (head != nullptr) {
        head->prev = nullptr;
    } else {
        tail = nullptr;
    }

    delete temp;
    size--;
    return true;
}

bool DoubleList::pop_back() {
    if (tail == nullptr)
        return false;

    DNode* temp = tail;
    tail = tail->prev;

    if (tail != nullptr) {
        tail->next = nullptr;
    } else {
        head = nullptr;
    }

    delete temp;
    size--;
    return true;
}

bool DoubleList::remove_before(const string& target) {
    DNode* current = head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }

    if (current == nullptr || current->prev == nullptr)
        return false;

    DNode* to_remove = current->prev;

    if (to_remove == head) {
        head = current;
        current->prev = nullptr;
    } else {
        to_remove->prev->next = current;
        current->prev = to_remove->prev;
    }

    delete to_remove;
    size--;
    return true;
}

bool DoubleList::remove_after(const string& target) {
    DNode* current = head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }

    if (current == nullptr || current->next == nullptr)
        return false;

    DNode* to_remove = current->next;

    if (to_remove == tail) {
        tail = current;
        current->next = nullptr;
    } else {
        current->next = to_remove->next;
        to_remove->next->prev = current;
    }

    delete to_remove;
    size--;
    return true;
}

bool DoubleList::remove_value(const string& value) {
    DNode* current = head;
    while (current != nullptr && current->data != value) {
        current = current->next;
    }

    if (current == nullptr)
        return false;

    if (current == head) {
        return pop_front();
    }

    if (current == tail) {
        return pop_back();
    }

    current->prev->next = current->next;
    current->next->prev = current->prev;

    delete current;
    size--;
    return true;
}

DNode* DoubleList::find(const string& value) {
    DNode* current = head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void DoubleList::print_forward() const {
    if (head == nullptr) {
        cout << "Двусвязный список пуст" << endl;
        return;
    }

    cout << "Двусвязный список [" << size << "]: ";
    DNode* current = head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " <-> ";
        }
        current = current->next;
    }
    cout << endl;
}

void DoubleList::print_backward() const {
    if (tail == nullptr) {
        cout << "Двусвязный список пуст" << endl;
        return;
    }

    cout << "Двусвязный список в обратном порядке [" << size << "]: ";
    DNode* current = tail;
    while (current != nullptr) {
        cout << current->data;
        if (current->prev != nullptr) {
            cout << " <-> ";
        }
        current = current->prev;
    }
    cout << endl;
}

int DoubleList::get_size() const {
    return size;
}

bool DoubleList::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    DNode* current = head;
    while (current != nullptr) {
        size_t str_size = current->data.size();
        file.write(reinterpret_cast<const char*>(&str_size), sizeof(str_size));
        file.write(current->data.c_str(), str_size);
        current = current->next;
    }

    return true;
}

bool DoubleList::deserialize_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int new_size;
    file.read(reinterpret_cast<char*>(&new_size), sizeof(new_size));

    clear();

    for (int i = 0; i < new_size; ++i) {
        size_t str_size;
        file.read(reinterpret_cast<char*>(&str_size), sizeof(str_size));

        string value(str_size, '\0');
        file.read(&value[0], str_size);
        push_back(value);
    }

    return true;
}

bool DoubleList::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << size << endl;

    DNode* current = head;
    while (current != nullptr) {
        file << current->data << endl;
        current = current->next;
    }

    return true;
}

bool DoubleList::deserialize_text(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int new_size;
    file >> new_size;
    file.ignore();

    clear();

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