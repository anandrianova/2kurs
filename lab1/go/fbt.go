package main

import (
	"fmt"
)

// Узел бинарного дерева
type TreeNode struct {
	key   int
	value string
	left  *TreeNode
	right *TreeNode
}

// Структура полного бинарного дерева
type FullBinaryTree struct {
	root *TreeNode
	size int
}

// Создание нового полного бинарного дерева
func createFullBinaryTree() *FullBinaryTree {
	return &FullBinaryTree{
		root: nil,
		size: 0,
	}
}

// Вспомогательная функция для вставки в полное бинарное дерево
func treeInsertHelper(root **TreeNode, key int, value string) bool {
	if *root == nil {
		*root = &TreeNode{
			key:   key,
			value: value,
			left:  nil,
			right: nil,
		}
		return true
	}

	// Для полного бинарного дерева вставляем слева направо
	// Используем очередь для обхода в ширину
	queue := []**TreeNode{root}

	for len(queue) > 0 {
		current := queue[0]
		queue = queue[1:]

		if (*current).left == nil {
			(*current).left = &TreeNode{
				key:   key,
				value: value,
				left:  nil,
				right: nil,
			}
			return true
		} else if (*current).right == nil {
			(*current).right = &TreeNode{
				key:   key,
				value: value,
				left:  nil,
				right: nil,
			}
			return true
		} else {
			queue = append(queue, &(*current).left)
			queue = append(queue, &(*current).right)
		}
	}

	return false
}

// Вставка элемента в дерево
func treeInsert(tree *FullBinaryTree, key int, value string) bool {
	if treeInsertHelper(&tree.root, key, value) {
		tree.size++
		return true
	}
	return false
}

// Поиск элемента в дереве по ключу
func treeSearchHelper(root *TreeNode, key int) string {
	if root == nil {
		return ""
	}

	if root.key == key {
		return root.value
	}

	// Поиск в левом поддереве
	leftResult := treeSearchHelper(root.left, key)
	if leftResult != "" {
		return leftResult
	}

	// Поиск в правом поддереве
	return treeSearchHelper(root.right, key)
}

func treeSearch(tree *FullBinaryTree, key int) string {
	return treeSearchHelper(tree.root, key)
}

// Проверка, является ли дерево полным бинарным деревом
func isFullBinaryTreeHelper(root *TreeNode) bool {
	if root == nil {
		return true
	}

	// Если у узла нет детей
	if root.left == nil && root.right == nil {
		return true
	}

	// Если у узла оба ребенка
	if root.left != nil && root.right != nil {
		return isFullBinaryTreeHelper(root.left) && isFullBinaryTreeHelper(root.right)
	}

	// Если у узла только один ребенок - не полное дерево
	return false
}

func isFullBinaryTree(tree *FullBinaryTree) bool {
	return isFullBinaryTreeHelper(tree.root)
}

// Высота дерева
func treeHeight(root *TreeNode) int {
	if root == nil {
		return 0
	}

	leftHeight := treeHeight(root.left)
	rightHeight := treeHeight(root.right)

	if leftHeight > rightHeight {
		return leftHeight + 1
	}
	return rightHeight + 1
}

// Размер дерева
func treeSize(tree *FullBinaryTree) int {
	return tree.size
}

// Обход в прямом порядке (pre-order)
func treePreorderHelper(root *TreeNode) {
	if root == nil {
		return
	}

	fmt.Printf("(%d: %s) ", root.key, root.value)
	treePreorderHelper(root.left)
	treePreorderHelper(root.right)
}

func treePreorder(tree *FullBinaryTree) {
	fmt.Print("PREORDER: ")
	treePreorderHelper(tree.root)
	fmt.Println()
}

// Обход в симметричном порядке (in-order)
func treeInorderHelper(root *TreeNode) {
	if root == nil {
		return
	}

	treeInorderHelper(root.left)
	fmt.Printf("(%d: %s) ", root.key, root.value)
	treeInorderHelper(root.right)
}

func treeInorder(tree *FullBinaryTree) {
	fmt.Print("INORDER: ")
	treeInorderHelper(tree.root)
	fmt.Println()
}

// Обход в обратном порядке (post-order)
func treePostorderHelper(root *TreeNode) {
	if root == nil {
		return
	}

	treePostorderHelper(root.left)
	treePostorderHelper(root.right)
	fmt.Printf("(%d: %s) ", root.key, root.value)
}

func treePostorder(tree *FullBinaryTree) {
	fmt.Print("POSTORDER: ")
	treePostorderHelper(tree.root)
	fmt.Println()
}

// Обход в ширину (level-order)
func treeLevelOrder(tree *FullBinaryTree) {
	if tree.root == nil {
		return
	}

	fmt.Print("LEVEL ORDER: ")
	queue := []*TreeNode{tree.root}

	for len(queue) > 0 {
		current := queue[0]
		queue = queue[1:]

		fmt.Printf("(%d: %s) ", current.key, current.value)

		if current.left != nil {
			queue = append(queue, current.left)
		}
		if current.right != nil {
			queue = append(queue, current.right)
		}
	}
	fmt.Println()
}

// Вывод дерева
func treePrintHelper(root *TreeNode, level int) {
	if root == nil {
		return
	}

	treePrintHelper(root.right, level+1)

	for i := 0; i < level; i++ {
		fmt.Print("    ")
	}
	fmt.Printf("(%d: %s)\n", root.key, root.value)

	treePrintHelper(root.left, level+1)
}

func treePrint(tree *FullBinaryTree) {
	fmt.Println("TREE:")
	treePrintHelper(tree.root, 0)
}

// Освобождение памяти
func treeFreeHelper(root *TreeNode) {
	if root == nil {
		return
	}

	treeFreeHelper(root.left)
	treeFreeHelper(root.right)
	root.left = nil
	root.right = nil
}

func treeFree(tree *FullBinaryTree) {
	treeFreeHelper(tree.root)
	tree.root = nil
	tree.size = 0
}
