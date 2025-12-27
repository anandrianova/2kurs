package main

import (
	"encoding/json"
	"fmt"
	"strings"
)

// Узел бинарного дерева
type TreeNode struct {
	Value string
	Left  *TreeNode
	Right *TreeNode
}

// Бинарное дерево
type BinaryTree struct {
	Root *TreeNode
}

// Создание нового бинарного дерева
func NewBinaryTree() *BinaryTree {
	return &BinaryTree{}
}

// Вставка значения в дерево
func (bt *BinaryTree) Insert(value string) {
	if value == "" {
		return // Не вставляем пустые строки
	}
	
	if bt.Root == nil {
		bt.Root = &TreeNode{Value: value}
		return
	}
	
	// Проверяем, есть ли уже такое значение
	if bt.Search(value) {
		return // Не вставляем дубликаты
	}
	
	bt.insertRecursive(bt.Root, value)
}

func (bt *BinaryTree) insertRecursive(node *TreeNode, value string) *TreeNode {
	if node == nil {
		return &TreeNode{Value: value}
	}

	if value < node.Value {
		node.Left = bt.insertRecursive(node.Left, value)
	} else {
		node.Right = bt.insertRecursive(node.Right, value)
	}
	return node
}

// Поиск значения в дереве
func (bt *BinaryTree) Search(value string) bool {
	if value == "" {
		return false // Пустые строки не ищем
	}
	return bt.searchRecursive(bt.Root, value)
}

func (bt *BinaryTree) searchRecursive(node *TreeNode, value string) bool {
	if node == nil {
		return false
	}
	if node.Value == value {
		return true
	}
	if value < node.Value {
		return bt.searchRecursive(node.Left, value)
	}
	return bt.searchRecursive(node.Right, value)
}

// Удаление значения из дерева
func (bt *BinaryTree) Delete(value string) {
	if value == "" {
		return // Не удаляем пустые строки
	}
	bt.Root = bt.deleteRecursive(bt.Root, value)
}

func (bt *BinaryTree) deleteRecursive(node *TreeNode, value string) *TreeNode {
	if node == nil {
		return nil
	}

	if value < node.Value {
		node.Left = bt.deleteRecursive(node.Left, value)
	} else if value > node.Value {
		node.Right = bt.deleteRecursive(node.Right, value)
	} else {
		// Узел найден
		if node.Left == nil {
			return node.Right
		} else if node.Right == nil {
			return node.Left
		}

		// Узел с двумя детьми
		minNode := bt.findMin(node.Right)
		node.Value = minNode.Value
		node.Right = bt.deleteRecursive(node.Right, minNode.Value)
	}
	return node
}

func (bt *BinaryTree) findMin(node *TreeNode) *TreeNode {
	current := node
	for current.Left != nil {
		current = current.Left
	}
	return current
}

// Обход дерева в порядке (in-order)
func (bt *BinaryTree) InOrder() []string {
	var result []string
	bt.inOrderRecursive(bt.Root, &result)
	return result
}

func (bt *BinaryTree) inOrderRecursive(node *TreeNode, result *[]string) {
	if node != nil {
		bt.inOrderRecursive(node.Left, result)
		*result = append(*result, node.Value)
		bt.inOrderRecursive(node.Right, result)
	}
}

// PreOrder обход (корень-левый-правый)
func (bt *BinaryTree) PreOrder() []string {
	var result []string
	bt.preOrderRecursive(bt.Root, &result)
	return result
}

func (bt *BinaryTree) preOrderRecursive(node *TreeNode, result *[]string) {
	if node != nil {
		*result = append(*result, node.Value)
		bt.preOrderRecursive(node.Left, result)
		bt.preOrderRecursive(node.Right, result)
	}
}

// PostOrder обход (левый-правый-корень)
func (bt *BinaryTree) PostOrder() []string {
	var result []string
	bt.postOrderRecursive(bt.Root, &result)
	return result
}

func (bt *BinaryTree) postOrderRecursive(node *TreeNode, result *[]string) {
	if node != nil {
		bt.postOrderRecursive(node.Left, result)
		bt.postOrderRecursive(node.Right, result)
		*result = append(*result, node.Value)
	}
}

// LevelOrder обход (по уровням)
func (bt *BinaryTree) LevelOrder() []string {
	if bt.Root == nil {
		return []string{}
	}

	var result []string
	queue := []*TreeNode{bt.Root}

	for len(queue) > 0 {
		node := queue[0]
		queue = queue[1:]
		
		result = append(result, node.Value)
		
		if node.Left != nil {
			queue = append(queue, node.Left)
		}
		if node.Right != nil {
			queue = append(queue, node.Right)
		}
	}
	return result
}

// Height возвращает высоту дерева
func (bt *BinaryTree) Height() int {
	return bt.heightRecursive(bt.Root)
}

func (bt *BinaryTree) heightRecursive(node *TreeNode) int {
	if node == nil {
		return 0
	}
	leftHeight := bt.heightRecursive(node.Left)
	rightHeight := bt.heightRecursive(node.Right)
	
	if leftHeight > rightHeight {
		return leftHeight + 1
	}
	return rightHeight + 1
}

