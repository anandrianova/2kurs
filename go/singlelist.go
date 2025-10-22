package main

import (
	"fmt"
)

// Узел односвязного списка
type SingleListNode struct {
	data string
	next *SingleListNode
}

// Структура односвязного списка
type SingleList struct {
	head *SingleListNode
	tail *SingleListNode
	size int
}

// Создание нового односвязного списка
func createSinglyLinkedList() *SingleList {
	return &SingleList{
		head: nil,
		tail: nil,
		size: 0,
	}
}

// Добавление в начало
func sllPushFront(list *SingleList, value string) {
	newNode := &SingleListNode{
		data: value,
		next: list.head,
	}

	list.head = newNode
	if list.tail == nil {
		list.tail = newNode
	}
	list.size++
}

// Добавление в конец
func sllPushBack(list *SingleList, value string) {
	newNode := &SingleListNode{
		data: value,
		next: nil,
	}

	if list.tail == nil {
		list.head = newNode
		list.tail = newNode
	} else {
		list.tail.next = newNode
		list.tail = newNode
	}
	list.size++
}

// Вставка перед заданным значением
func sllInsertBefore(list *SingleList, target string, value string) bool {
	if list.head == nil {
		return false
	}

	// Если вставляем перед головой
	if list.head.data == target {
		sllPushFront(list, value)
		return true
	}

	current := list.head
	for current.next != nil && current.next.data != target {
		current = current.next
	}

	if current.next == nil {
		return false
	}

	newNode := &SingleListNode{
		data: value,
		next: current.next,
	}
	current.next = newNode
	list.size++
	return true
}

// Вставка после заданного значения
func sllInsertAfter(list *SingleList, target string, value string) bool {
	current := list.head
	for current != nil && current.data != target {
		current = current.next
	}

	if current == nil {
		return false
	}

	newNode := &SingleListNode{
		data: value,
		next: current.next,
	}
	current.next = newNode

	if current == list.tail {
		list.tail = newNode
	}
	list.size++
	return true
}

// Удаление из начала
func sllPopFront(list *SingleList) bool {
	if list.head == nil {
		return false
	}

	list.head = list.head.next
	if list.head == nil {
		list.tail = nil
	}
	list.size--
	return true
}

// Удаление из конца
func sllPopBack(list *SingleList) bool {
	if list.head == nil {
		return false
	}

	if list.head == list.tail {
		list.head = nil
		list.tail = nil
		list.size = 0
		return true
	}

	current := list.head
	for current.next != list.tail {
		current = current.next
	}

	current.next = nil
	list.tail = current
	list.size--
	return true
}

// Удаление по значению
func sllRemoveValue(list *SingleList, value string) bool {
	if list.head == nil {
		return false
	}

	// Если удаляем голову
	if list.head.data == value {
		return sllPopFront(list)
	}

	current := list.head
	for current.next != nil && current.next.data != value {
		current = current.next
	}

	if current.next == nil {
		return false
	}

	current.next = current.next.next

	// Если удалили хвост
	if current.next == nil {
		list.tail = current
	}
	list.size--
	return true
}

// Удаление перед заданным значением
func sllRemoveBefore(list *SingleList, target string) bool {
	if list.head == nil || list.head.data == target {
		return false
	}

	// Если удаляем перед головой
	if list.head.next != nil && list.head.next.data == target {
		return sllPopFront(list)
	}

	current := list.head
	for current.next != nil && current.next.next != nil && current.next.next.data != target {
		current = current.next
	}

	if current.next == nil || current.next.next == nil {
		return false
	}

	current.next = current.next.next
	list.size--
	return true
}

// Удаление после заданного значения
func sllRemoveAfter(list *SingleList, target string) bool {
	current := list.head
	for current != nil && current.data != target {
		current = current.next
	}

	if current == nil || current.next == nil {
		return false
	}

	current.next = current.next.next

	// Если удалили хвост
	if current.next == nil {
		list.tail = current
	}
	list.size--
	return true
}

// Поиск элемента
func sllFind(list *SingleList, value string) *SingleListNode {
	current := list.head
	for current != nil {
		if current.data == value {
			return current
		}
		current = current.next
	}
	return nil
}

// Получение размера
func sllSize(list *SingleList) int {
	return list.size
}

// Вывод списка вперед
func sllPrintForward(list *SingleList) {
	fmt.Print("SINGLY LIST FORWARD: ")
	current := list.head
	for current != nil {
		fmt.Print(current.data)
		if current.next != nil {
			fmt.Print(" -> ")
		}
		current = current.next
	}
	fmt.Println()
}

// Вывод списка назад (рекурсивно)
func sllPrintBackwardRecursive(node *SingleListNode) {
	if node == nil {
		return
	}
	sllPrintBackwardRecursive(node.next)
	fmt.Print(node.data)
	if node != nil { // Условие для проверки, что это не первый элемент
		fmt.Print(" <- ")
	}
}

func sllPrintBackward(list *SingleList) {
	fmt.Print("SINGLY LIST BACKWARD: ")
	sllPrintBackwardRecursive(list.head)
	fmt.Println()
}

// Освобождение памяти
func sllFree(list *SingleList) {
	current := list.head
	for current != nil {
		next := current.next
		current.next = nil
		current = next
	}
	list.head = nil
	list.tail = nil
	list.size = 0
}
