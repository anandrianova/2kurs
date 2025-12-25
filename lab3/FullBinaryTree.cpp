#include "FullBinaryTree.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;

FullBinaryTree::FullBinaryTree() {
    root = nullptr;
    size = 0;
}

FullBinaryTree::~FullBinaryTree() {
    free_tree_helper(root);
}

FullBinaryTree::FullBinaryTree(const FullBinaryTree& other) {
    root = copy_tree_helper(other.root);
    size = other.size;
}

FullBinaryTree& FullBinaryTree::operator=(const FullBinaryTree& other) {
    if (this != &other) {
        free_tree_helper(root);
        root = copy_tree_helper(other.root);
        size = other.size;
    }
    return *this;
}

TreeNode* FullBinaryTree::copy_tree_helper(const TreeNode* node) {
    if (node == nullptr) {
        return nullptr;
    }

    TreeNode* new_node = new TreeNode(node->key, node->value);
    new_node->left = copy_tree_helper(node->left);
    new_node->right = copy_tree_helper(node->right);
    return new_node;
}

bool FullBinaryTree::insert(int key, const string& value) {
    if (!search(key).empty()) {
        return false;
    }

    if (root == nullptr) {
        root = new TreeNode(key, value);
        size++;
        return true;
    }

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();

        if (current->left == nullptr) {
            current->left = new TreeNode(key, value);
            size++;
            return true;
        } else if (current->right == nullptr) {
            current->right = new TreeNode(key, value);
            size++;
            return true;
        } else {
            q.push(current->left);
            q.push(current->right);
        }
    }
    return false;
}

string FullBinaryTree::search(int key) const {
    return search_helper(root, key);
}

string FullBinaryTree::search_helper(const TreeNode* node, int key) const {
    if (node == nullptr) {
        return "";
    }

    if (node->key == key) {
        return node->value;
    }

    string left_result = search_helper(node->left, key);
    if (!left_result.empty()) {
        return left_result;
    }

    return search_helper(node->right, key);
}

bool FullBinaryTree::is_full() const {
    return is_full_binary_tree_helper(root);
}

bool FullBinaryTree::is_full_binary_tree_helper(const TreeNode* node) const {
    if (node == nullptr) {
        return true;
    }

    if (node->left == nullptr && node->right == nullptr) {
        return true;
    }

    if (node->left != nullptr && node->right != nullptr) {
        return is_full_binary_tree_helper(node->left) && is_full_binary_tree_helper(node->right);
    }

    return false;
}

int FullBinaryTree::height() const {
    return tree_height_helper(root);
}

int FullBinaryTree::tree_height_helper(const TreeNode* node) const {
    if (node == nullptr) {
        return 0;
    }

    int left_height = tree_height_helper(node->left);
    int right_height = tree_height_helper(node->right);

    return max(left_height, right_height) + 1;
}

int FullBinaryTree::get_size() const {
    return size;
}

void FullBinaryTree::print() const {
    if (root == nullptr) {
        cout << "Tree is empty" << endl;
        return;
    }

    cout << "Full Binary Tree structure:" << endl;
    print_tree_helper(root, 0);
    cout << endl;

    bool is_full_tree = is_full();
    cout << "Is full binary tree: " << (is_full_tree ? "YES" : "NO") << endl;
    cout << "Tree size: " << size << endl;
    cout << "Tree height: " << height() << endl;
}

void FullBinaryTree::print_tree_helper(const TreeNode* root, int space) const {
    const int COUNT = 5;

    if (root == nullptr) {
        return;
    }

    space += COUNT;

    print_tree_helper(root->right, space);

    cout << endl;
    for (int i = COUNT; i < space; i++) {
        cout << " ";
    }
    cout << root->key << ":" << root->value << endl;

    print_tree_helper(root->left, space);
}

