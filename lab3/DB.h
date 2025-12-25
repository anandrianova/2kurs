#ifndef DB_H
#define DB_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;

// Предварительные объявления классов
#include "Array.h"
#include "SingleList.h"
#include "DoubleList.h"
#include "Stack.h"
#include "Queue.h"
#include "FullBinaryTree.h"
#include "HashTable.h"

// Класс для управления базой данных контейнеров
class Database {
private:
    unordered_map<string, unique_ptr<Array>> arrays;
    unordered_map<string, unique_ptr<SingleList>> singly_lists;
    unordered_map<string, unique_ptr<DoubleList>> doubly_lists;
    unordered_map<string, unique_ptr<Stack>> stacks;
    unordered_map<string, unique_ptr<Queue>> queues;
    unordered_map<string, unique_ptr<FullBinaryTree>> trees;
    unordered_map<string, unique_ptr<DoubleHashTable>> hash_tables;  

    // Вспомогательные методы
    vector<string> splitCommand(const string& command) const;
    bool isArrayCommand(const string& cmd) const;
    bool isSinglyListCommand(const string& cmd) const;
    bool isDoublyListCommand(const string& cmd) const;
    bool isStackCommand(const string& cmd) const;
    bool isQueueCommand(const string& cmd) const;
    bool isTreeCommand(const string& cmd) const;
    bool isHashTableCommand(const string& cmd) const;  
public:
    Database() = default;
    ~Database() = default;
    
    // Запрещаем копирование
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    // Разрешаем перемещение
    Database(Database&&) = default;
    Database& operator=(Database&&) = default;
    
    // Управление базой данных
    bool saveToFile(const string& filename) const;
    bool loadFromFile(const string& filename);
    void clear();
    
    // Интерфейс команд
    string executeCommand(const string& command);
    
    // Методы для доступа к контейнерам (для тестирования)
    bool hasArray(const string& name) const { return arrays.find(name) != arrays.end(); }
    bool hasSinglyList(const string& name) const { return singly_lists.find(name) != singly_lists.end(); }
    bool hasDoublyList(const string& name) const { return doubly_lists.find(name) != doubly_lists.end(); }
    bool hasStack(const string& name) const { return stacks.find(name) != stacks.end(); }
    bool hasQueue(const string& name) const { return queues.find(name) != queues.end(); }
    bool hasTree(const string& name) const { return trees.find(name) != trees.end(); }
    bool hasHashTable(const string& name) const { return hash_tables.find(name) != hash_tables.end(); } 
    
    // Геттеры (только для чтения)
    const Array* getArray(const string& name) const;
    const SingleList* getSinglyList(const string& name) const;
    const DoubleList* getDoublyList(const string& name) const;
    const Stack* getStack(const string& name) const;
    const Queue* getQueue(const string& name) const;
    const FullBinaryTree* getTree(const string& name) const;
    const DoubleHashTable* getHashTable(const string& name) const;  

    // Статические методы для помощи
    static string getHelpText();
};

#endif