#include <iostream>
#include <string>

using namespace std;

// Структура узла бинарного дерева
struct TreeNode {
    int key;           // Ключ узла
    string value;      // Значение узла
    TreeNode* left;    // Левый потомок
    TreeNode* right;   // Правый потомок
};

// Структура бинарного дерева
struct BinaryTree {
    TreeNode* root;    // Корень дерева
    int size;          // Количество узлов
};

// Структура узла очереди
struct QueueNode {
    TreeNode* tree_node;   // Указатель на узел дерева
    int level;             // Уровень узла
    QueueNode* next;       // Следующий элемент очереди
};

// Структура очереди
struct Queue {
    QueueNode* front;      // Начало очереди
    QueueNode* rear;       // Конец очереди
    int size;             // Размер очереди
};

// Функции для работы с очередью
Queue* create_queue() {
    Queue* queue = new Queue;
    queue->front = nullptr;
    queue->rear = nullptr;
    queue->size = 0;
    return queue;
}

void queue_push(Queue* queue, TreeNode* tree_node, int level) {
    QueueNode* new_node = new QueueNode;
    new_node->tree_node = tree_node;
    new_node->level = level;
    new_node->next = nullptr;
    
    if (queue->rear == nullptr) {
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->size++;
}

QueueNode* queue_pop(Queue* queue) {
    if (queue->front == nullptr) {
        return nullptr;
    }
    
    QueueNode* temp = queue->front;
    queue->front = queue->front->next;
    
    if (queue->front == nullptr) {
        queue->rear = nullptr;
    }
    
    queue->size--;
    return temp;
}

bool queue_is_empty(const Queue* queue) {
    return queue->front == nullptr;
}

void queue_free(Queue* queue) {
    while (!queue_is_empty(queue)) {
        QueueNode* node = queue_pop(queue);
        delete node;
    }
    delete queue;
}

// Функции для работы с деревом
BinaryTree* create_binary_tree() {
    BinaryTree* tree = new BinaryTree;
    tree->root = nullptr;
    tree->size = 0;
    return tree;
}

// Вставка элемента в бинарное дерево поиска
TreeNode* insert_helper(TreeNode* node, int key, const string& value, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        TreeNode* new_node = new TreeNode;
        new_node->key = key;
        new_node->value = value;
        new_node->left = nullptr;
        new_node->right = nullptr;
        return new_node;
    }
    
    if (key < node->key) {
        node->left = insert_helper(node->left, key, value, inserted);
    } else if (key > node->key) {
        node->right = insert_helper(node->right, key, value, inserted);
    }
    // Если ключ уже существует, не вставляем
    
    return node;
}

bool tree_insert(BinaryTree* tree, int key, const string& value) {
    bool inserted = false;
    tree->root = insert_helper(tree->root, key, value, inserted);
    if (inserted) {
        tree->size++;
    }
    return inserted;
}

// Структура для хранения уровней дерева
struct LevelList {
    TreeNode** nodes;      // Массив узлов уровня
    int size;              // Количество узлов в уровне
    int capacity;          // Вместимость массива
};

