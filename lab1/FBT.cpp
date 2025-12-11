#include "FBT.h"
#include <iostream>
#include <queue>
#include <cmath>
using namespace std;

FullBinaryTree* create_full_binary_tree() {
    FullBinaryTree* tree = new FullBinaryTree;
    tree->root = nullptr;//пустое
    tree->size = 0;
    return tree;
}

// Вспомогательная функция для добавления в полное бинарное дерево
TreeNode* insert_helper(TreeNode* root, int key, const string& value, bool& inserted) {
    if (root == nullptr) {
        inserted = true;
        TreeNode* new_node = new TreeNode;
        new_node->key = key;
        new_node->value = value;
        new_node->left = nullptr;
        new_node->right = nullptr;
        return new_node;
    }
    
    // Для полного бинарного дерева используем обход в ширину
    // и вставляем на первый доступный уровень
    queue<TreeNode*> q;
    q.push(root);
    
    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();
        
        // Если у текущего узла нет левого потомка - вставляем туда
        if (current->left == nullptr) {
            inserted = true;
            current->left = new TreeNode;
            current->left->key = key;
            current->left->value = value;
            current->left->left = nullptr;
            current->left->right = nullptr;
            return root;
        }
        // Если у текущего узла нет правого потомка - вставляем туда
        else if (current->right == nullptr) {
            inserted = true;
            current->right = new TreeNode;
            current->right->key = key;
            current->right->value = value;
            current->right->left = nullptr;
            current->right->right = nullptr;
            return root;
        }
        // Если оба потомка есть, добавляем их в очередь для дальнейшего обхода
        else {
            q.push(current->left);
            q.push(current->right);
        }
    }
    
    return root;
}

// Добавляет новый узел в полное бинарное дерево
bool tree_insert(FullBinaryTree* tree, int key, const string& value) {
    // Сначала проверяем, нет ли уже узла с таким ключом
    if (!tree_search(tree, key).empty()) {
        return false; // Ключ уже существует
    }
    
    bool inserted = false;
    tree->root = insert_helper(tree->root, key, value, inserted);
    if (inserted) {
        tree->size++;
    }
    return inserted;
}

// Рекурсивный поиск
string search_helper(const TreeNode* node, int key) {
    if (node == nullptr) {
        return "";// Узел не найден
    }
    
    if (node->key == key) { // Если нашли узел с нужным ключом
        return node->value;
    }
    
    // Поиск в левом поддереве
    string left_result = search_helper(node->left, key);
    if (!left_result.empty()) {
        return left_result;// Нашли в левом поддереве
    }
    
    // Поиск в правом поддереве
    return search_helper(node->right, key);
}

// Ищет узел по ключу и возвращает его значение
string tree_search(const FullBinaryTree* tree, int key) {
    return search_helper(tree->root, key);
}

// Проверка является ли дерево полным бинарным деревом
bool is_full_binary_tree_helper(const TreeNode* node) {
    if (node == nullptr) {
        return true;
    }
    
    // Если у узла нет детей - это корректно
    if (node->left == nullptr && node->right == nullptr) {
        return true;
    }
    
    // Если у узла оба ребенка - проверяем рекурсивно
    if (node->left != nullptr && node->right != nullptr) {
        return is_full_binary_tree_helper(node->left) && 
               is_full_binary_tree_helper(node->right);
    }
    
    // Если только один ребенок - это не полное бинарное дерево
    return false;
}

// Основная функция проверки свойства полного бинарного дерева
bool is_full_binary_tree(const FullBinaryTree* tree) {
    return is_full_binary_tree_helper(tree->root);
}

// Высота дерева - длина самого длинного пути от корня до листа
int tree_height(const TreeNode* node) {
    if (node == nullptr) {//пустое
        return 0;
    }
    
    //высота поддеревьев
    int left_height = tree_height(node->left);
    int right_height = tree_height(node->right);
    
    return max(left_height, right_height) + 1;// Высота текущего узла = максимальная высота поддеревьев + 1 (текущий узел)
}

// Функция получения количества узлов в дереве
int tree_size(const FullBinaryTree* tree) {
    return tree->size;
}

// Вспомогательные функции для обходов
void inorder_helper(const TreeNode* node) {// Центрированный обход (In-order): левый потомок -> корень -> правый потомок
    if (node == nullptr) return;
    
    inorder_helper(node->left);// Рекурсивно обходим левое поддерево
    cout << node->key << ":" << node->value << " ";
    inorder_helper(node->right);// Рекурсивно обходим правое поддерево
}

void preorder_helper(const TreeNode* node) {// Прямой обход (Pre-order): корень -> левый потомок -> правый потомок
    if (node == nullptr) return;
    
    cout << node->key << ":" << node->value << " ";
    preorder_helper(node->left);// Рекурсивно обходим левое поддерево
    preorder_helper(node->right);// Рекурсивно обходим правое поддерево
}

void postorder_helper(const TreeNode* node) {// Обратный обход (Post-order): левый потомок -> правый потомок -> корень
    if (node == nullptr) return;
    
    postorder_helper(node->left);// Рекурсивно обходим левое поддерево
    postorder_helper(node->right);// Рекурсивно обходим правое поддерево
    cout << node->key << ":" << node->value << " ";
}

// Основные функции обхода дерева
void tree_inorder(const FullBinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    cout << "Inorder: ";
    inorder_helper(tree->root);
    cout << endl;
}

void tree_preorder(const FullBinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    cout << "Preorder: ";
    preorder_helper(tree->root);
    cout << endl;
}

void tree_postorder(const FullBinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    cout << "Postorder: ";
    postorder_helper(tree->root);
    cout << endl;
}

// Обход в ширину (Level-order) с использованием очереди
// Посещает узлы уровень за уровнем, слева направо
void tree_level_order(const FullBinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    
    cout << "Level order: ";
    queue<const TreeNode*> q;
    q.push(tree->root);
    
    while (!q.empty()) {
        const TreeNode* current = q.front();
        q.pop();
        
        cout << current->key << ":" << current->value << " ";
        
        if (current->left != nullptr) {
            q.push(current->left);
        }
        if (current->right != nullptr) {
            q.push(current->right);
        }
    }
    cout << endl;
}

// Визуальное представление дерева
void print_tree_helper(const TreeNode* root, int space) {
    const int COUNT = 5; // Расстояние между уровнями
    
    if (root == nullptr) {
        return;
    }
    
    space += COUNT;
    
    // Сначала правый узел
    print_tree_helper(root->right, space);
    
    // Текущий узел
    cout << endl;
    for (int i = COUNT; i < space; i++) {
        cout << " ";
    }
    cout << root->key << ":" << root->value << endl;
    
    // Левый узел
    print_tree_helper(root->left, space);
}

void tree_print(const FullBinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Tree is empty" << endl;
        return;
    }
    
    cout << "Full Binary Tree structure:" << endl;
    print_tree_helper(tree->root, 0);
    cout << endl;
    
    // Проверка и вывод статуса
    bool is_full = is_full_binary_tree(tree);
    cout << "Is full binary tree: " << (is_full ? "YES" : "NO") << endl;
    cout << "Tree size: " << tree->size << endl;
    cout << "Tree height: " << tree_height(tree->root) << endl;
}

// Освобождение памяти (рекурсивное удаление)
void free_tree_helper(TreeNode* node) {
    if (node == nullptr) {
        return;
    }
    
    free_tree_helper(node->left);
    free_tree_helper(node->right);
    delete node;
}

void tree_free(FullBinaryTree* tree) {
    free_tree_helper(tree->root);
    delete tree;
}