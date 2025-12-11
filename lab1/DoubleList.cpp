#include "DoubleList.h"
#include <iostream>

//создание двусвязного списка
DoubleList* create_doubly_linked_list() {
    DoubleList* list = new DoubleList;
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
    return list;
}

//добавление в голову
void dll_push_front(DoubleList* list, const string& value) {
    DNode* new_node = new DNode;
    new_node->data = value;
    new_node->prev = nullptr; //новый=голова
    new_node->next = list->head; //узел на старый
    
    //если не пустой, то узел на старую голову
    if (list->head != nullptr) {
        list->head->prev = new_node;
    }
    
    list->head = new_node; //новая голова
    
    //если пуст, то и голова и хвост
    if (list->tail == nullptr) {
        list->tail = new_node;
    }
    list->size++;
}

//в хвост
void dll_push_back(DoubleList* list, const string& value) {
    DNode* new_node = new DNode;
    new_node->data = value;
    new_node->next = nullptr; // новый=хвост
    new_node->prev = list->tail; //узел на старый хвост
    
    if (list->tail != nullptr) { //если не пустой, то обновление узла
        list->tail->next = new_node;
    }
    
    list->tail = new_node; // новый=хвост
    
    if (list->head == nullptr) { //если пустой, то голова и хвост
        list->head = new_node;
    }
    list->size++;
}

//вставка перед 
bool dll_insert_before(DoubleList* list, const string& target, const string& value) {
    //поиск элемента 
    DNode* current = list->head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }
    
    if (current == nullptr) return false; // не найден
    
    if (current == list->head) { //перед головой
        dll_push_front(list, value);
        return true;
    }
    
    //новый узел
    DNode* new_node = new DNode;
    new_node->data = value;
    new_node->prev = current->prev; //новй указывает на предыдущий 
    new_node->next = current; //слудеющий на новый
    
    current->prev->next = new_node; // следующий от старого на новый
    current->prev = new_node; //предыдущий от нового на новый
    
    list->size++;
    return true;
}

// после узла
bool dll_insert_after(DoubleList* list, const string& target, const string& value) {
    //поиск 
    DNode* current = list->head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }
    
    if (current == nullptr) return false; // не найден
    
    if (current == list->tail) { //после хвоста
        dll_push_back(list, value);
        return true;
    }
    
    //новый узел
    DNode* new_node = new DNode;
    new_node->data = value;
    new_node->prev = current; 
    new_node->next = current->next; 
    
    current->next->prev = new_node; //предыдущий старого на новый
    current->next = new_node; //следующий старого на новый
    
    list->size++;
    return true;
}

//удаление головы
bool dll_pop_front(DoubleList* list) {
    if (list->head == nullptr) return false; //пустой
    
    DNode* temp = list->head; //запоминаем старую
    list->head = list->head->next;//на след элемент
    
    if (list->head != nullptr) {//не пустой,обновление
        list->head->prev = nullptr;
    } else {//стал пустым, обновление хвоста
        list->tail = nullptr;
    }
    
    delete temp; 
    list->size--;
    return true;
}

//хвоста
bool dll_pop_back(DoubleList* list) {
    if (list->tail == nullptr) return false; //пустой
    
    DNode* temp = list->tail;//запоминаем старый
    list->tail = list->tail->prev;//на предыдущий
    
    if (list->tail != nullptr) {//не пкст,обновление нового хвоста
        list->tail->next = nullptr;
    } else {//если стал пустым, обновление
        list->head = nullptr;
    }
    
    delete temp;
    list->size--;
    return true;
}

//перед узлом
bool dll_remove_before(DoubleList* list, const string& target) {
    //поиск
    DNode* current = list->head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }
    
    if (current == nullptr || current->prev == nullptr) return false;//нет или нет элементов
    
    DNode* to_remove = current->prev;//узел удаления
    
    if (to_remove == list->head) {//если перед головой
        list->head = current;
        current->prev = nullptr;
    } else {//обновление связей
        to_remove->prev->next = current;
        current->prev = to_remove->prev;
    }
    
    delete to_remove;
    list->size--;
    return true;
}

//после
bool dll_remove_after(DoubleList* list, const string& target) {
    //поиск
    DNode* current = list->head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }
    
    if (current == nullptr || current->next == nullptr) return false;//нет или нет элементов
    
    DNode* to_remove = current->next;//узел удаления
    
    if (to_remove == list->tail) {//если после хвоста
        list->tail = current;
        current->next = nullptr;
    } else {//обновление связей
        current->next = to_remove->next;
        to_remove->next->prev = current;
    }
    
    delete to_remove;
    list->size--;
    return true;
}

//по значению
bool dll_remove_value(DoubleList* list, const string& value) {
    //поиск
    DNode* current = list->head;
    while (current != nullptr && current->data != value) {
        current = current->next;
    }
    
    if (current == nullptr) return false; // значение не найдено
    
    if (current == list->head) {//голова
        return dll_pop_front(list);
    }
    
    if (current == list->tail) {//хвост
        return dll_pop_back(list);
    }
    
     // Обновляем связи соседних узлов, обходя удаляемый
    current->prev->next = current->next;
    current->next->prev = current->prev;
    
    delete current;
    list->size--;
    return true;
}

//поиск по значению
DNode* dll_find(DoubleList* list, const string& value) {
    //поиск
    DNode* current = list->head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

//от головы до хвоста
void dll_print_forward(const DoubleList* list) {
    if (list->head == nullptr) {
        cout << "Двусвязный список пуст" << endl;
        return;
    }
    
    cout << "Двусвязный список [" << list->size << "]: ";
    DNode* current = list->head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " <-> ";
        }
        current = current->next;
    }
    cout << endl;
}

//от хвоста до головы
void dll_print_backward(const DoubleList* list) {
    if (list->tail == nullptr) {
        cout << "Двусвязный список пуст" << endl;
        return;
    }
    
    cout << "Двусвязный список в обратном порядке [" << list->size << "]: ";
    DNode* current = list->tail;
    while (current != nullptr) {
        cout << current->data;
        if (current->prev != nullptr) {
            cout << " <-> ";
        }
        current = current->prev;
    }
    cout << endl;
}

//кол-во элементов
int dll_size(const DoubleList* list) {
    return list->size;
}

//очистка памяти
void dll_free(DoubleList* list) {
    DNode* current = list->head;
    while (current != nullptr) {
        DNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete list;
}