// Создание списка уровня
LevelList* create_level_list(int initial_capacity = 10) {
    LevelList* list = new LevelList;
    list->nodes = new TreeNode*[initial_capacity];
    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

// Добавление узла в список уровня
void level_list_add(LevelList* list, TreeNode* node) {
    if (list->size >= list->capacity) {
        // Увеличиваем вместимость
        int new_capacity = list->capacity * 2;
        TreeNode** new_nodes = new TreeNode*[new_capacity];
        
        for (int i = 0; i < list->size; i++) {
            new_nodes[i] = list->nodes[i];
        }
        
        delete[] list->nodes;
        list->nodes = new_nodes;
        list->capacity = new_capacity;
    }
    
    list->nodes[list->size] = node;
    list->size++;
}

// Освобождение памяти списка уровня
void free_level_list(LevelList* list) {
    delete[] list->nodes;
    delete list;
}

// Функция для обхода дерева "змейкой" с использованием очереди
void snake_traversal(const BinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    
    Queue* queue = create_queue();
    queue_push(queue, tree->root, 0);
    
    // Массив для хранения списков уровней
    LevelList** levels = new LevelList*[100]; // Максимум 100 уровней
    int max_level = 0;
    
    // Инициализация массивов уровней
    for (int i = 0; i < 100; i++) {
        levels[i] = create_level_list();
    }
    
    // Обход в ширину для сбора узлов по уровням
    while (!queue_is_empty(queue)) {
        QueueNode* qnode = queue_pop(queue);
        TreeNode* node = qnode->tree_node;
        int level = qnode->level;
        
        // Добавляем узел в соответствующий уровень
        level_list_add(levels[level], node);
        
        if (level > max_level) {
            max_level = level;
        }
        
        // Добавляем потомков в очередь
        if (node->left != nullptr) {
            queue_push(queue, node->left, level + 1);
        }
        if (node->right != nullptr) {
            queue_push(queue, node->right, level + 1);
        }
        
        delete qnode;
    }
    
    queue_free(queue);
    
    // Вывод узлов змейкой
    cout << "Обход дерева змейкой: ";
    bool first = true;
    
    for (int level = 0; level <= max_level; level++) {
        LevelList* current_level = levels[level];
        
        if (level % 2 == 0) {
            // Четные уровни: слева направо
            for (int i = 0; i < current_level->size; i++) {
                if (!first) cout << " - ";
                cout << current_level->nodes[i]->key << ":" << current_level->nodes[i]->value;
                first = false;
            }
        } else {
            // Нечетные уровни: справа налево
            for (int i = current_level->size - 1; i >= 0; i--) {
                if (!first) cout << " - ";
                cout << current_level->nodes[i]->key << ":" << current_level->nodes[i]->value;
                first = false;
            }
        }
    }
    cout << endl;
    
    // Освобождение памяти
    for (int i = 0; i <= max_level; i++) {
        free_level_list(levels[i]);
    }
    delete[] levels;
}

// Вспомогательная функция для рекурсивного удаления дерева
void free_tree_helper(TreeNode* node) {
    if (node == nullptr) {
        return;
    }
    
    free_tree_helper(node->left);
    free_tree_helper(node->right);
    delete node;
}

void tree_free(BinaryTree* tree) {
    free_tree_helper(tree->root);
    delete tree;
}

// Функция для визуального вывода дерева
void print_tree_helper(const TreeNode* root, int space) {
    if (root == nullptr) {
        return;
    }
    
    // Увеличиваем отступ для каждого уровня
    space += 5;
    
    // Сначала правый потомок
    print_tree_helper(root->right, space);
    
    // Текущий узел
    cout << endl;
    for (int i = 5; i < space; i++) {
        cout << " ";
    }
    cout << root->key << ":" << root->value << endl;
    
    // Левый потомок
    print_tree_helper(root->left, space);
}

void print_tree(const BinaryTree* tree) {
    if (tree->root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    
    cout << "Структура дерева:" << endl;
    print_tree_helper(tree->root, 0);
    cout << endl;
}

int main() {
    BinaryTree* tree = create_binary_tree();
    int n, key;
    string value;
    
    cout << "=== Обход бинарного дерева змейкой ===" << endl;
    cout << "Введите количество узлов в дереве: ";
    cin >> n;
    
    cout << "Введите " << n << " пар (ключ значение):" << endl;
    for (int i = 0; i < n; i++) {
        cin >> key >> value;
        if (!tree_insert(tree, key, value)) {
            cout << "Ключ " << key << " уже существует, пропускаем" << endl;
        }
    }
    
    cout << endl;
    print_tree(tree);
    snake_traversal(tree);
    
    tree_free(tree);
    return 0;
}