// GetMin возвращает минимальное значение
func (bt *BinaryTree) GetMin() (string, bool) {
	if bt.Root == nil {
		return "", false
	}
	current := bt.Root
	for current.Left != nil {
		current = current.Left
	}
	return current.Value, true
}

// GetMax возвращает максимальное значение
func (bt *BinaryTree) GetMax() (string, bool) {
	if bt.Root == nil {
		return "", false
	}
	current := bt.Root
	for current.Right != nil {
		current = current.Right
	}
	return current.Value, true
}

// GetRoot возвращает значение корня
func (bt *BinaryTree) GetRoot() (string, bool) {
	if bt.Root == nil {
		return "", false
	}
	return bt.Root.Value, true
}

// Представление дерева в виде строки
func (bt *BinaryTree) String() string {
	var builder strings.Builder
	bt.stringify(bt.Root, 0, &builder)
	return builder.String()
}

func (bt *BinaryTree) stringify(node *TreeNode, depth int, builder *strings.Builder) {
	if node == nil {
		return
	}

	bt.stringify(node.Right, depth+1, builder)
	builder.WriteString(strings.Repeat("  ", depth))
	builder.WriteString(fmt.Sprintf("%s\n", node.Value))
	bt.stringify(node.Left, depth+1, builder)
}

// Методы для интерфейса Container

func (bt *BinaryTree) Length() int {
	return bt.countNodes(bt.Root)
}

func (bt *BinaryTree) countNodes(node *TreeNode) int {
	if node == nil {
		return 0
	}
	return 1 + bt.countNodes(node.Left) + bt.countNodes(node.Right)
}

func (bt *BinaryTree) Size() int {
	return bt.Length()
}

func (bt *BinaryTree) IsEmpty() bool {
	return bt.Root == nil
}

func (bt *BinaryTree) Clear() {
	bt.Root = nil
}

func (bt *BinaryTree) Print() {
	if bt.Root == nil {
		fmt.Println("Empty tree")
		return
	}
	fmt.Println(bt.String())
}

// Remove удаляет значение из дерева (нужен для main.go)
func (bt *BinaryTree) Remove(value string) bool {
	if value == "" {
		return false // Не удаляем пустые строки
	}
	if !bt.Search(value) {
		return false
	}
	bt.Delete(value)
	return true
}

// Values возвращает все значения (нужен для main.go)
func (bt *BinaryTree) Values() []string {
	return bt.InOrder()
}

// Методы для интерфейса Serializer

// Serialize сериализует дерево в JSON
func (bt *BinaryTree) Serialize() ([]byte, error) {
	type SerializableNode struct {
		Value string            `json:"value"`
		Left  *SerializableNode `json:"left,omitempty"`
		Right *SerializableNode `json:"right,omitempty"`
	}

	var convert func(*TreeNode) *SerializableNode
	convert = func(node *TreeNode) *SerializableNode {
		if node == nil {
			return nil
		}
		return &SerializableNode{
			Value: node.Value,
			Left:  convert(node.Left),
			Right: convert(node.Right),
		}
	}

	root := convert(bt.Root)
	return json.Marshal(root)
}

// Deserialize десериализует дерево из JSON
func (bt *BinaryTree) Deserialize(data []byte) error {
	type SerializableNode struct {
		Value string            `json:"value"`
		Left  *SerializableNode `json:"left,omitempty"`
		Right *SerializableNode `json:"right,omitempty"`
	}

	var root *SerializableNode
	if err := json.Unmarshal(data, &root); err != nil {
		return err
	}

	var convert func(*SerializableNode) *TreeNode
	convert = func(node *SerializableNode) *TreeNode {
		if node == nil {
			return nil
		}
		return &TreeNode{
			Value: node.Value,
			Left:  convert(node.Left),
			Right: convert(node.Right),
		}
	}

	bt.Root = convert(root)
	return nil
}

// Методы для интерфейса BinarySerializer

// SerializeBinary сериализует дерево в бинарный формат
func (bt *BinaryTree) SerializeBinary() ([]byte, error) {
	// Используем существующую функцию serializeToBinary из containers.go
	type BinaryNode struct {
		Value string
		Left  *BinaryNode
		Right *BinaryNode
	}

	var convert func(*TreeNode) *BinaryNode
	convert = func(node *TreeNode) *BinaryNode {
		if node == nil {
			return nil
		}
		return &BinaryNode{
			Value: node.Value,
			Left:  convert(node.Left),
			Right: convert(node.Right),
		}
	}

	root := convert(bt.Root)
	return serializeToBinary(root)
}

// DeserializeBinary десериализует дерево из бинарного формата
func (bt *BinaryTree) DeserializeBinary(data []byte) error {
	// Используем существующую функцию deserializeFromBinary из containers.go
	type BinaryNode struct {
		Value string
		Left  *BinaryNode
		Right *BinaryNode
	}

	var root *BinaryNode
	if err := deserializeFromBinary(data, &root); err != nil {
		return err
	}

	var convert func(*BinaryNode) *TreeNode
	convert = func(node *BinaryNode) *TreeNode {
		if node == nil {
			return nil
		}
		return &TreeNode{
			Value: node.Value,
			Left:  convert(node.Left),
			Right: convert(node.Right),
		}
	}

	bt.Root = convert(root)
	return nil
}
