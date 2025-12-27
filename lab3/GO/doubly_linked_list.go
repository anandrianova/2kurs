package main

import (
	"encoding/json"
	"fmt"
)

type dllNode struct {
	value string
	prev  *dllNode
	next  *dllNode
}

// DoublyLinkedList представляет двусвязный список
type DoublyLinkedList struct {
	head *dllNode
	tail *dllNode
	size int
}

// NewDoublyLinkedList создает новый пустой двусвязный список
func NewDoublyLinkedList() *DoublyLinkedList {
	return &DoublyLinkedList{}
}

// PushFront добавляет элемент в начало списка
func (d *DoublyLinkedList) PushFront(value string) {
	newNode := &dllNode{value: value}

	if d.head == nil {
		d.head = newNode
		d.tail = newNode
	} else {
		newNode.next = d.head
		d.head.prev = newNode
		d.head = newNode
	}
	d.size++
}

// PushBack добавляет элемент в конец списка
func (d *DoublyLinkedList) PushBack(value string) {
	newNode := &dllNode{value: value}

	if d.tail == nil {
		d.head = newNode
		d.tail = newNode
	} else {
		newNode.prev = d.tail
		d.tail.next = newNode
		d.tail = newNode
	}
	d.size++
}

// PopFront удаляет и возвращает первый элемент
func (d *DoublyLinkedList) PopFront() (string, error) {
	if d.head == nil {
		return "", ErrEmptyContainer
	}

	value := d.head.value
	if d.head == d.tail {
		d.head = nil
		d.tail = nil
	} else {
		d.head = d.head.next
		d.head.prev = nil
	}
	d.size--
	return value, nil
}

// PopBack удаляет и возвращает последний элемент
func (d *DoublyLinkedList) PopBack() (string, error) {
	if d.tail == nil {
		return "", ErrEmptyContainer
	}

	value := d.tail.value
	if d.head == d.tail {
		d.head = nil
		d.tail = nil
	} else {
		d.tail = d.tail.prev
		d.tail.next = nil
	}
	d.size--
	return value, nil
}

// Insert вставляет элемент по указанному индексу
func (d *DoublyLinkedList) Insert(index int, value string) error {
	if index < 0 || index > d.size {
		return ErrIndexOutOfRange
	}

	if index == 0 {
		d.PushFront(value)
		return nil
	}

	if index == d.size {
		d.PushBack(value)
		return nil
	}

	var current *dllNode
	if index < d.size/2 {
		current = d.head
		for i := 0; i < index; i++ {
			current = current.next
		}
	} else {
		current = d.tail
		for i := d.size - 1; i > index; i-- {
			current = current.prev
		}
	}

	newNode := &dllNode{value: value, prev: current.prev, next: current}
	current.prev.next = newNode
	current.prev = newNode
	d.size++
	return nil
}

// Remove удаляет элемент по индексу
func (d *DoublyLinkedList) Remove(index int) error {
	if err := checkIndex(index, d.size); err != nil {
		return err
	}

	if index == 0 {
		_, err := d.PopFront()
		return err
	}

	if index == d.size-1 {
		_, err := d.PopBack()
		return err
	}

	var current *dllNode
	if index < d.size/2 {
		current = d.head
		for i := 0; i < index; i++ {
			current = current.next
		}
	} else {
		current = d.tail
		for i := d.size - 1; i > index; i-- {
			current = current.prev
		}
	}

	current.prev.next = current.next
	current.next.prev = current.prev
	d.size--
	return nil
}

// RemoveValue удаляет первое вхождение значения
func (d *DoublyLinkedList) RemoveValue(value string) bool {
	current := d.head
	for current != nil {
		if current.value == value {
			switch current {
			case d.head:
				_, err := d.PopFront()
				return err == nil
			case d.tail:
				_, err := d.PopBack()
				return err == nil
			default:
				current.prev.next = current.next
				current.next.prev = current.prev
				d.size--
				return true
			}
		}
		current = current.next
	}
	return false
}

// Get возвращает элемент по индексу
func (d *DoublyLinkedList) Get(index int) (string, error) {
	if err := checkIndex(index, d.size); err != nil {
		return "", err
	}

	if index < d.size/2 {
		current := d.head
		for i := 0; i < index; i++ {
			current = current.next
		}
		return current.value, nil
	}
	current := d.tail
	for i := d.size - 1; i > index; i-- {
		current = current.prev
	}
	return current.value, nil
}

// IndexOf возвращает индекс первого вхождения значения
func (d *DoublyLinkedList) IndexOf(value string) int {
	current := d.head
	index := 0
	for current != nil {
		if current.value == value {
			return index
		}
		current = current.next
		index++
	}
	return -1
}

// Contains проверяет наличие значения в списке
func (d *DoublyLinkedList) Contains(value string) bool {
	return d.IndexOf(value) != -1
}

// Size возвращает количество элементов
func (d *DoublyLinkedList) Size() int {
	return d.size
}

// IsEmpty проверяет пуст ли список
func (d *DoublyLinkedList) IsEmpty() bool {
	return d.size == 0
}

// Clear очищает список
func (d *DoublyLinkedList) Clear() {
	d.head = nil
	d.tail = nil
	d.size = 0
}

// Values возвращает все элементы списка
func (d *DoublyLinkedList) Values() []string {
	values := make([]string, 0, d.size)
	current := d.head
	for current != nil {
		values = append(values, current.value)
		current = current.next
	}
	return values
}

// Print выводит список в stdout
func (d *DoublyLinkedList) Print() {
	fmt.Print("DoublyLinkedList: nil <- ")
	current := d.head
	for current != nil {
		fmt.Printf("%s", current.value)
		if current.next != nil {
			fmt.Print(" <-> ")
		}
		current = current.next
	}
	fmt.Println(" -> nil")
}

// Serialize сериализует список в JSON
func (d *DoublyLinkedList) Serialize() ([]byte, error) {
	values := d.Values()
	return json.Marshal(values)
}

// Deserialize десериализует список из JSON
func (d *DoublyLinkedList) Deserialize(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return err
	}

	d.Clear()
	for _, value := range values {
		d.PushBack(value)
	}
	return nil
}

// SerializeBinary сериализует список в бинарный формат
func (d *DoublyLinkedList) SerializeBinary() ([]byte, error) {
	values := d.Values()
	type ListData struct {
		Size   int
		Values []string
	}

	listData := ListData{
		Size:   d.size,
		Values: values,
	}

	return serializeToBinary(listData)
}

// DeserializeBinary десериализует список из бинарного формата
func (d *DoublyLinkedList) DeserializeBinary(data []byte) error {
	type ListData struct {
		Size   int
		Values []string
	}

	var listData ListData
	if err := deserializeFromBinary(data, &listData); err != nil {
		return err
	}

	d.Clear()
	for _, value := range listData.Values {
		d.PushBack(value)
	}
	return nil
}
