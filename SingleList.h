#ifndef SINGLELIST_H
#define SINGLELIST_H

#include <string>
using namespace std;

// Узел односвязного списка
struct SNode {
    string data; //узел
    SNode* next; //указатель на след узел
};

// Односвязный список
struct SingleList {
    SNode* head; //первый узел
    SNode* tail; // последний
    int size; //кол-во
};

// Создание списка
SingleList* create_singly_linked_list();

//операции с массивом
void sll_push_front(SingleList* list, const string& value);// Добавление в голову
void sll_push_back(SingleList* list, const string& value);// Добавление в хвост
bool sll_insert_before(SingleList* list, const string& target, const string& value);// Добавление до элемента
bool sll_insert_after(SingleList* list, const string& target, const string& value);// Добавление после элемента
bool sll_pop_front(SingleList* list);// Удаление из головы
bool sll_pop_back(SingleList* list);// Удаление из хвоста
bool sll_remove_before(SingleList* list, const string& target);// Удаление до элемента
bool sll_remove_after(SingleList* list, const string& target);// Удаление после элемента
bool sll_remove_value(SingleList* list, const string& value);// Удаление по значению
SNode* sll_find(SingleList* list, const string& value);// Поиск элемента
void sll_print_forward(const SingleList* list);// Чтение списка (прямой порядок)
void sll_print_backward(const SingleList* list);// Чтение списка (обратный порядок - рекурсивно)
int sll_size(const SingleList* list);// Получение размера
void sll_free(SingleList* list);// Освобождение памяти

#endif