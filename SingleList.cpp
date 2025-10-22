#include "SingleList.h"
#include <iostream>

SingleList* create_singly_linked_list() {
    SingleList* list = new SingleList;
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0; //начальный размер
    return list;
}

//добавление в голову
void sll_push_front(SingleList* list, const string& value) {
    SNode* new_node = new SNode;
    new_node->data = value;
    new_node->next = list->head; //новый указывает на "старый"
    
    list->head = new_node; 

    //если список пуст, то и голова и хвост
    if (list->tail == nullptr) {
        list->tail = new_node;
    }
    list->size++; //увеличение размера
}

//добавление в хвост
void sll_push_back(SingleList* list, const string& value) {
    SNode* new_node = new SNode;
    new_node->data = value;
    new_node->next = nullptr; // новый хвост
    
    if (list->tail == nullptr) { // если пуст то и голова и хвост
        list->head = new_node;
        list->tail = new_node;
    } else { // после текущего хвоста
        list->tail->next = new_node;
        list->tail = new_node; //обновление
    }
    list->size++;
}

//вставка перед элементом
bool sll_insert_before(SingleList* list, const string& target, const string& value) {
    if (list->head == nullptr) return false; //если пустой
    
    // Если 1 элемент
    if (list->head->data == target) {
        sll_push_front(list, value);
        return true;
    }
    
    // Ищем узел перед элементом
    SNode* current = list->head;
    while (current->next != nullptr && current->next->data != target) {
        current = current->next;
    }
    
    if (current->next == nullptr) return false; // если не найден
    
    // создаем и вставляем узел
    SNode* new_node = new SNode;
    new_node->data = value;
    new_node->next = current->next; //новый указывает на следующий элемент
    current->next = new_node; //старый на новый
    list->size++;
    return true;
}

//вставка после элемента
bool sll_insert_after(SingleList* list, const string& target, const string& value) {
    if (list->head == nullptr) return false;
    
    //поиск 
    SNode* current = list->head;
    while (current != nullptr && current->data != target) {
        current = current->next;
    }
    
    if (current == nullptr) return false; // элемент не найден
    
    // создаем и вставляем узел
    SNode* new_node = new SNode;
    new_node->data = value;
    new_node->next = current->next;
    current->next = new_node; 
    
    if (current == list->tail) { //если после хвоста, то обновляем хвост
        list->tail = new_node;
    }
    list->size++;
    return true;
}

//удаление головы
bool sll_pop_front(SingleList* list) {
    if (list->head == nullptr) return false; //пустой
    
    SNode* temp = list->head; //запоминаем старую голову
    list->head = list->head->next; // узел на след элемент
    
    if (list->head == nullptr) { //если стал пустым,обновление
        list->tail = nullptr;
    }
    
    delete temp; //очистка памяти от старой головы
    list->size--; 
    return true;
}

//удаление хвоста
bool sll_pop_back(SingleList* list) {
    if (list->head == nullptr) return false;//пустой
    
    if (list->head == list->tail) {
        //только 1 элемент
        delete list->head;
        list->head = nullptr;
        list->tail = nullptr;
        list->size--;
        return true;
    }
    
    //поиск предпоследнего узла
    SNode* current = list->head;
    while (current->next != list->tail) {
        current = current->next;
    }

    //удаление и обновление
    delete list->tail;
    current->next = nullptr;
    list->tail = current;
    list->size--;
    return true;
}

//удаление по значению
bool sll_remove_value(SingleList* list, const string& value) {
    if (list->head == nullptr) return false; //пустой
    
    // Если значение в голове
    if (list->head->data == value) {
        return sll_pop_front(list);
    }
    
    //поиск 
    SNode* current = list->head;
    while (current->next != nullptr && current->next->data != value) {
        current = current->next;
    }
    
    if (current->next == nullptr) return false; // значение не найдено
    
    SNode* temp = current->next;
    current->next = temp->next;
    
    if (temp == list->tail) { //хвост, обновление
        list->tail = current;
    }
    
    delete temp;
    list->size--;
    return true;
}

//поиск по значению
SNode* sll_find(SingleList* list, const string& value) {
    SNode* current = list->head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

//вывод от головы до хвоста
void sll_print_forward(const SingleList* list) {
    if (list->head == nullptr) {
        cout << "Односвязный список пуст" << endl;
        return;
    }
    
    cout << "Односвязный список [" << list->size << "]: ";
    SNode* current = list->head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " -> ";
        }
        current = current->next;
    }
    cout << endl;
}

//вспомогательная функция для обратного вывода
void sll_print_backward_helper(SNode* node) {
    if (node == nullptr) return;
    sll_print_backward_helper(node->next);
    cout << node->data;
    if (node->next != nullptr) {
        cout << " <- ";
    }
}

void sll_print_backward(const SingleList* list) {
    if (list->head == nullptr) {
        cout << "Односвязный список пуст" << endl;
        return;
    }
    
    cout << "Односвязный список в обратном порядке [" << list->size << "]: ";
    sll_print_backward_helper(list->head);
    cout << endl;
}

//вывод кол-ва
int sll_size(const SingleList* list) {
    return list->size;
}

//очистка памяти
void sll_free(SingleList* list) {
    SNode* current = list->head;
    //последовательное удаление
    while (current != nullptr) {
        SNode* temp = current; //запоминаем
        current = current->next; //переход к следующему
        delete temp; //удаление 
    }
    delete list;
}