#include "SingleList.h"

#include <fstream>
#include <iostream>

using namespace std;

SingleList::SingleList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

SingleList::~SingleList() {
    clear();
}

SingleList::SingleList(const SingleList& other) {
    head = nullptr;
    tail = nullptr;
    size = 0;

    SNode* current = other.head;
    while (current != nullptr) {
        push_back(current->data);
        current = current->next;
    }
}

SingleList& SingleList::operator=(const SingleList& other) {
    if (this != &other) {
        clear();
        SNode* current = other.head;
        while (current != nullptr) {
            push_back(current->data);
            current = current->next;
        }
    }
    return *this;
}

void SingleList::clear() {
    SNode* current = head;
    while (current != nullptr) {
        SNode* temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void SingleList::push_front(const string& value) {
    SNode* new_node = new SNode(value);
    new_node->next = head;
    head = new_node;

    if (tail == nullptr) {
        tail = new_node;
    }
    size++;
}

void SingleList::push_back(const string& value) {
    SNode* new_node = new SNode(value);

    if (tail == nullptr) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    size++;
}

bool SingleList::insert_before(const string& target, const string& value) {
    if (head == nullptr)
        return false;

    if (head->data == target) {
        push_front(value);
        return true;
    }

    SNode* current = head;
    while (current->next != nullptr && current->next->data != target) {
        current = current->next;
    }

    if (current->next == nullptr)
        return false;

    SNode* new_node = new SNode(value);
    new_node->next = current->next;
    current->next = new_node;
    size++;
    return true;
}

bool SingleList::insert_after(const string& target, const string& value) {
    if (head == nullptr)
        return false;

    SNode* current = head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }

    if (current == nullptr)
        return false;

    SNode* new_node = new SNode(value);
    new_node->next = current->next;
    current->next = new_node;

    if (current == tail) {
        tail = new_node;
    }
    size++;
    return true;
}

bool SingleList::pop_front() {
    if (head == nullptr)
        return false;

    SNode* temp = head;
    head = head->next;

    if (head == nullptr) {
        tail = nullptr;
    }

    delete temp;
    size--;
    return true;
}

bool SingleList::pop_back() {
    if (head == nullptr)
        return false;

    if (head == tail) {
        delete head;
        head = nullptr;
        tail = nullptr;
        size--;
        return true;
    }

    SNode* current = head;
    while (current->next != tail) {
        current = current->next;
    }

    delete tail;
    current->next = nullptr;
    tail = current;
    size--;
    return true;
}

bool SingleList::remove_before(const string& target) {
    if (head == nullptr || head->data == target)
        return false;

    if (head->next != nullptr && head->next->data == target) {
        return pop_front();
    }

    SNode* current = head;
    while (current->next != nullptr && current->next->next != nullptr) {
        if (current->next->next->data == target) {
            SNode* temp = current->next;
            current->next = temp->next;
            delete temp;
            size--;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SingleList::remove_after(const string& target) {
    SNode* current = head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }

    if (current == nullptr || current->next == nullptr)
        return false;

    SNode* temp = current->next;
    current->next = temp->next;

    if (temp == tail) {
        tail = current;
    }

    delete temp;
    size--;
    return true;
}

bool SingleList::remove_value(const string& value) {
    if (head == nullptr)
        return false;

    if (head->data == value) {
        return pop_front();
    }

    SNode* current = head;
    while (current->next != nullptr && current->next->data != value) {
        current = current->next;
    }

    if (current->next == nullptr)
        return false;

    SNode* temp = current->next;
    current->next = temp->next;

    if (temp == tail) {
        tail = current;
    }

    delete temp;
    size--;
    return true;
}

SNode* SingleList::find(const string& value) {
    SNode* current = head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void SingleList::print_forward() const {
    if (head == nullptr) {
        cout << "Односвязный список пуст" << endl;
        return;
    }

    cout << "Односвязный список [" << size << "]: ";
    SNode* current = head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " -> ";
        }
        current = current->next;
    }
    cout << endl;
}

void SingleList::print_backward_helper(SNode* node) const {
    if (node == nullptr)
        return;
    print_backward_helper(node->next);
    cout << node->data;
    if (node != head) {
        cout << " <- ";
    }
}

void SingleList::print_backward() const {
    if (head == nullptr) {
        cout << "Односвязный список пуст" << endl;
        return;
    }

    cout << "Односвязный список в обратном порядке [" << size << "]: ";
    print_backward_helper(head);
    cout << endl;
}

int SingleList::get_size() const {
    return size;
}

bool SingleList::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    SNode* current = head;
    while (current != nullptr) {
        size_t str_size = current->data.size();
        file.write(reinterpret_cast<const char*>(&str_size), sizeof(str_size));
        file.write(current->data.c_str(), str_size);
        current = current->next;
    }

    return true;
}

bool SingleList::deserialize_binary(const string& filename) {
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

bool SingleList::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << size << endl;

    SNode* current = head;
    while (current != nullptr) {
        file << current->data << endl;
        current = current->next;
    }

    return true;
}

bool SingleList::deserialize_text(const string& filename) {
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