void FullBinaryTree::inorder() const {
    if (root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    cout << "Inorder: ";
    inorder_helper(root);
    cout << endl;
}

void FullBinaryTree::inorder_helper(const TreeNode* node) const {
    if (node == nullptr)
        return;

    inorder_helper(node->left);
    cout << node->key << ":" << node->value << " ";
    inorder_helper(node->right);
}

void FullBinaryTree::preorder() const {
    if (root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    cout << "Preorder: ";
    preorder_helper(root);
    cout << endl;
}

void FullBinaryTree::preorder_helper(const TreeNode* node) const {
    if (node == nullptr)
        return;

    cout << node->key << ":" << node->value << " ";
    preorder_helper(node->left);
    preorder_helper(node->right);
}

void FullBinaryTree::postorder() const {
    if (root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }
    cout << "Postorder: ";
    postorder_helper(root);
    cout << endl;
}

void FullBinaryTree::postorder_helper(const TreeNode* node) const {
    if (node == nullptr)
        return;

    postorder_helper(node->left);
    postorder_helper(node->right);
    cout << node->key << ":" << node->value << " ";
}

void FullBinaryTree::level_order() const {
    if (root == nullptr) {
        cout << "Дерево пусто" << endl;
        return;
    }

    cout << "Level order: ";
    queue<const TreeNode*> q;
    q.push(root);

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

void FullBinaryTree::free_tree_helper(TreeNode* node) {
    if (node == nullptr) {
        return;
    }

    free_tree_helper(node->left);
    free_tree_helper(node->right);
    delete node;
}

void FullBinaryTree::serialize_binary_helper(ofstream& file, const TreeNode* node) const {
    if (node == nullptr) {
        int marker = -1;
        file.write(reinterpret_cast<const char*>(&marker), sizeof(marker));
        return;
    }

    file.write(reinterpret_cast<const char*>(&node->key), sizeof(node->key));

    size_t str_size = node->value.size();
    file.write(reinterpret_cast<const char*>(&str_size), sizeof(str_size));
    file.write(node->value.c_str(), str_size);

    serialize_binary_helper(file, node->left);
    serialize_binary_helper(file, node->right);
}

TreeNode* FullBinaryTree::deserialize_binary_helper(ifstream& file) {
    int key;
    file.read(reinterpret_cast<char*>(&key), sizeof(key));

    if (key == -1) {
        return nullptr;
    }

    size_t str_size;
    file.read(reinterpret_cast<char*>(&str_size), sizeof(str_size));

    string value(str_size, '\0');
    file.read(&value[0], str_size);

    TreeNode* node = new TreeNode(key, value);
    node->left = deserialize_binary_helper(file);
    node->right = deserialize_binary_helper(file);

    return node;
}

bool FullBinaryTree::serialize_binary(const string& filename) const {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    serialize_binary_helper(file, root);

    return true;
}

bool FullBinaryTree::deserialize_binary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    free_tree_helper(root);
    root = nullptr;

    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    root = deserialize_binary_helper(file);

    return true;
}

void FullBinaryTree::serialize_text_helper(ofstream& file, const TreeNode* node) const {
    if (node == nullptr) {
        file << "#" << endl;
        return;
    }

    file << node->key << " " << node->value << endl;
    serialize_text_helper(file, node->left);
    serialize_text_helper(file, node->right);
}

TreeNode* FullBinaryTree::deserialize_text_helper(ifstream& file) {
    string line;
    getline(file, line);

    if (line == "#") {
        return nullptr;
    }

    size_t space_pos = line.find(' ');
    if (space_pos == string::npos) {
        return nullptr;
    }

    int key = stoi(line.substr(0, space_pos));
    string value = line.substr(space_pos + 1);

    TreeNode* node = new TreeNode(key, value);
    node->left = deserialize_text_helper(file);
    node->right = deserialize_text_helper(file);

    return node;
}

bool FullBinaryTree::serialize_text(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << size << endl;
    serialize_text_helper(file, root);

    return true;
}

bool FullBinaryTree::deserialize_text(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    free_tree_helper(root);
    root = nullptr;

    string size_str;
    getline(file, size_str);
    size = stoi(size_str);

    root = deserialize_text_helper(file);

    return true;
}