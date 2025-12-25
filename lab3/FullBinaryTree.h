#ifndef FULLBINARYTREE_H
#define FULLBINARYTREE_H

#include <fstream>
#include <queue>
#include <string>

using namespace std;

class TreeNode {
public:
    int key;
    string value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int k = 0, const string& v = "") : key(k), value(v), left(nullptr), right(nullptr) {}
};

class FullBinaryTree {
private:
    TreeNode* root;
    int size;

    string search_helper(const TreeNode* node, int key) const;
    bool is_full_binary_tree_helper(const TreeNode* node) const;
    int tree_height_helper(const TreeNode* node) const;
    void inorder_helper(const TreeNode* node) const;
    void preorder_helper(const TreeNode* node) const;
    void postorder_helper(const TreeNode* node) const;
    void print_tree_helper(const TreeNode* root, int space) const;
    void free_tree_helper(TreeNode* node);
    TreeNode* copy_tree_helper(const TreeNode* node);

    void serialize_binary_helper(ofstream& file, const TreeNode* node) const;
    TreeNode* deserialize_binary_helper(ifstream& file);
    void serialize_text_helper(ofstream& file, const TreeNode* node) const;
    TreeNode* deserialize_text_helper(ifstream& file);

public:
    FullBinaryTree();
    ~FullBinaryTree();
    FullBinaryTree(const FullBinaryTree& other);
    FullBinaryTree& operator=(const FullBinaryTree& other);

    bool insert(int key, const string& value);
    string search(int key) const;
    bool is_full() const;
    int height() const;
    int get_size() const;
    void print() const;
    void inorder() const;
    void preorder() const;
    void postorder() const;
    void level_order() const;

    bool serialize_binary(const string& filename) const;
    bool deserialize_binary(const string& filename);
    bool serialize_text(const string& filename) const;
    bool deserialize_text(const string& filename);
};

#endif