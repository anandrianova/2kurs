#ifndef FBT_H
#define FBT_H

#include <string>
using namespace std;

// Узел бинарного дерева
struct TreeNode {
    int key;           // Ключ узла (целое число)
    string value;      // Значение узла (строка)
    TreeNode* left;    // Указатель на левого потомка
    TreeNode* right;   // Указатель на правого потомка
};

// Структура полного бинарного дерева
struct FullBinaryTree {
    TreeNode* root;    // Корень дерева
    int size;          // Количество узлов в дереве
};

// Создание дерева
FullBinaryTree* create_full_binary_tree();

bool tree_insert(FullBinaryTree* tree, int key, const string& value);// Добавление элемента (сохраняя свойство полного бинарного дерева)
string tree_search(const FullBinaryTree* tree, int key);// Поиск элемента по ключу
bool is_full_binary_tree(const FullBinaryTree* tree);// Проверка является ли дерево полным бинарным деревом
int tree_height(const TreeNode* node);// Получение высоты дерева
int tree_size(const FullBinaryTree* tree);// Подсчет количества узлов
void tree_print(const FullBinaryTree* tree);// Чтение дерева (визуальное представление)
void tree_free(FullBinaryTree* tree);// Освобождение памяти

// Обходы дерева
void tree_inorder(const FullBinaryTree* tree);
void tree_preorder(const FullBinaryTree* tree);
void tree_postorder(const FullBinaryTree* tree);
void tree_level_order(const FullBinaryTree* tree);

#endif