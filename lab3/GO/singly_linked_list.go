package main

import (
	"encoding/json"
	"fmt"
)

type sllNode struct {
	value string
	next  *sllNode
}

type SinglyLinkedList struct {
	head *sllNode
	tail *sllNode
	size int
}

func NewSinglyLinkedList() *SinglyLinkedList {
	return &SinglyLinkedList{}
}

func (s *SinglyLinkedList) PushFront(value string) {
	newNode := &sllNode{value: value}
	if s.head == nil {
		s.head = newNode
		s.tail = newNode
	} else {
		newNode.next = s.head
		s.head = newNode
	}
	s.size++
}

func (s *SinglyLinkedList) PushBack(value string) {
	newNode := &sllNode{value: value}
	if s.tail == nil {
		s.head = newNode
		s.tail = newNode
	} else {
		s.tail.next = newNode
		s.tail = newNode
	}
	s.size++
}

func (s *SinglyLinkedList) PopFront() (string, error) {
	if s.head == nil {
		return "", ErrEmptyContainer
	}

	value := s.head.value
	s.head = s.head.next
	s.size--

	if s.head == nil {
		s.tail = nil
	}
	return value, nil
}

func (s *SinglyLinkedList) PopBack() (string, error) {
	if s.head == nil {
		return "", ErrEmptyContainer
	}

	if s.head.next == nil {
		return s.PopFront()
	}

	current := s.head
	for current.next != s.tail {
		current = current.next
	}

	value := s.tail.value
	current.next = nil
	s.tail = current
	s.size--
	return value, nil
}

func (s *SinglyLinkedList) Insert(index int, value string) error {
	if index < 0 || index > s.size {
		return ErrIndexOutOfRange
	}

	if index == 0 {
		s.PushFront(value)
		return nil
	}

	if index == s.size {
		s.PushBack(value)
		return nil
	}

	current := s.head
	for i := 0; i < index-1; i++ {
		current = current.next
	}

	newNode := &sllNode{value: value, next: current.next}
	current.next = newNode
	s.size++
	return nil
}

func (s *SinglyLinkedList) Remove(index int) error {
	if err := checkIndex(index, s.size); err != nil {
		return err
	}

	if index == 0 {
		_, err := s.PopFront()
		return err
	}

	current := s.head
	for i := 0; i < index-1; i++ {
		current = current.next
	}

	if current.next == s.tail {
		s.tail = current
	}
	current.next = current.next.next
	s.size--
	return nil
}

func (s *SinglyLinkedList) RemoveValue(value string) bool {
	if s.head == nil {
		return false
	}

	if s.head.value == value {
		_, err := s.PopFront()
		return err == nil
	}

	current := s.head
	for current.next != nil && current.next.value != value {
		current = current.next
	}

	if current.next != nil {
		if current.next == s.tail {
			s.tail = current
		}
		current.next = current.next.next
		s.size--
		return true
	}
	return false
}

func (s *SinglyLinkedList) Get(index int) (string, error) {
	if err := checkIndex(index, s.size); err != nil {
		return "", err
	}

	current := s.head
	for i := 0; i < index; i++ {
		current = current.next
	}
	return current.value, nil
}

func (s *SinglyLinkedList) IndexOf(value string) int {
	current := s.head
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

func (s *SinglyLinkedList) Contains(value string) bool {
	return s.IndexOf(value) != -1
}

func (s *SinglyLinkedList) Size() int {
	return s.size
}

func (s *SinglyLinkedList) IsEmpty() bool {
	return s.size == 0
}

func (s *SinglyLinkedList) Clear() {
	s.head = nil
	s.tail = nil
	s.size = 0
}

func (s *SinglyLinkedList) Values() []string {
	values := make([]string, 0, s.size)
	current := s.head
	for current != nil {
		values = append(values, current.value)
		current = current.next
	}
	return values
}

func (s *SinglyLinkedList) Print() {
	fmt.Print("SinglyLinkedList: ")
	current := s.head
	for current != nil {
		fmt.Printf("%s", current.value)
		if current.next != nil {
			fmt.Print(" -> ")
		}
		current = current.next
	}
	fmt.Println()
}

func (s *SinglyLinkedList) Serialize() ([]byte, error) {
	values := s.Values()
	return json.Marshal(values)
}

func (s *SinglyLinkedList) Deserialize(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return err
	}

	s.Clear()
	for _, value := range values {
		s.PushBack(value)
	}
	return nil
}

// SerializeBinary сериализует список в бинарный формат
func (s *SinglyLinkedList) SerializeBinary() ([]byte, error) {
	values := s.Values()
	type ListData struct {
		Size   int
		Values []string
	}

	listData := ListData{
		Size:   s.size,
		Values: values,
	}

	return serializeToBinary(listData)
}

// DeserializeBinary десериализует список из бинарного формата
func (s *SinglyLinkedList) DeserializeBinary(data []byte) error {
	type ListData struct {
		Size   int
		Values []string
	}

	var listData ListData
	if err := deserializeFromBinary(data, &listData); err != nil {
		return err
	}

	s.Clear()
	for _, value := range listData.Values {
		s.PushBack(value)
	}
	return nil
}
