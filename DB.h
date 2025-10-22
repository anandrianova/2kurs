#ifndef DB_H
#define DB_H

#include <string>
#include <unordered_map>
using namespace std;

// Структура для хранения всех контейнеров
struct Database {
    unordered_map<string, void*> arrays;
    unordered_map<string, void*> singly_lists;
    unordered_map<string, void*> doubly_lists;
    unordered_map<string, void*> stacks;
    unordered_map<string, void*> queues;
    unordered_map<string, void*> trees;  
};

// Создание и управление базой данных
Database* create_database();
void free_database(Database* db);
bool save_database(const Database* db, const string& filename);
bool load_database(Database* db, const string& filename);

// Командный интерфейс
string execute_command(Database* db, const string& command);

#endif