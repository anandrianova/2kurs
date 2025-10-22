#ifndef DOUBLELIST_H
#define DOUBLELIST_H

#include <string>
using namespace std;

// Узел двусвязного списка
struct DNode {
    string data;
    DNode* prev; //предыдущий
    DNode* next; //следующий
};

// Двусвязный список
struct DoubleList {
    DNode* head; //голова
    DNode* tail; //хвост
    int size; //кол-во
};

// Создание списка
DoubleList* create_doubly_linked_list();


void dll_push_front(DoubleList* list, const string& value);// Добавление в голову
void dll_push_back(DoubleList* list, const string& value);// Добавление в хвост
bool dll_insert_before(DoubleList* list, const string& target, const string& value);// Добавление до элемента
bool dll_insert_after(DoubleList* list, const string& target, const string& value);// Добавление после элемента
bool dll_pop_front(DoubleList* list);// Удаление из головы
bool dll_pop_back(DoubleList* list);// Удаление из хвоста
bool dll_remove_before(DoubleList* list, const string& target);// Удаление до элемента
bool dll_remove_after(DoubleList* list, const string& target);// Удаление после элемента
bool dll_remove_value(DoubleList* list, const string& value);// Удаление по значению
DNode* dll_find(DoubleList* list, const string& value);// Поиск элемента
void dll_print_forward(const DoubleList* list);// Чтение списка (прямой порядок)
void dll_print_backward(const DoubleList* list);// Чтение списка (обратный порядок)
int dll_size(const DoubleList* list);// Получение размера
void dll_free(DoubleList* list);// Освобождение памяти

#endif