package main

import (
	"fmt"
)

// Узел двусвязного списка
type DoubleListNode struct {
	data string
	next *DoubleListNode
	prev *DoubleListNode
}

// Структура двусвязного списка
type DoubleList struct {
	head *DoubleListNode
	tail *DoubleListNode
	size int
}

// Создание нового двусвязного списка
func createDoublyLinkedList() *DoubleList {
	return &DoubleList{
		head: nil,
		tail: nil,
		size: 0,
	}
}

// Добавление в начало
func dllPushFront(list *DoubleList, value string) {
	newNode := &DoubleListNode{
		data: value,
		next: list.head,
		prev: nil,
	}

	if list.head != nil {
		list.head.prev = newNode
	}
	list.head = newNode

	if list.tail == nil {
		list.tail = newNode
	}
	list.size++
}

// Добавление в конец
func dllPushBack(list *DoubleList, value string) {
	newNode := &DoubleListNode{
		data: value,
		next: nil,
		prev: list.tail,
	}

	if list.tail != nil {
		list.tail.next = newNode
	}
	list.tail = newNode

	if list.head == nil {
		list.head = newNode
	}
	list.size++
}

// Вставка перед заданным значением
func dllInsertBefore(list *DoubleList, target string, value string) bool {
	current := list.head
	for current != nil && current.data != target {
		current = current.next
	}

	if current == nil {
		return false
	}

	newNode := &DoubleListNode{
		data: value,
		next: current,
		prev: current.prev,
	}

	if current.prev != nil {
		current.prev.next = newNode
	} else {
		list.head = newNode
	}
	current.prev = newNode
	list.size++
	return true
}

// Вставка после заданного значения
func dllInsertAfter(list *DoubleList, target string, value string) bool {
	current := list.head
	for current != nil && current.data != target {
		current = current.next
	}

	if current == nil {
		return false
	}

	newNode := &DoubleListNode{
		data: value,
		next: current.next,
		prev: current,
	}

	if current.next != nil {
		current.next.prev = newNode
	} else {
		list.tail = newNode
	}
	current.next = newNode
	list.size++
	return true
}

// Удаление из начала
func dllPopFront(list *DoubleList) bool {
	if list.head == nil {
		return false
	}

	if list.head == list.tail {
		list.head = nil
		list.tail = nil
	} else {
		list.head = list.head.next
		list.head.prev = nil
	}
	list.size--
	return true
}

// Удаление из конца
func dllPopBack(list *DoubleList) bool {
	if list.tail == nil {
		return false
	}

	if list.head == list.tail {
		list.head = nil
		list.tail = nil
	} else {
		list.tail = list.tail.prev
		list.tail.next = nil
	}
	list.size--
	return true
}

// Удаление по значению
func dllRemoveValue(list *DoubleList, value string) bool {
	current := list.head
	for current != nil && current.data != value {
		current = current.next
	}

	if current == nil {
		return false
	}

	if current.prev != nil {
		current.prev.next = current.next
	} else {
		list.head = current.next
	}

	if current.next != nil {
		current.next.prev = current.prev
	} else {
		list.tail = current.prev
	}
	list.size--
	return true
}

// Удаление перед заданным значением
func dllRemoveBefore(list *DoubleList, target string) bool {
	current := list.head
	for current != nil && current.data != target {
		current = current.next
	}

	if current == nil || current.prev == nil {
		return false
	}

	return dllRemoveValue(list, current.prev.data)
}

// Удаление после заданного значения
func dllRemoveAfter(list *DoubleList, target string) bool {
	current := list.head
	for current != nil && current.data != target {
		current = current.next
	}

	if current == nil || current.next == nil {
		return false
	}

	return dllRemoveValue(list, current.next.data)
}

// Поиск элемента
func dllFind(list *DoubleList, value string) *DoubleListNode {
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
func dllSize(list *DoubleList) int {
	return list.size
}

// Вывод списка вперед
func dllPrintForward(list *DoubleList) {
	fmt.Print("DOUBLY LIST FORWARD: ")
	current := list.head
	for current != nil {
		fmt.Print(current.data)
		if current.next != nil {
			fmt.Print(" <-> ")
		}
		current = current.next
	}
	fmt.Println()
}

// Вывод списка назад
func dllPrintBackward(list *DoubleList) {
	fmt.Print("DOUBLY LIST BACKWARD: ")
	current := list.tail
	for current != nil {
		fmt.Print(current.data)
		if current.prev != nil {
			fmt.Print(" <-> ")
		}
		current = current.prev
	}
	fmt.Println()
}

// Освобождение памяти
func dllFree(list *DoubleList) {
	current := list.head
	for current != nil {
		next := current.next
		current.next = nil
		current.prev = nil
		current = next
	}
	list.head = nil
	list.tail = nil
	list.size = 0
}
