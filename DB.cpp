#include "DB.h"
#include "Array.h"
#include "SingleList.h"
#include "DoubleList.h"
#include "Stack.h"
#include "Queue.h"
#include "FBT.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>

Database* create_database() {
    return new Database;
}

//освобождение памяти
void free_database(Database* db) {
    // Освобождаем массивы
    for (auto& pair : db->arrays) {
        array_free(static_cast<Array*>(pair.second));
    }
    
    // Освобождаем односвязные списки
    for (auto& pair : db->singly_lists) {
        sll_free(static_cast<SingleList*>(pair.second));
    }
    
    // Освобождаем двусвязные списки
    for (auto& pair : db->doubly_lists) {
        dll_free(static_cast<DoubleList*>(pair.second));
    }
    
    // Освобождаем стеки
    for (auto& pair : db->stacks) {
        stack_free(static_cast<Stack*>(pair.second));
    }
    
    // Освобождаем очереди
    for (auto& pair : db->queues) {
        queue_free(static_cast<Queue*>(pair.second));
    }
    
    // Освобождаем деревья
    for (auto& pair : db->trees) {
        tree_free(static_cast<FullBinaryTree*>(pair.second));
    }
    
    delete db;
}

//сохранене в бд
bool save_database(const Database* db, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Сохраняем массивы
    for (const auto& pair : db->arrays) {
        const Array* arr = static_cast<const Array*>(pair.second);
        file << "ARRAY " << pair.first << " " << array_length(arr) << " ";
        for (int i = 0; i < array_length(arr); i++) {
            file << array_get(arr, i) << " ";
        }
        file << "\n";
    }
    
    // Сохраняем односвязные списки
    for (const auto& pair : db->singly_lists) {
        const SingleList* list = static_cast<const SingleList*>(pair.second);
        file << "SINGLY_LIST " << pair.first << " " << sll_size(list) << " ";
        SNode* current = list->head;
        while (current != nullptr) {
            file << current->data << " ";
            current = current->next;
        }
        file << "\n";
    }
    
    // Сохраняем двусвязные списки
    for (const auto& pair : db->doubly_lists) {
        const DoubleList* list = static_cast<const DoubleList*>(pair.second);
        file << "DOUBLY_LIST " << pair.first << " " << dll_size(list) << " ";
        DNode* current = list->head;
        while (current != nullptr) {
            file << current->data << " ";
            current = current->next;
        }
        file << "\n";
    }
    
    // Сохраняем стеки
    for (const auto& pair : db->stacks) {
        const Stack* stack = static_cast<const Stack*>(pair.second);
        file << "STACK " << pair.first << " " << stack_size(stack) << " ";
        // Для стека сохраняем в обратном порядке
        vector<string> elements;
        const Stack* original = stack;
        
        // Копируем стек
        Stack* copy = create_stack(original->capacity);
        copy->top = original->top;
        for (int i = 0; i <= original->top; i++) {
            copy->data[i] = original->data[i];
        }
        
        while (!stack_is_empty(copy)) {
            elements.push_back(stack_pop(copy));
        }
        
        for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
            file << *it << " ";
        }
        stack_free(copy);
        file << "\n";
    }
    
    // Сохраняем очереди
    for (const auto& pair : db->queues) {
        const Queue* queue = static_cast<const Queue*>(pair.second);
        file << "QUEUE " << pair.first << " " << queue_size(queue) << " ";
        for (int i = 0; i < queue_size(queue); i++) {
            int index = (queue->front + i) % queue->capacity;
            file << queue->data[index] << " ";
        }
        file << "\n";
    }
    
    // Сохраняем деревья
    for (const auto& pair : db->trees) {
        const FullBinaryTree* tree = static_cast<const FullBinaryTree*>(pair.second);
        file << "TREE " << pair.first << " " << tree_size(tree) << " ";
        
        // Сохраняем дерево в порядке уровней
        if (tree->root != nullptr) {
            queue<const TreeNode*> q;
            q.push(tree->root);
            
            while (!q.empty()) {
                const TreeNode* current = q.front();
                q.pop();
                
                file << current->key << " " << current->value << " ";
                
                if (current->left != nullptr) {
                    q.push(current->left);
                }
                if (current->right != nullptr) {
                    q.push(current->right);
                }
            }
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

bool load_database(Database* db, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type, name;
        int size;
        
        iss >> type >> name >> size;
        
        if (type == "ARRAY") {
            Array* arr = create_array(size);
            string value;
            for (int i = 0; i < size; i++) {
                iss >> value;
                array_push_back(arr, value);
            }
            db->arrays[name] = arr;
        }
        else if (type == "SINGLY_LIST") {
            SingleList* list = create_singly_linked_list();
            string value;
            for (int i = 0; i < size; i++) {
                iss >> value;
                sll_push_back(list, value);
            }
            db->singly_lists[name] = list;
        }
        else if (type == "DOUBLY_LIST") {
            DoubleList* list = create_doubly_linked_list();
            string value;
            for (int i = 0; i < size; i++) {
                iss >> value;
                dll_push_back(list, value);
            }
            db->doubly_lists[name] = list;
        }
        else if (type == "STACK") {
            Stack* stack = create_stack(size);
            string value;
            for (int i = 0; i < size; i++) {
                iss >> value;
                stack_push(stack, value);
            }
            db->stacks[name] = stack;
        }
        else if (type == "QUEUE") {
            Queue* queue = create_queue(size);
            string value;
            for (int i = 0; i < size; i++) {
                iss >> value;
                queue_push(queue, value);
            }
            db->queues[name] = queue;
        }
        else if (type == "TREE") {
            FullBinaryTree* tree = create_full_binary_tree();
            int key;
            string value;
            
            for (int i = 0; i < size; i++) {
                iss >> key >> value;
                tree_insert(tree, key, value);
            }
            db->trees[name] = tree;
        }
    }
    
    file.close();
    return true;
}

// Вспомогательные функции для разбора команд
vector<string> split_command(const string& command) {
    vector<string> tokens;
    istringstream iss(command);
    string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

string execute_command(Database* db, const string& command) {
    vector<string> tokens = split_command(command);
    
    if (tokens.empty()) {
        return "ERROR: Empty command";
    }
    
    string cmd = tokens[0];
    
    // Команда PRINT для любого контейнера
    if (cmd == "PRINT") {
        if (tokens.size() < 2) {
            return "ERROR: PRINT requires container name";
        }
        
        string container_name = tokens[1];
        
        // Проверяем все типы контейнеров
        if (db->arrays.find(container_name) != db->arrays.end()) {
            Array* arr = static_cast<Array*>(db->arrays[container_name]);
            array_print(arr);
            return "SUCCESS";
        }
        else if (db->singly_lists.find(container_name) != db->singly_lists.end()) {
            SingleList* list = static_cast<SingleList*>(db->singly_lists[container_name]);
            sll_print_forward(list);
            return "SUCCESS";
        }
        else if (db->doubly_lists.find(container_name) != db->doubly_lists.end()) {
            DoubleList* list = static_cast<DoubleList*>(db->doubly_lists[container_name]);
            dll_print_forward(list);
            return "SUCCESS";
        }
        else if (db->stacks.find(container_name) != db->stacks.end()) {
            Stack* stack = static_cast<Stack*>(db->stacks[container_name]);
            stack_print(stack);
            return "SUCCESS";
        }
        else if (db->queues.find(container_name) != db->queues.end()) {
            Queue* queue = static_cast<Queue*>(db->queues[container_name]);
            queue_print(queue);
            return "SUCCESS";
        }
        else if (db->trees.find(container_name) != db->trees.end()) {
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[container_name]);
            tree_print(tree);
            return "SUCCESS";
        }
        else {
            return "ERROR: Container not found: " + container_name;
        }
    }
    
    // Обработка команд для массивов (M)
    else if (cmd.substr(0, 1) == "M") {
        if (tokens.size() < 2) {
            return "ERROR: Array command requires container name";
        }
        
        string array_name = tokens[1];
        
        if (cmd == "MCREATE") {
            if (db->arrays.find(array_name) != db->arrays.end()) {
                return "ERROR: Array already exists: " + array_name;
            }
            db->arrays[array_name] = create_array();
            return "SUCCESS: Array created: " + array_name;
        }
        else if (cmd == "MPUSH") {
            if (tokens.size() < 3) {
                return "ERROR: MPUSH requires value";
            }
            if (db->arrays.find(array_name) == db->arrays.end()) {
                return "ERROR: Array not found: " + array_name;
            }
            Array* arr = static_cast<Array*>(db->arrays[array_name]);
            array_push_back(arr, tokens[2]);
            return "SUCCESS: Value pushed to array";
        }
        else if (cmd == "MINSERT") {
            if (tokens.size() < 4) {
                return "ERROR: MINSERT requires index and value";
            }
            if (db->arrays.find(array_name) == db->arrays.end()) {
                return "ERROR: Array not found: " + array_name;
            }
            int index = stoi(tokens[2]);
            Array* arr = static_cast<Array*>(db->arrays[array_name]);
            if (array_insert(arr, index, tokens[3])) {
                return "SUCCESS: Value inserted at index " + tokens[2];
            } else {
                return "ERROR: Invalid index";
            }
        }
        else if (cmd == "MGET") {
            if (tokens.size() < 3) {
                return "ERROR: MGET requires index";
            }
            if (db->arrays.find(array_name) == db->arrays.end()) {
                return "ERROR: Array not found: " + array_name;
            }
            int index = stoi(tokens[2]);
            Array* arr = static_cast<Array*>(db->arrays[array_name]);
            string value = array_get(arr, index);
            if (!value.empty()) {
                return "VALUE: " + value;
            } else {
                return "ERROR: Invalid index or empty value";
            }
        }
        else if (cmd == "MDEL") {
            if (tokens.size() < 3) {
                return "ERROR: MDEL requires index";
            }
            if (db->arrays.find(array_name) == db->arrays.end()) {
                return "ERROR: Array not found: " + array_name;
            }
            int index = stoi(tokens[2]);
            Array* arr = static_cast<Array*>(db->arrays[array_name]);
            if (array_remove(arr, index)) {
                return "SUCCESS: Element removed at index " + tokens[2];
            } else {
                return "ERROR: Invalid index";
            }
        }
        else if (cmd == "MREPLACE") {
            if (tokens.size() < 4) {
                return "ERROR: MREPLACE requires index and value";
            }
            if (db->arrays.find(array_name) == db->arrays.end()) {
                return "ERROR: Array not found: " + array_name;
            }
            int index = stoi(tokens[2]);
            Array* arr = static_cast<Array*>(db->arrays[array_name]);
            if (array_replace(arr, index, tokens[3])) {
                return "SUCCESS: Value replaced at index " + tokens[2];
            } else {
                return "ERROR: Invalid index";
            }
        }
        else if (cmd == "MSIZE") {
            if (db->arrays.find(array_name) == db->arrays.end()) {
                return "ERROR: Array not found: " + array_name;
            }
            Array* arr = static_cast<Array*>(db->arrays[array_name]);
            return "SIZE: " + to_string(array_length(arr));
        }
    }
    
    // Обработка команд для односвязных списков (F)
    else if (cmd.substr(0, 1) == "F") {
        if (tokens.size() < 2) {
            return "ERROR: Singly list command requires container name";
        }
        
        string list_name = tokens[1];
        
        if (cmd == "FCREATE") {
            if (db->singly_lists.find(list_name) != db->singly_lists.end()) {
                return "ERROR: Singly list already exists: " + list_name;
            }
            db->singly_lists[list_name] = create_singly_linked_list();
            return "SUCCESS: Singly list created: " + list_name;
        }
        else if (cmd == "FPUSH") {
            if (tokens.size() < 4) {
                return "ERROR: FPUSH requires type and value";
            }
            if (db->singly_lists.find(list_name) == db->singly_lists.end()) {
                return "ERROR: Singly list not found: " + list_name;
            }
            
            string push_type = tokens[2];
            SingleList* list = static_cast<SingleList*>(db->singly_lists[list_name]);
            
            if (push_type == "FRONT") {
                string value = tokens[3];
                sll_push_front(list, value);
                return "SUCCESS: Value pushed to front";
            }
            else if (push_type == "BACK") {
                string value = tokens[3];
                sll_push_back(list, value);
                return "SUCCESS: Value pushed to back";
            }
            else if (push_type == "BEFORE") {
                if (tokens.size() < 5) {
                    return "ERROR: FPUSH BEFORE requires target value";
                }
                string target = tokens[3];
                string value = tokens[4];
                if (sll_insert_before(list, target, value)) {
                    return "SUCCESS: Value inserted before target";
                } else {
                    return "ERROR: Target not found";
                }
            }
            else if (push_type == "AFTER") {
                if (tokens.size() < 5) {
                    return "ERROR: FPUSH AFTER requires target value";
                }
                string target = tokens[3];
                string value = tokens[4];
                if (sll_insert_after(list, target, value)) {
                    return "SUCCESS: Value inserted after target";
                } else {
                    return "ERROR: Target not found";
                }
            }
            else {
                return "ERROR: Invalid push type. Use FRONT/BACK/BEFORE/AFTER";
            }
        }
        else if (cmd == "FDEL") {
            if (tokens.size() < 3) {
                return "ERROR: FDEL requires type";
            }
            if (db->singly_lists.find(list_name) == db->singly_lists.end()) {
                return "ERROR: Singly list not found: " + list_name;
            }
            
            string del_type = tokens[2];
            SingleList* list = static_cast<SingleList*>(db->singly_lists[list_name]);
            
            if (del_type == "FRONT") {
                if (sll_pop_front(list)) {
                    return "SUCCESS: Front element removed";
                } else {
                    return "ERROR: List is empty";
                }
            }
            else if (del_type == "BACK") {
                if (sll_pop_back(list)) {
                    return "SUCCESS: Back element removed";
                } else {
                    return "ERROR: List is empty";
                }
            }
            else if (del_type == "VALUE") {
                if (tokens.size() < 4) {
                    return "ERROR: FDEL VALUE requires target value";
                }
                if (sll_remove_value(list, tokens[3])) {
                    return "SUCCESS: Value removed";
                } else {
                    return "ERROR: Value not found";
                }
            }
            else if (del_type == "BEFORE") {
                if (tokens.size() < 4) {
                    return "ERROR: FDEL BEFORE requires target value";
                }
                if (sll_remove_value(list, tokens[3])) {
                    return "SUCCESS: Element before target removed";
                } else {
                    return "ERROR: Cannot remove before target";
                }
            }
            else if (del_type == "AFTER") {
                if (tokens.size() < 4) {
                    return "ERROR: FDEL AFTER requires target value";
                }
                if (sll_remove_value(list, tokens[3])) {
                    return "SUCCESS: Element after target removed";
                } else {
                    return "ERROR: Cannot remove after target";
                }
            }
            else {
                return "ERROR: Invalid delete type. Use FRONT/BACK/VALUE/BEFORE/AFTER";
            }
        }
        else if (cmd == "FGET") {
            if (db->singly_lists.find(list_name) == db->singly_lists.end()) {
                return "ERROR: Singly list not found: " + list_name;
            }
            SingleList* list = static_cast<SingleList*>(db->singly_lists[list_name]);
            
            if (tokens.size() == 2) {
                // Возвращаем весь список
                string result = "LIST: ";
                SNode* current = list->head;
                while (current != nullptr) {
                    result += current->data;
                    if (current->next != nullptr) result += " -> ";
                    current = current->next;
                }
                return result;
            }
            else if (tokens.size() == 3) {
                // Поиск элемента
                SNode* found = sll_find(list, tokens[2]);
                if (found) {
                    return "FOUND: " + found->data;
                } else {
                    return "NOT_FOUND";
                }
            }
            else {
                return "ERROR: FGET requires either no arguments (to display list) or one argument (to search)";
            }
        }
        else if (cmd == "FSIZE") {
            if (db->singly_lists.find(list_name) == db->singly_lists.end()) {
                return "ERROR: Singly list not found: " + list_name;
            }
            SingleList* list = static_cast<SingleList*>(db->singly_lists[list_name]);
            return "SIZE: " + to_string(sll_size(list));
        }
        else if (cmd == "FPRINT_BACKWARD") {
            if (db->singly_lists.find(list_name) == db->singly_lists.end()) {
                return "ERROR: Singly list not found: " + list_name;
            }
            SingleList* list = static_cast<SingleList*>(db->singly_lists[list_name]);
            sll_print_backward(list);
            return "SUCCESS";
        }
    }
    
    // Обработка команд для двусвязных списков (L)
    else if (cmd.substr(0, 1) == "L") {
        if (tokens.size() < 2) {
            return "ERROR: Doubly list command requires container name";
        }
        
        string list_name = tokens[1];
        
        if (cmd == "LCREATE") {
            if (db->doubly_lists.find(list_name) != db->doubly_lists.end()) {
                return "ERROR: Doubly list already exists: " + list_name;
            }
            db->doubly_lists[list_name] = create_doubly_linked_list();
            return "SUCCESS: Doubly list created: " + list_name;
        }
        else if (cmd == "LPUSH") {
            if (tokens.size() < 4) {
                return "ERROR: LPUSH requires type and value";
            }
            if (db->doubly_lists.find(list_name) == db->doubly_lists.end()) {
                return "ERROR: Doubly list not found: " + list_name;
            }
            
            string push_type = tokens[2];
            string value = tokens[3];
            DoubleList* list = static_cast<DoubleList*>(db->doubly_lists[list_name]);
            
            if (push_type == "FRONT") {
                dll_push_front(list, value);
                return "SUCCESS: Value pushed to front";
            }
            else if (push_type == "BACK") {
                dll_push_back(list, value);
                return "SUCCESS: Value pushed to back";
            }
            else if (push_type == "BEFORE") {
                if (tokens.size() < 5) {
                    return "ERROR: LPUSH BEFORE requires target value";
                }
                string target = tokens[3];
                string value = tokens[4];
                if (dll_insert_before(list, target, value)) {
                    return "SUCCESS: Value inserted before target";
                } else {
                    return "ERROR: Target not found";
                }
            }
            else if (push_type == "AFTER") {
                if (tokens.size() < 5) {
                    return "ERROR: LPUSH AFTER requires target value";
                }
                string target = tokens[3];
                string value = tokens[4];
                if (dll_insert_after(list, target, value)) {
                    return "SUCCESS: Value inserted after target";
                } else {
                    return "ERROR: Target not found";
                }
            }
            else {
                return "ERROR: Invalid push type. Use FRONT/BACK/BEFORE/AFTER";
            }
        }
        else if (cmd == "LDEL") {
            if (tokens.size() < 3) {
                return "ERROR: LDEL requires type";
            }
            if (db->doubly_lists.find(list_name) == db->doubly_lists.end()) {
                return "ERROR: Doubly list not found: " + list_name;
            }
            
            string del_type = tokens[2];
            DoubleList* list = static_cast<DoubleList*>(db->doubly_lists[list_name]);
            
            if (del_type == "FRONT") {
                if (dll_pop_front(list)) {
                    return "SUCCESS: Front element removed";
                } else {
                    return "ERROR: List is empty";
                }
            }
            else if (del_type == "BACK") {
                if (dll_pop_back(list)) {
                    return "SUCCESS: Back element removed";
                } else {
                    return "ERROR: List is empty";
                }
            }
            else if (del_type == "VALUE") {
                if (tokens.size() < 4) {
                    return "ERROR: LDEL VALUE requires target value";
                }
                if (dll_remove_value(list, tokens[3])) {
                    return "SUCCESS: Value removed";
                } else {
                    return "ERROR: Value not found";
                }
            }
            else if (del_type == "BEFORE"){
                if (tokens.size() < 4){
                    return "ERROR: LDEL BEFORE requires target value";
                }
                if(dll_remove_before(list, tokens[3])){
                     return "SUCCESS: Value removed";
                }else {
                    return "ERROR: Value not found";
                }
            }
            else if (del_type == "AFTER"){
                if (tokens.size() < 4){
                    return "ERROR: LDEL BEFORE requires target value";
                }
                if(dll_remove_after(list, tokens[3])){
                     return "SUCCESS: Value removed";
                }else {
                    return "ERROR: Value not found";
                }
            }
            else {
                return "ERROR: Invalid delete type. Use FRONT/BACK/VALUE";
            }
        }
        else if (cmd == "LGET") {
            if (db->doubly_lists.find(list_name) == db->doubly_lists.end()) {
                return "ERROR: Doubly list not found: " + list_name;
            }
            DoubleList* list = static_cast<DoubleList*>(db->doubly_lists[list_name]);
            
            if (tokens.size() == 2) {
                // Возвращаем весь список
                string result = "LIST: ";
                DNode* current = list->head;
                while (current != nullptr) {
                    result += current->data;
                    if (current->next != nullptr) result += " <-> ";
                    current = current->next;
                }
                return result;
            }
            else if (tokens.size() == 3) {
                // Поиск элемента
                DNode* found = dll_find(list, tokens[2]);
                if (found) {
                    return "FOUND: " + found->data;
                } else {
                    return "NOT_FOUND";
                }
            }
        }
        else if (cmd == "LPRINT_BACKWARD") {
            if (db->doubly_lists.find(list_name) == db->doubly_lists.end()) {
                return "ERROR: Doubly list not found: " + list_name;
            }
            DoubleList* list = static_cast<DoubleList*>(db->doubly_lists[list_name]);
            dll_print_backward(list);
            return "SUCCESS";
        }
        else if (cmd == "LSIZE") {
            if (db->doubly_lists.find(list_name) == db->doubly_lists.end()) {
                return "ERROR: Doubly list not found: " + list_name;
            }
            DoubleList* list = static_cast<DoubleList*>(db->doubly_lists[list_name]);
            return "SIZE: " + to_string(dll_size(list));
        }
    }
    
    // Обработка команд для стеков (S)
    else if (cmd.substr(0, 1) == "S") {
        if (tokens.size() < 2) {
            return "ERROR: Stack command requires container name";
        }
        
        string stack_name = tokens[1];
        
        if (cmd == "SCREATE") {
            if (db->stacks.find(stack_name) != db->stacks.end()) {
                return "ERROR: Stack already exists: " + stack_name;
            }
            db->stacks[stack_name] = create_stack();
            return "SUCCESS: Stack created: " + stack_name;
        }
        else if (cmd == "SPUSH") {
            if (tokens.size() < 3) {
                return "ERROR: SPUSH requires value";
            }
            if (db->stacks.find(stack_name) == db->stacks.end()) {
                return "ERROR: Stack not found: " + stack_name;
            }
            Stack* stack = static_cast<Stack*>(db->stacks[stack_name]);
            stack_push(stack, tokens[2]);
            return "SUCCESS: Value pushed to stack";
        }
        else if (cmd == "SPOP") {
            if (db->stacks.find(stack_name) == db->stacks.end()) {
                return "ERROR: Stack not found: " + stack_name;
            }
            Stack* stack = static_cast<Stack*>(db->stacks[stack_name]);
            string value = stack_pop(stack);
            if (!value.empty()) {
                return "POPPED: " + value;
            } else {
                return "ERROR: Stack is empty";
            }
        }
        else if (cmd == "SPEEK") {
            if (db->stacks.find(stack_name) == db->stacks.end()) {
                return "ERROR: Stack not found: " + stack_name;
            }
            Stack* stack = static_cast<Stack*>(db->stacks[stack_name]);
            string value = stack_peek(stack);
            if (!value.empty()) {
                return "PEEK: " + value;
            } else {
                return "ERROR: Stack is empty";
            }
        }
        else if (cmd == "SSIZE") {
            if (db->stacks.find(stack_name) == db->stacks.end()) {
                return "ERROR: Stack not found: " + stack_name;
            }
            Stack* stack = static_cast<Stack*>(db->stacks[stack_name]);
            return "SIZE: " + to_string(stack_size(stack));
        }
    }
    
    // Обработка команд для очередей (Q)
    else if (cmd.substr(0, 1) == "Q") {
        if (tokens.size() < 2) {
            return "ERROR: Queue command requires container name";
        }
        
        string queue_name = tokens[1];
        
        if (cmd == "QCREATE") {
            if (db->queues.find(queue_name) != db->queues.end()) {
                return "ERROR: Queue already exists: " + queue_name;
            }
            db->queues[queue_name] = create_queue();
            return "SUCCESS: Queue created: " + queue_name;
        }
        else if (cmd == "QPUSH") {
            if (tokens.size() < 3) {
                return "ERROR: QPUSH requires value";
            }
            if (db->queues.find(queue_name) == db->queues.end()) {
                return "ERROR: Queue not found: " + queue_name;
            }
            Queue* queue = static_cast<Queue*>(db->queues[queue_name]);
            queue_push(queue, tokens[2]);
            return "SUCCESS: Value pushed to queue";
        }
        else if (cmd == "QPOP") {
            if (db->queues.find(queue_name) == db->queues.end()) {
                return "ERROR: Queue not found: " + queue_name;
            }
            Queue* queue = static_cast<Queue*>(db->queues[queue_name]);
            string value = queue_pop(queue);
            if (!value.empty()) {
                return "POPPED: " + value;
            } else {
                return "ERROR: Queue is empty";
            }
        }
        else if (cmd == "QPEEK") {
            if (db->queues.find(queue_name) == db->queues.end()) {
                return "ERROR: Queue not found: " + queue_name;
            }
            Queue* queue = static_cast<Queue*>(db->queues[queue_name]);
            string value = queue_peek(queue);
            if (!value.empty()) {
                return "PEEK: " + value;
            } else {
                return "ERROR: Queue is empty";
            }
        }
        else if (cmd == "QSIZE") {
            if (db->queues.find(queue_name) == db->queues.end()) {
                return "ERROR: Queue not found: " + queue_name;
            }
            Queue* queue = static_cast<Queue*>(db->queues[queue_name]);
            return "SIZE: " + to_string(queue_size(queue));
        }
    }
    
    // Обработка команд для деревьев (T)
    else if (cmd.substr(0, 1) == "T") {
        if (tokens.size() < 2) {
            return "ERROR: Tree command requires container name";
        }
        
        string tree_name = tokens[1];
        
        if (cmd == "TCREATE") {
            if (db->trees.find(tree_name) != db->trees.end()) {
                return "ERROR: Tree already exists: " + tree_name;
            }
            db->trees[tree_name] = create_full_binary_tree();
            return "SUCCESS: Tree created: " + tree_name;
        }
        else if (cmd == "TINSERT") {
            if (tokens.size() < 4) {
                return "ERROR: TINSERT requires key and value";
            }
            if (db->trees.find(tree_name) == db->trees.end()) {
                return "ERROR: Tree not found: " + tree_name;
            }
            
            int key;
            try {
                key = stoi(tokens[2]);
            } catch (const exception& e) {
                return "ERROR: Invalid key format";
            }
            
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[tree_name]);
            if (tree_insert(tree, key, tokens[3])) {
                return "SUCCESS: Value inserted with key " + tokens[2];
            } else {
                return "ERROR: Failed to insert value";
            }
        }
        else if (cmd == "TSEARCH") {
            if (tokens.size() < 3) {
                return "ERROR: TSEARCH requires key";
            }
            if (db->trees.find(tree_name) == db->trees.end()) {
                return "ERROR: Tree not found: " + tree_name;
            }
            
            int key;
            try {
                key = stoi(tokens[2]);
            } catch (const exception& e) {
                return "ERROR: Invalid key format";
            }
            
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[tree_name]);
            string value = tree_search(tree, key);
            if (!value.empty()) {
                return "FOUND: " + value;
            } else {
                return "NOT_FOUND";
            }
        }
        else if (cmd == "TISFULL") {
            if (db->trees.find(tree_name) == db->trees.end()) {
                return "ERROR: Tree not found: " + tree_name;
            }
            
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[tree_name]);
            bool is_full = is_full_binary_tree(tree);
            return "IS_FULL: " + string(is_full ? "YES" : "NO");
        }
        else if (cmd == "THEIGHT") {
            if (db->trees.find(tree_name) == db->trees.end()) {
                return "ERROR: Tree not found: " + tree_name;
            }
            
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[tree_name]);
            int height = tree_height(tree->root);
            return "HEIGHT: " + to_string(height);
        }
        else if (cmd == "TSIZE") {
            if (db->trees.find(tree_name) == db->trees.end()) {
                return "ERROR: Tree not found: " + tree_name;
            }
            
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[tree_name]);
            return "SIZE: " + to_string(tree_size(tree));
        }
        else if (cmd == "TTRAVERSE") {
            if (tokens.size() < 3) {
                return "ERROR: TTRAVERSE requires type (INORDER/PREORDER/POSTORDER/LEVEL)";
            }
            if (db->trees.find(tree_name) == db->trees.end()) {
                return "ERROR: Tree not found: " + tree_name;
            }
            
            FullBinaryTree* tree = static_cast<FullBinaryTree*>(db->trees[tree_name]);
            string traverse_type = tokens[2];
            
            if (traverse_type == "INORDER") {
                tree_inorder(tree);
                return "SUCCESS";
            }
            else if (traverse_type == "PREORDER") {
                tree_preorder(tree);
                return "SUCCESS";
            }
            else if (traverse_type == "POSTORDER") {
                tree_postorder(tree);
                return "SUCCESS";
            }
            else if (traverse_type == "LEVEL") {
                tree_level_order(tree);
                return "SUCCESS";
            }
            else {
                return "ERROR: Invalid traverse type. Use INORDER/PREORDER/POSTORDER/LEVEL";
            }
        }
    }
    
    // Команды управления базой данных
    else if (cmd == "SAVE" || cmd == "save") {
        if (tokens.size() < 2) {
            return "ERROR: SAVE requires filename";
        }
        if (save_database(db, tokens[1])) {
            return "SUCCESS: Database saved to " + tokens[1];
        } else {
            return "ERROR: Failed to save database";
        }
    }
    else if (cmd == "LOAD" || cmd == "load") {
        if (tokens.size() < 2) {
            return "ERROR: LOAD requires filename";
        }
        if (load_database(db, tokens[1])) {
            return "SUCCESS: Database loaded from " + tokens[1];
        } else {
            return "ERROR: Failed to load database";
        }
    }
    else if (cmd == "HELP" || cmd == "help") {
        return "COMMANDS:\n"
               "PRINT <container> - Print any container\n"
               "MCREATE <имя> - Create array\n"
               "MPUSH <имя> <значение> - Push to array\n"
               "MINSERT <имя> <индекс> <значение> - Insert into array\n"
               "MREPLACE <имя> <индекс> <значение> - Replace element at index\n"
               "MGET <имя> <индекс> - Get from array\n"
               "MDEL <имя> <индекс> - Delete from array\n"
               "FCREATE <имя> - Create singly list\n"
               "FPUSH <имя> <тип> <target> <value> - Push to list (types: FRONT/BACK/BEFORE/AFTER)\n"
               "FDEL <имя> <тип> [значение] - Delete from list\n"
               "FGET <имя> <индекс> - Get from list\n"
               "LCREATE <имя> - Create doubly list\n"
               "LPUSH <имя> <тип> <target> <value> - Push to list (types: FRONT/BACK/BEFORE/AFTER)\n"
               "LDEL <имя> <тип> [значение] - Delete from list\n"
               "LGET <имя> <индекс> - Get from list\n"
               "SCREATE <имя> - Create stack\n"
               "SPUSH <имя> <значение> - Push to stack\n"
               "SPOP <имя> - Pop from stack\n"
               "QCREATE <имя> - Create queue\n"
               "QPUSH <имя> <значение> - Push to queue\n"
               "QPOP <имя> - Pop from queue\n"
               "TCREATE <имя> - Create full binary tree\n"
               "TINSERT <имя> <ключ> <значение> - Insert into tree\n"
               "TSEARCH <имя> <ключ> - Search in tree\n"
               "TISFULL <имя> - Check if tree is full\n"
               "TTRAVERSE <имя> <тип> - Traverse tree\n"
               "SAVE <filename> - Save database\n"
               "LOAD <filename> - Load database\n"
               "HELP - Show this help";
    }
    
    // Обработка неизвестных команд
    return "ERROR: Unknown command: " + cmd;
}