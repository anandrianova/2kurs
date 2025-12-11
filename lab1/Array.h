#ifndef ARRAY_H
#define ARRAY_H

#include <string>
using namespace std;

//структура для массива
struct Array
{
    string* data; //указывает на первый элемент массива
    int capacity; //размер массива
    int size; //кол-во элементов
};

//создание массива
Array* create_array(int initial_capacity=10);

//операции с массивом
void array_push_back(Array* arr, const string& value);//добавление в конец
bool array_insert(Array* arr, int index, const string& value);//по индксу добавление
string array_get(const Array* arr, int index);//поиск по индексу
bool array_remove(Array* arr, int index);//удалить по идндксу
bool array_replace(Array* arr, int index, const string& value);;//замена по индексу
int array_length(const Array* arr);//длина массива
void array_print(const Array* arr);//вывод массива
void array_free(Array* arr);//очистка памяти

#endif
