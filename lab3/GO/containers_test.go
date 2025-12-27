package main

import (
	"fmt"
	"strconv"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// ==================== ARRAY TESTS ====================

func TestArrayBasicOperations(t *testing.T) {
	arr := NewArray()

	if !arr.IsEmpty() {
		t.Error("New array should be empty")
	}
	if arr.Size() != 0 {
		t.Error("New array size should be 0")
	}

	arr.Insert("one")
	arr.Insert("two")
	arr.Insert("three")

	if arr.IsEmpty() {
		t.Error("Array should not be empty after inserts")
	}
	if arr.Size() != 3 {
		t.Errorf("Array size should be 3, got %d", arr.Size())
	}
}

func TestArrayGetSetOperations(t *testing.T) {
	arr := NewArray()
	arr.Insert("one")
	arr.Insert("two")
	arr.Insert("three")

	val, err := arr.Get(0)
	if err != nil {
		t.Error("Should not get error for valid index")
	}
	if val != "one" {
		t.Error("Get should return correct value")
	}

	err = arr.Set(1, "modified")
	if err != nil {
		t.Error("Should not get error for valid set")
	}
	val, err = arr.Get(1)
	if err != nil {
		t.Error("Should not get error for valid index")
	}
	if val != "modified" {
		t.Error("Set should modify correct value")
	}
}

func TestArrayInsertAtOperations(t *testing.T) {
	arr := NewArray()
	arr.Insert("one")
	arr.Insert("three")

	err := arr.InsertAt(0, "zero")
	if err != nil {
		t.Error("Should not get error for valid insert at beginning")
	}
	val, _ := arr.Get(0)
	if val != "zero" {
		t.Error("Insert at beginning should work correctly")
	}

	err = arr.InsertAt(2, "two")
	if err != nil {
		t.Error("Should not get error for valid insert at middle")
	}
	val, _ = arr.Get(2)
	if val != "two" {
		t.Error("Insert at middle should work correctly")
	}

	err = arr.InsertAt(4, "four")
	if err != nil {
		t.Error("Should not get error for valid insert at end")
	}
	val, _ = arr.Get(4)
	if val != "four" {
		t.Error("Insert at end should work correctly")
	}
}

func TestArrayRemoveOperations(t *testing.T) {
	arr := NewArray()
	arr.Insert("one")
	arr.Insert("two")
	arr.Insert("three")
	arr.Insert("four")

	err := arr.Remove(0)
	if err != nil {
		t.Error("Should not get error for valid remove from beginning")
	}
	val, _ := arr.Get(0)
	if val != "two" {
		t.Error("Remove from beginning should work correctly")
	}

	err = arr.Remove(1)
	if err != nil {
		t.Error("Should not get error for valid remove from middle")
	}
	val, _ = arr.Get(1)
	if val != "four" {
		t.Error("Remove from middle should work correctly")
	}

	err = arr.Remove(1)
	if err != nil {
		t.Error("Should not get error for valid remove from end")
	}
	if arr.Size() != 1 {
		t.Errorf("Array size should be 1 after removes, got %d", arr.Size())
	}
}

func TestArrayErrorConditions(t *testing.T) {
	arr := NewArray()
	arr.Insert("one")

	_, err := arr.Get(1)
	if err == nil {
		t.Error("Should get error for invalid get index")
	}

	err = arr.Set(1, "test")
	if err == nil {
		t.Error("Should get error for invalid set index")
	}

	err = arr.Remove(1)
	if err == nil {
		t.Error("Should get error for invalid remove index")
	}

	err = arr.InsertAt(2, "test")
	if err == nil {
		t.Error("Should get error for invalid insert index")
	}
}

func TestArrayClearAndEmpty(t *testing.T) {
	arr := NewArray()
	arr.Insert("one")
	arr.Insert("two")
	arr.Insert("three")

	arr.Clear()
	if !arr.IsEmpty() {
		t.Error("Array should be empty after clear")
	}
	if arr.Size() != 0 {
		t.Error("Array size should be 0 after clear")
	}
}

func TestArrayWithSizeConstructor(t *testing.T) {
	arr := NewArrayWithSize(5)
	if arr.Size() != 5 {
		t.Errorf("Array with size should have size 5, got %d", arr.Size())
	}

	val, err := arr.Get(0)
	if err != nil {
		t.Error("Should not get error for valid index")
	}
	if val != "" {
		t.Error("New array elements should be empty strings")
	}
}

func TestArraySerialization(t *testing.T) {
	arr := NewArray()
	arr.Insert("test1")
	arr.Insert("test2")
	arr.Insert("test3")

	data, err := arr.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newArr := NewArray()
	err = newArr.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if arr.Size() != newArr.Size() {
		t.Error("Sizes should match after serialization")
	}
	for i := 0; i < arr.Size(); i++ {
		val1, _ := arr.Get(i)
		val2, _ := newArr.Get(i)
		if val1 != val2 {
			t.Error("Values should match after serialization")
		}
	}
}

func TestArrayExtendedOperations(t *testing.T) {
	arr := NewArray()

	arr.Insert("one")
	arr.Insert("two")
	arr.Insert("three")

	if !arr.RemoveValue("two") {
		t.Error("RemoveValue should return true for existing value")
	}
	if arr.Size() != 2 {
		t.Error("Size should decrease after RemoveValue")
	}
	if arr.RemoveValue("nonexistent") {
		t.Error("RemoveValue should return false for non-existing value")
	}

	if arr.IndexOf("one") != 0 {
		t.Error("IndexOf should return correct index")
	}
	if arr.IndexOf("nonexistent") != -1 {
		t.Error("IndexOf should return -1 for non-existing value")
	}
	if !arr.Contains("one") {
		t.Error("Contains should return true for existing value")
	}
	if arr.Contains("nonexistent") {
		t.Error("Contains should return false for non-existing value")
	}

	values := arr.Values()
	if len(values) != 2 {
		t.Error("Values should return all elements")
	}
}

func TestArrayEdgeCases(t *testing.T) {
	arr := NewArrayWithSize(-1)
	if arr.Size() != 0 {
		t.Error("Array with negative size should have size 0")
	}

	emptyArr := NewArray()
	if emptyArr.RemoveValue("test") {
		t.Error("RemoveValue on empty array should return false")
	}
	if emptyArr.IndexOf("test") != -1 {
		t.Error("IndexOf on empty array should return -1")
	}
	if emptyArr.Contains("test") {
		t.Error("Contains on empty array should return false")
	}

	values := emptyArr.Values()
	if len(values) != 0 {
		t.Error("Values on empty array should return empty slice")
	}
}

// ==================== SINGLY LINKED LIST TESTS ====================

func TestSinglyLinkedListBasicOperations(t *testing.T) {
	list := NewSinglyLinkedList()

	if !list.IsEmpty() {
		t.Error("New list should be empty")
	}
	if list.Size() != 0 {
		t.Error("New list size should be 0")
	}
}

func TestSinglyLinkedListPushOperations(t *testing.T) {
	list := NewSinglyLinkedList()

	list.PushFront("front")
	if list.Size() != 1 {
		t.Errorf("List size should be 1 after push front, got %d", list.Size())
	}

	list.PushBack("back")
	if list.Size() != 2 {
		t.Errorf("List size should be 2 after push back, got %d", list.Size())
	}
}

func TestSinglyLinkedListGetOperations(t *testing.T) {
	list := NewSinglyLinkedList()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	val, err := list.Get(0)
	if err != nil || val != "first" {
		t.Error("Get at index 0 should work correctly")
	}

	val, err = list.Get(1)
	if err != nil || val != "second" {
		t.Error("Get at index 1 should work correctly")
	}

	val, err = list.Get(2)
	if err != nil || val != "third" {
		t.Error("Get at index 2 should work correctly")
	}
}

func TestSinglyLinkedListInsertOperations(t *testing.T) {
	list := NewSinglyLinkedList()
	list.PushBack("first")
	list.PushBack("third")

	err := list.Insert(1, "second")
	if err != nil {
		t.Error("Should not get error for valid insert")
	}
	if list.Size() != 3 {
		t.Errorf("List size should be 3 after insert, got %d", list.Size())
	}
	val, _ := list.Get(1)
	if val != "second" {
		t.Error("Insert should place value correctly")
	}

	err = list.Insert(0, "zero")
	if err != nil {
		t.Error("Should not get error for valid insert at beginning")
	}
	val, _ = list.Get(0)
	if val != "zero" {
		t.Error("Insert at beginning should work correctly")
	}

	err = list.Insert(4, "end")
	if err != nil {
		t.Error("Should not get error for valid insert at end")
	}
	val, _ = list.Get(4)
	if val != "end" {
		t.Error("Insert at end should work correctly")
	}
}

func TestSinglyLinkedListPopOperations(t *testing.T) {
	list := NewSinglyLinkedList()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	val, err := list.PopFront()
	if err != nil {
		t.Error("Should not get error for valid pop front")
	}
	if val != "first" {
		t.Error("PopFront should return correct value")
	}
	if list.Size() != 2 {
		t.Errorf("List size should be 2 after pop front, got %d", list.Size())
	}

	val, err = list.PopBack()
	if err != nil {
		t.Error("Should not get error for valid pop back")
	}
	if val != "third" {
		t.Error("PopBack should return correct value")
	}
	if list.Size() != 1 {
		t.Errorf("List size should be 1 after pop back, got %d", list.Size())
	}
}

func TestSinglyLinkedListRemoveOperations(t *testing.T) {
	list := NewSinglyLinkedList()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	err := list.Remove(1)
	if err != nil {
		t.Error("Should not get error for valid remove")
	}
	if list.Size() != 2 {
		t.Errorf("List size should be 2 after remove, got %d", list.Size())
	}
	val, _ := list.Get(1)
	if val != "third" {
		t.Error("Remove should work correctly")
	}

	err = list.Remove(0)
	if err != nil {
		t.Error("Should not get error for valid remove from beginning")
	}
	if list.Size() != 1 {
		t.Errorf("List size should be 1 after remove from beginning, got %d", list.Size())
	}

	err = list.Remove(0)
	if err != nil {
		t.Error("Should not get error for valid remove from end")
	}
	if !list.IsEmpty() {
		t.Error("List should be empty after all removes")
	}
}

func TestSinglyLinkedListErrorConditions(t *testing.T) {
	list := NewSinglyLinkedList()

	_, err := list.Get(0)
	if err == nil {
		t.Error("Should get error for get on empty list")
	}

	_, err = list.PopFront()
	if err == nil {
		t.Error("Should get error for pop front on empty list")
	}
	_, err = list.PopBack()
	if err == nil {
		t.Error("Should get error for pop back on empty list")
	}

	err = list.Remove(0)
	if err == nil {
		t.Error("Should get error for remove on empty list")
	}

	list.PushBack("item")
	err = list.Insert(2, "test")
	if err == nil {
		t.Error("Should get error for insert out of bounds")
	}
}

func TestSinglyLinkedListSerialization(t *testing.T) {
	list := NewSinglyLinkedList()
	list.PushBack("a")
	list.PushBack("b")
	list.PushBack("c")

	data, err := list.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newList := NewSinglyLinkedList()
	err = newList.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if list.Size() != newList.Size() {
		t.Error("Sizes should match after serialization")
	}
	for i := 0; i < list.Size(); i++ {
		val1, _ := list.Get(i)
		val2, _ := newList.Get(i)
		if val1 != val2 {
			t.Error("Values should match after serialization")
		}
	}
}

func TestSinglyLinkedListExtendedOperations(t *testing.T) {
	list := NewSinglyLinkedList()

	list.PushBack("one")
	list.PushBack("two")
	list.PushBack("three")

	if !list.RemoveValue("two") {
		t.Error("RemoveValue should return true for existing value")
	}
	if list.Size() != 2 {
		t.Error("Size should decrease after RemoveValue")
	}
	if list.RemoveValue("nonexistent") {
		t.Error("RemoveValue should return false for non-existing value")
	}

	if list.IndexOf("one") != 0 {
		t.Error("IndexOf should return correct index")
	}
	if list.IndexOf("nonexistent") != -1 {
		t.Error("IndexOf should return -1 for non-existing value")
	}
	if !list.Contains("one") {
		t.Error("Contains should return true for existing value")
	}
	if list.Contains("nonexistent") {
		t.Error("Contains should return false for non-existing value")
	}

	values := list.Values()
	if len(values) != 2 {
		t.Error("Values should return all elements")
	}
}

func TestSinglyLinkedListEdgeCases(t *testing.T) {
	list := NewSinglyLinkedList()

	if list.RemoveValue("test") {
		t.Error("RemoveValue on empty list should return false")
	}

	list.PushBack("single")
	if !list.RemoveValue("single") {
		t.Error("RemoveValue should work on single element list")
	}
	if !list.IsEmpty() {
		t.Error("List should be empty after removing single element")
	}

	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	if !list.RemoveValue("third") {
		t.Error("Should be able to remove tail")
	}

	if val, _ := list.Get(list.Size() - 1); val != "second" {
		t.Error("Tail should be updated after removal")
	}

	emptyList := NewSinglyLinkedList()
	values := emptyList.Values()
	if len(values) != 0 {
		t.Error("Values on empty list should return empty slice")
	}
}

// ==================== DOUBLY LINKED LIST TESTS ====================

func TestDoublyLinkedListBasicOperations(t *testing.T) {
	list := NewDoublyLinkedList()

	if !list.IsEmpty() {
		t.Error("New list should be empty")
	}
	if list.Size() != 0 {
		t.Error("New list size should be 0")
	}
}

func TestDoublyLinkedListPushOperations(t *testing.T) {
	list := NewDoublyLinkedList()

	list.PushFront("front")
	if list.Size() != 1 {
		t.Errorf("List size should be 1 after push front, got %d", list.Size())
	}

	list.PushBack("back")
	if list.Size() != 2 {
		t.Errorf("List size should be 2 after push back, got %d", list.Size())
	}
}

func TestDoublyLinkedListGetOperations(t *testing.T) {
	list := NewDoublyLinkedList()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	val, err := list.Get(0)
	if err != nil || val != "first" {
		t.Error("Get at index 0 should work correctly")
	}

	val, err = list.Get(1)
	if err != nil || val != "second" {
		t.Error("Get at index 1 should work correctly")
	}

	val, err = list.Get(2)
	if err != nil || val != "third" {
		t.Error("Get at index 2 should work correctly")
	}
}

func TestDoublyLinkedListInsertOperations(t *testing.T) {
	list := NewDoublyLinkedList()
	list.PushBack("first")
	list.PushBack("third")

	err := list.Insert(1, "second")
	if err != nil {
		t.Error("Should not get error for valid insert")
	}
	if list.Size() != 3 {
		t.Errorf("List size should be 3 after insert, got %d", list.Size())
	}
	val, _ := list.Get(1)
	if val != "second" {
		t.Error("Insert should place value correctly")
	}

	err = list.Insert(0, "zero")
	if err != nil {
		t.Error("Should not get error for valid insert at beginning")
	}
	val, _ = list.Get(0)
	if val != "zero" {
		t.Error("Insert at beginning should work correctly")
	}

	err = list.Insert(4, "end")
	if err != nil {
		t.Error("Should not get error for valid insert at end")
	}
	val, _ = list.Get(4)
	if val != "end" {
		t.Error("Insert at end should work correctly")
	}
}

func TestDoublyLinkedListPopOperations(t *testing.T) {
	list := NewDoublyLinkedList()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	val, err := list.PopFront()
	if err != nil {
		t.Error("Should not get error for valid pop front")
	}
	if val != "first" {
		t.Error("PopFront should return correct value")
	}
	if list.Size() != 2 {
		t.Errorf("List size should be 2 after pop front, got %d", list.Size())
	}

	val, err = list.PopBack()
	if err != nil {
		t.Error("Should not get error for valid pop back")
	}
	if val != "third" {
		t.Error("PopBack should return correct value")
	}
	if list.Size() != 1 {
		t.Errorf("List size should be 1 after pop back, got %d", list.Size())
	}
}

func TestDoublyLinkedListRemoveOperations(t *testing.T) {
	list := NewDoublyLinkedList()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	err := list.Remove(1)
	if err != nil {
		t.Error("Should not get error for valid remove")
	}
	if list.Size() != 2 {
		t.Errorf("List size should be 2 after remove, got %d", list.Size())
	}
	val, _ := list.Get(1)
	if val != "third" {
		t.Error("Remove should work correctly")
	}

	err = list.Remove(0)
	if err != nil {
		t.Error("Should not get error for valid remove from beginning")
	}
	if list.Size() != 1 {
		t.Errorf("List size should be 1 after remove from beginning, got %d", list.Size())
	}

	err = list.Remove(0)
	if err != nil {
		t.Error("Should not get error for valid remove from end")
	}
	if !list.IsEmpty() {
		t.Error("List should be empty after all removes")
	}
}

func TestDoublyLinkedListErrorConditions(t *testing.T) {
	list := NewDoublyLinkedList()

	_, err := list.Get(0)
	if err == nil {
		t.Error("Should get error for get on empty list")
	}

	_, err = list.PopFront()
	if err == nil {
		t.Error("Should get error for pop front on empty list")
	}
	_, err = list.PopBack()
	if err == nil {
		t.Error("Should get error for pop back on empty list")
	}

	err = list.Remove(0)
	if err == nil {
		t.Error("Should get error for remove on empty list")
	}

	list.PushBack("item")
	err = list.Insert(2, "test")
	if err == nil {
		t.Error("Should get error for insert out of bounds")
	}
}

func TestDoublyLinkedListSerialization(t *testing.T) {
	list := NewDoublyLinkedList()
	list.PushBack("x")
	list.PushBack("y")
	list.PushBack("z")

	data, err := list.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newList := NewDoublyLinkedList()
	err = newList.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if list.Size() != newList.Size() {
		t.Error("Sizes should match after serialization")
	}
	for i := 0; i < list.Size(); i++ {
		val1, _ := list.Get(i)
		val2, _ := newList.Get(i)
		if val1 != val2 {
			t.Error("Values should match after serialization")
		}
	}
}

func TestDoublyLinkedListExtendedOperations(t *testing.T) {
	list := NewDoublyLinkedList()

	list.PushBack("one")
	list.PushBack("two")
	list.PushBack("three")

	if !list.RemoveValue("two") {
		t.Error("RemoveValue should return true for existing value")
	}
	if list.Size() != 2 {
		t.Error("Size should decrease after RemoveValue")
	}
	if list.RemoveValue("nonexistent") {
		t.Error("RemoveValue should return false for non-existing value")
	}

	if list.IndexOf("one") != 0 {
		t.Error("IndexOf should return correct index")
	}
	if list.IndexOf("nonexistent") != -1 {
		t.Error("IndexOf should return -1 for non-existing value")
	}
	if !list.Contains("one") {
		t.Error("Contains should return true for existing value")
	}
	if list.Contains("nonexistent") {
		t.Error("Contains should return false for non-existing value")
	}

	values := list.Values()
	if len(values) != 2 {
		t.Error("Values should return all elements")
	}
}

func TestDoublyLinkedListEdgeCases(t *testing.T) {
	list := NewDoublyLinkedList()

	if list.RemoveValue("test") {
		t.Error("RemoveValue on empty list should return false")
	}

	list.PushBack("single")
	if !list.RemoveValue("single") {
		t.Error("RemoveValue should work on single element list")
	}
	if !list.IsEmpty() {
		t.Error("List should be empty after removing single element")
	}

	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	if !list.RemoveValue("first") {
		t.Error("Should be able to remove head")
	}
	if val, _ := list.Get(0); val != "second" {
		t.Error("Head should be updated after removal")
	}

	if !list.RemoveValue("third") {
		t.Error("Should be able to remove tail")
	}
	if val, _ := list.Get(list.Size() - 1); val != "second" {
		t.Error("Tail should be updated after removal")
	}

	emptyList := NewDoublyLinkedList()
	values := emptyList.Values()
	if len(values) != 0 {
		t.Error("Values on empty list should return empty slice")
	}
}

func TestDoublyLinkedListCoverage(t *testing.T) {
	list := NewDoublyLinkedList()

	if list.IndexOf("nonexistent") != -1 {
		t.Error("IndexOf should return -1 for empty list")
	}
	if list.Contains("nonexistent") {
		t.Error("Contains should return false for empty list")
	}

	list.PushFront("a")
	list.PushBack("b")
	list.PushFront("c")

	if val, _ := list.Get(0); val != "c" {
		t.Error("Get should work after multiple push operations")
	}
	if val, _ := list.Get(2); val != "b" {
		t.Error("Get should work for last element")
	}

	singleList := NewDoublyLinkedList()
	singleList.PushBack("single")
	if !singleList.RemoveValue("single") {
		t.Error("RemoveValue should work on single element list")
	}
	if !singleList.IsEmpty() {
		t.Error("List should be empty after removing single element")
	}

	list.Clear()
	list.PushBack("1")
	list.PushBack("2")
	list.PushBack("3")
	list.PushBack("4")

	err := list.Remove(1)
	if err != nil {
		t.Errorf("Remove failed: %v", err)
	}
	if val, _ := list.Get(1); val != "3" {
		t.Error("Remove should maintain proper links")
	}

	emptyList := NewDoublyLinkedList()
	data, err := emptyList.Serialize()
	if err != nil {
		t.Error("Serialize should work on empty list")
	}
	if err := emptyList.Deserialize(data); err != nil {
		t.Error("Deserialize should work on empty data")
	}
}

// ==================== QUEUE TESTS ====================

func TestQueueBasicOperations(t *testing.T) {
	queue := NewQueue()

	if !queue.IsEmpty() {
		t.Error("New queue should be empty")
	}
	if queue.Size() != 0 {
		t.Error("New queue size should be 0")
	}
}

func TestQueueEnqueueDequeue(t *testing.T) {
	queue := NewQueue()

	queue.Enqueue("first")
	queue.Enqueue("second")
	queue.Enqueue("third")

	if queue.Size() != 3 {
		t.Errorf("Queue size should be 3, got %d", queue.Size())
	}

	val, err := queue.Dequeue()
	if err != nil {
		t.Error("Should not get error for valid dequeue")
	}
	if val != "first" {
		t.Error("Dequeue should return first element")
	}
	if queue.Size() != 2 {
		t.Errorf("Queue size should be 2 after dequeue, got %d", queue.Size())
	}
}

func TestQueueFrontBackOperations(t *testing.T) {
	queue := NewQueue()
	queue.Enqueue("first")
	queue.Enqueue("second")
	queue.Enqueue("third")

	front, err := queue.Front()
	if err != nil {
		t.Error("Should not get error for valid front")
	}
	if front != "first" {
		t.Error("Front should return first element")
	}

	back, err := queue.Back()
	if err != nil {
		t.Error("Should not get error for valid back")
	}
	if back != "third" {
		t.Error("Back should return last element")
	}
}

func TestQueueErrorConditions(t *testing.T) {
	queue := NewQueue()

	_, err := queue.Dequeue()
	if err == nil {
		t.Error("Should get error for dequeue on empty queue")
	}

	_, err = queue.Front()
	if err == nil {
		t.Error("Should get error for front on empty queue")
	}

	_, err = queue.Back()
	if err == nil {
		t.Error("Should get error for back on empty queue")
	}
}

func TestQueueClear(t *testing.T) {
	queue := NewQueue()
	queue.Enqueue("first")
	queue.Enqueue("second")

	queue.Clear()
	if !queue.IsEmpty() {
		t.Error("Queue should be empty after clear")
	}
	if queue.Size() != 0 {
		t.Error("Queue size should be 0 after clear")
	}
}

func TestQueueSerialization(t *testing.T) {
	queue := NewQueue()
	queue.Enqueue("q1")
	queue.Enqueue("q2")
	queue.Enqueue("q3")

	data, err := queue.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newQueue := NewQueue()
	err = newQueue.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if queue.Size() != newQueue.Size() {
		t.Error("Sizes should match after serialization")
	}

	for !queue.IsEmpty() {
		val1, _ := queue.Dequeue()
		val2, _ := newQueue.Dequeue()
		if val1 != val2 {
			t.Error("Values should match after serialization")
		}
	}
}

// ==================== STACK TESTS ====================

func TestStackBasicOperations(t *testing.T) {
	stack := NewStack()

	if !stack.IsEmpty() {
		t.Error("New stack should be empty")
	}
	if stack.Size() != 0 {
		t.Error("New stack size should be 0")
	}
}

func TestStackPushPop(t *testing.T) {
	stack := NewStack()

	stack.Push("first")
	stack.Push("second")
	stack.Push("third")

	if stack.Size() != 3 {
		t.Errorf("Stack size should be 3, got %d", stack.Size())
	}

	val, err := stack.Pop()
	if err != nil {
		t.Error("Should not get error for valid pop")
	}
	if val != "third" {
		t.Error("Pop should return last element")
	}
	if stack.Size() != 2 {
		t.Errorf("Stack size should be 2 after pop, got %d", stack.Size())
	}
}

func TestStackTopOperation(t *testing.T) {
	stack := NewStack()
	stack.Push("first")
	stack.Push("second")
	stack.Push("third")

	top, err := stack.Top()
	if err != nil {
		t.Error("Should not get error for valid top")
	}
	if top != "third" {
		t.Error("Top should return last element")
	}

	if stack.Size() != 3 {
		t.Error("Top should not change stack size")
	}
}

func TestStackErrorConditions(t *testing.T) {
	stack := NewStack()

	_, err := stack.Pop()
	if err == nil {
		t.Error("Should get error for pop on empty stack")
	}

	_, err = stack.Top()
	if err == nil {
		t.Error("Should get error for top on empty stack")
	}
}

func TestStackClear(t *testing.T) {
	stack := NewStack()
	stack.Push("first")
	stack.Push("second")

	stack.Clear()
	if !stack.IsEmpty() {
		t.Error("Stack should be empty after clear")
	}
	if stack.Size() != 0 {
		t.Error("Stack size should be 0 after clear")
	}
}

func TestStackSerialization(t *testing.T) {
	stack := NewStack()
	stack.Push("s1")
	stack.Push("s2")
	stack.Push("s3")

	data, err := stack.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newStack := NewStack()
	err = newStack.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if stack.Size() != newStack.Size() {
		t.Error("Sizes should match after serialization")
	}

	for !stack.IsEmpty() {
		val1, _ := stack.Pop()
		val2, _ := newStack.Pop()
		if val1 != val2 {
			t.Error("Values should match after serialization")
		}
	}
}

// ==================== HASH TABLE TESTS ====================

func TestHashTableBasicOperations(t *testing.T) {
	ht := NewHashTable()

	if !ht.IsEmpty() {
		t.Error("New hash table should be empty")
	}
	if ht.Size() != 0 {
		t.Error("New hash table size should be 0")
	}
}

func TestHashTableInsertGet(t *testing.T) {
	ht := NewHashTable()

	ht.Insert(1, "Alice")
	ht.Insert(2, "Bob")
	ht.Insert(3, "Charlie")

	if ht.Size() != 3 {
		t.Errorf("Hash table size should be 3, got %d", ht.Size())
	}

	val, exists := ht.Get(1)
	if !exists || val != "Alice" {
		t.Error("Get should return correct value for existing key")
	}

	val, exists = ht.Get(2)
	if !exists || val != "Bob" {
		t.Error("Get should return correct value for existing key")
	}
}

func TestHashTableUpdate(t *testing.T) {
	ht := NewHashTable()

	ht.Insert(1, "Alice")
	ht.Insert(1, "Alicia")

	val, exists := ht.Get(1)
	if !exists || val != "Alicia" {
		t.Error("Insert should update existing key")
	}
	if ht.Size() != 1 {
		t.Error("Size should not change when updating existing key")
	}
}

func TestHashTableContains(t *testing.T) {
	ht := NewHashTable()
	ht.Insert(1, "Alice")
	ht.Insert(2, "Bob")

	if !ht.Contains(1) {
		t.Error("Contains should return true for existing key")
	}
	if !ht.Contains(2) {
		t.Error("Contains should return true for existing key")
	}
	if ht.Contains(999) {
		t.Error("Contains should return false for non-existing key")
	}
}

func TestHashTableRemove(t *testing.T) {
	ht := NewHashTable()
	ht.Insert(1, "Alice")
	ht.Insert(2, "Bob")
	ht.Insert(3, "Charlie")

	removed := ht.Remove(2)
	if !removed {
		t.Error("Remove should return true for existing key")
	}
	if ht.Size() != 2 {
		t.Errorf("Hash table size should be 2 after remove, got %d", ht.Size())
	}
	if ht.Contains(2) {
		t.Error("Contains should return false for removed key")
	}

	removed = ht.Remove(999)
	if removed {
		t.Error("Remove should return false for non-existing key")
	}
}

func TestHashTableCollisions(t *testing.T) {
	ht := NewHashTableWithCapacity(4)

	ht.Insert(1, "Alice")
	ht.Insert(5, "Bob")
	ht.Insert(9, "Charlie")

	if ht.Size() != 3 {
		t.Errorf("Hash table should handle collisions, size should be 3, got %d", ht.Size())
	}

	if !ht.Contains(1) || !ht.Contains(5) || !ht.Contains(9) {
		t.Error("Hash table should handle collisions correctly")
	}
}

func TestHashTableRehashing(t *testing.T) {
	ht := NewHashTableWithCapacity(4)

	for i := 0; i < 10; i++ {
		ht.Insert(i, string(rune('a'+i)))
	}

	if ht.Size() != 10 {
		t.Errorf("Hash table should work after rehashing, size should be 10, got %d", ht.Size())
	}

	for i := 0; i < 10; i++ {
		if !ht.Contains(i) {
			t.Errorf("Key %d should be accessible after rehashing", i)
		}
	}
}

func TestHashTableChainStatistics(t *testing.T) {
	ht := NewHashTableWithCapacity(4)

	ht.Insert(1, "Alice")
	ht.Insert(5, "Bob")
	ht.Insert(9, "Charlie")
	ht.Insert(2, "David")

	longest := ht.GetLongestChain()
	shortest := ht.GetShortestChain()

	if longest < 1 {
		t.Error("Longest chain should be at least 1")
	}
	if shortest < 0 {
		t.Error("Shortest chain should be non-negative")
	}
}

func TestHashTableErrorConditions(t *testing.T) {
	ht := NewHashTable()

	_, exists := ht.Get(999)
	if exists {
		t.Error("Get should return false for non-existing key")
	}
}

func TestHashTableClear(t *testing.T) {
	ht := NewHashTable()
	ht.Insert(1, "Alice")
	ht.Insert(2, "Bob")

	ht.Clear()
	if !ht.IsEmpty() {
		t.Error("Hash table should be empty after clear")
	}
	if ht.Size() != 0 {
		t.Error("Hash table size should be 0 after clear")
	}
}

func TestHashTableSerialization(t *testing.T) {
	ht := NewHashTable()
	ht.Insert(1, "one")
	ht.Insert(2, "two")
	ht.Insert(3, "three")

	data, err := ht.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newHt := NewHashTable()
	err = newHt.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if ht.Size() != newHt.Size() {
		t.Error("Sizes should match after serialization")
	}

	for i := 1; i <= 3; i++ {
		val1, exists1 := ht.Get(i)
		val2, exists2 := newHt.Get(i)

		if exists1 != exists2 {
			t.Error("Key existence should match after serialization")
		}
		if val1 != val2 {
			t.Error("Values should match after serialization")
		}
	}
}

func TestHashTableExtendedOperations(t *testing.T) {
	ht := NewHashTable()

	ht.Insert(1, "one")
	ht.Insert(2, "two")
	ht.Insert(3, "three")

	keys := ht.Keys()
	values := ht.Values()

	if len(keys) != 3 {
		t.Error("Keys should return all keys")
	}
	if len(values) != 3 {
		t.Error("Values should return all values")
	}

	if ht.Capacity() <= 0 {
		t.Error("Capacity should be positive")
	}

	loadFactor := ht.GetLoadFactor()
	if loadFactor < 0 {
		t.Error("Load factor should be non-negative")
	}
}

func TestHashTableEdgeCases(t *testing.T) {
	emptyHt := NewHashTable()
	if emptyHt.GetLongestChain() != 0 {
		t.Error("Empty hash table should have longest chain 0")
	}
	if emptyHt.GetShortestChain() != 0 {
		t.Error("Empty hash table should have shortest chain 0")
	}
	if emptyHt.GetLoadFactor() != 0 {
		t.Error("Empty hash table should have load factor 0")
	}

	if emptyHt.Remove(1) {
		t.Error("Remove on empty hash table should return false")
	}
	if emptyHt.Contains(1) {
		t.Error("Contains on empty hash table should return false")
	}

	keys := emptyHt.Keys()
	if len(keys) != 0 {
		t.Error("Empty hash table should return empty keys slice")
	}

	values := emptyHt.Values()
	if len(values) != 0 {
		t.Error("Empty hash table should return empty values slice")
	}
}

func TestHashTableRehashingEdgeCases(t *testing.T) {
	ht := NewHashTableWithCapacity(2)

	for i := 0; i < 10; i++ {
		ht.Insert(i, string(rune('a'+i)))
	}

	if ht.Size() != 10 {
		t.Error("All elements should be inserted after rehashing")
	}

	for i := 0; i < 10; i++ {
		if !ht.Contains(i) {
			t.Errorf("Element %d should be accessible after rehashing", i)
		}
	}
}

func TestHashTableCoverage(t *testing.T) {
	ht := NewHashTable()

	if ht.GetLongestChain() != 0 {
		t.Error("Empty table should have longest chain 0")
	}
	if ht.GetShortestChain() != 0 {
		t.Error("Empty table should have shortest chain 0")
	}
	if ht.GetLoadFactor() != 0 {
		t.Error("Empty table should have load factor 0")
	}

	ht.Insert(1, "one")
	if ht.GetLongestChain() != 1 {
		t.Error("Single element table should have longest chain 1")
	}
	if ht.GetShortestChain() != 1 {
		t.Error("Single element table should have shortest chain 1")
	}

	if ht.Capacity() <= 0 {
		t.Error("Hash table should have positive capacity")
	}

	keys := ht.Keys()
	if len(keys) != 1 || keys[0] != 1 {
		t.Error("Keys should return correct key")
	}
	values := ht.Values()
	if len(values) != 1 || values[0] != "one" {
		t.Error("Values should return correct value")
	}
}

func TestHashTableNegativeKeys(t *testing.T) {
	ht := NewHashTable()

	ht.Insert(-1, "negative one")
	ht.Insert(-100, "negative hundred")

	if !ht.Contains(-1) {
		t.Error("Should contain negative key -1")
	}
	if !ht.Contains(-100) {
		t.Error("Should contain negative key -100")
	}

	val, exists := ht.Get(-1)
	if !exists || val != "negative one" {
		t.Error("Should get value for negative key")
	}

	if !ht.Remove(-1) {
		t.Error("Should remove negative key")
	}
	if ht.Contains(-1) {
		t.Error("Should not contain removed negative key")
	}
}

func TestHashTableZeroCapacity(t *testing.T) {
	ht := NewHashTableWithCapacity(0)
	if ht.Capacity() != defaultCapacity {
		t.Errorf("Should handle zero capacity by using default, got %d, expected %d", ht.Capacity(), defaultCapacity)
	}

	ht2 := NewHashTableWithCapacity(-5)
	if ht2.Capacity() != defaultCapacity {
		t.Errorf("Should handle negative capacity by using default, got %d, expected %d", ht2.Capacity(), defaultCapacity)
	}
}

func TestHashTableChainOperations(t *testing.T) {
	// Create small table to force collisions, но отключим рехэширование временно
	ht := NewHashTableWithCapacity(3)

	// Вставим минимальное количество элементов чтобы избежать рехэширования
	// Рехэширование происходит при load factor > 0.75
	// Для capacity=3 максимальное количество до рехэширования: 3 * 0.75 = 2.25, т.е. 2 элемента
	ht.Insert(1, "a") // 1 % 3 = 1
	ht.Insert(4, "d") // 4 % 3 = 1 - collision

	// Проверим что у нас действительно есть коллизия
	if ht.GetLongestChain() != 2 {
		t.Errorf("Should have chain of length 2 before rehashing, got %d", ht.GetLongestChain())
	}

	// Теперь проверим что все элементы доступны
	if !ht.Contains(1) || !ht.Contains(4) {
		t.Error("All collided keys should be accessible")
	}

	// Удалим из середины цепочки
	if !ht.Remove(4) {
		t.Error("Should remove from middle of chain")
	}
	if ht.Contains(4) {
		t.Error("Should not contain removed key from chain")
	}

	// После удаления цепочка должна стать короче
	if ht.GetLongestChain() != 1 {
		t.Errorf("Should have chain of length 1 after removal, got %d", ht.GetLongestChain())
	}
}

// ==================== BINARY TREE TESTS ====================

func TestBinaryTreeBasicOperations(t *testing.T) {
	tree := NewBinaryTree()

	if !tree.IsEmpty() {
		t.Error("New tree should be empty")
	}
	if tree.Size() != 0 {
		t.Error("New tree size should be 0")
	}
}

func TestBinaryTreeInsertSearch(t *testing.T) {
	tree := NewBinaryTree()

	tree.Insert("dog")
	tree.Insert("cat")
	tree.Insert("elephant")
	tree.Insert("bird")

	if tree.Size() != 4 {
		t.Errorf("Tree size should be 4, got %d", tree.Size())
	}

	if !tree.Search("cat") {
		t.Error("Search should find existing value")
	}
	if !tree.Search("elephant") {
		t.Error("Search should find existing value")
	}
	if tree.Search("zebra") {
		t.Error("Search should not find non-existing value")
	}
}

func TestBinaryTreeGetRoot(t *testing.T) {
	tree := NewBinaryTree()

	_, exists := tree.GetRoot()
	if exists {
		t.Error("GetRoot should return false for empty tree")
	}

	tree.Insert("dog")
	tree.Insert("cat")

	root, exists := tree.GetRoot()
	if !exists {
		t.Error("GetRoot should return true for non-empty tree")
	}
	if root != "dog" {
		t.Error("GetRoot should return correct root value")
	}
}

func TestBinaryTreeTraversals(t *testing.T) {
	tree := NewBinaryTree()
	values := []string{"dog", "cat", "elephant", "bird", "fish"}

	for _, value := range values {
		tree.Insert(value)
	}

	inOrder := tree.InOrder()
	preOrder := tree.PreOrder()
	postOrder := tree.PostOrder()
	levelOrder := tree.LevelOrder()

	if len(inOrder) != 5 {
		t.Errorf("In-order should return 5 elements, got %d", len(inOrder))
	}
	if len(preOrder) != 5 {
		t.Errorf("Pre-order should return 5 elements, got %d", len(preOrder))
	}
	if len(postOrder) != 5 {
		t.Errorf("Post-order should return 5 elements, got %d", len(postOrder))
	}
	if len(levelOrder) != 5 {
		t.Errorf("Level-order should return 5 elements, got %d", len(levelOrder))
	}

	for _, value := range values {
		found := false
		for _, v := range inOrder {
			if v == value {
				found = true
				break
			}
		}
		if !found {
			t.Errorf("Value %s should be in in-order traversal", value)
		}
	}
}

func TestBinaryTreeRemove(t *testing.T) {
	tree := NewBinaryTree()
	tree.Insert("dog")
	tree.Insert("cat")
	tree.Insert("elephant")
	tree.Insert("bird")

	removed := tree.Remove("bird")
	if !removed {
		t.Error("Remove should return true for existing value")
	}
	if tree.Size() != 3 {
		t.Errorf("Tree size should be 3 after remove, got %d", tree.Size())
	}
	if tree.Search("bird") {
		t.Error("Search should not find removed value")
	}

	tree.Insert("ant")
	removed = tree.Remove("cat")
	if !removed {
		t.Error("Remove should return true for existing value")
	}
	if tree.Size() != 3 {
		t.Errorf("Tree size should be 3 after remove, got %d", tree.Size())
	}

	tree.Insert("cow")
	tree.Insert("duck")
	removed = tree.Remove("dog")
	if !removed {
		t.Error("Remove should return true for existing value")
	}

	removed = tree.Remove("zebra")
	if removed {
		t.Error("Remove should return false for non-existing value")
	}
}

func TestBinaryTreeErrorConditions(t *testing.T) {
	tree := NewBinaryTree()

	removed := tree.Remove("test")
	if removed {
		t.Error("Remove should return false for empty tree")
	}
}

func TestBinaryTreeClear(t *testing.T) {
	tree := NewBinaryTree()
	tree.Insert("dog")
	tree.Insert("cat")
	tree.Insert("elephant")

	tree.Clear()
	if !tree.IsEmpty() {
		t.Error("Tree should be empty after clear")
	}
	if tree.Size() != 0 {
		t.Error("Tree size should be 0 after clear")
	}
}

func TestBinaryTreeSerialization(t *testing.T) {
	tree := NewBinaryTree()
	tree.Insert("m")
	tree.Insert("f")
	tree.Insert("s")
	tree.Insert("a")
	tree.Insert("h")

	data, err := tree.Serialize()
	if err != nil {
		t.Error("Serialize should not fail")
	}

	newTree := NewBinaryTree()
	err = newTree.Deserialize(data)
	if err != nil {
		t.Error("Deserialize should not fail")
	}

	if tree.Size() != newTree.Size() {
		t.Error("Sizes should match after serialization")
	}

	inOrder1 := tree.InOrder()
	inOrder2 := newTree.InOrder()

	if len(inOrder1) != len(inOrder2) {
		t.Error("Traversal lengths should match")
	}

	for i := range inOrder1 {
		if inOrder1[i] != inOrder2[i] {
			t.Error("Traversal values should match after serialization")
		}
	}
}

func TestBinaryTreeExtendedOperations(t *testing.T) {
	tree := NewBinaryTree()

	if tree.Height() != 0 {
		t.Error("Empty tree should have height 0")
	}

	tree.Insert("m")
	tree.Insert("f")
	tree.Insert("s")
	tree.Insert("a")
	tree.Insert("h")

	if tree.Height() <= 0 {
		t.Error("Non-empty tree should have positive height")
	}

	min, found := tree.GetMin()
	if !found || min != "a" {
		t.Error("GetMin should return smallest value")
	}

	max, found := tree.GetMax()
	if !found || max != "s" {
		t.Error("GetMax should return largest value")
	}

	emptyTree := NewBinaryTree()
	if _, found := emptyTree.GetMin(); found {
		t.Error("GetMin on empty tree should return false")
	}
	if _, found := emptyTree.GetMax(); found {
		t.Error("GetMax on empty tree should return false")
	}
}

func TestBinaryTreeEdgeCases(t *testing.T) {
	tree := NewBinaryTree()

	tree.Insert("")
	if tree.Size() != 0 {
		t.Error("Should not insert empty string")
	}
	if tree.Search("") {
		t.Error("Should not find empty string")
	}
	if tree.Remove("") {
		t.Error("Should not remove empty string")
	}

	tree.Insert("test")
	originalSize := tree.Size()
	tree.Insert("test")
	if tree.Size() != originalSize {
		t.Error("Should not insert duplicates")
	}

	tree.Clear()
	tree.Insert("50")
	tree.Insert("30")
	tree.Insert("70")
	tree.Insert("20")
	tree.Insert("40")
	tree.Insert("60")
	tree.Insert("80")

	if !tree.Remove("50") {
		t.Error("Should remove root with two children")
	}
	if tree.Size() != 6 {
		t.Error("Size should decrease after removal")
	}

	if !tree.Search("60") || !tree.Search("30") || !tree.Search("70") {
		t.Error("Tree structure should be maintained after complex removal")
	}
}

func TestBinaryTreeTraversalEdgeCases(t *testing.T) {
	emptyTree := NewBinaryTree()

	if len(emptyTree.InOrder()) != 0 {
		t.Error("Empty tree should return empty in-order traversal")
	}
	if len(emptyTree.PreOrder()) != 0 {
		t.Error("Empty tree should return empty pre-order traversal")
	}
	if len(emptyTree.PostOrder()) != 0 {
		t.Error("Empty tree should return empty post-order traversal")
	}
	if len(emptyTree.LevelOrder()) != 0 {
		t.Error("Empty tree should return empty level-order traversal")
	}

	singleTree := NewBinaryTree()
	singleTree.Insert("single")

	if len(singleTree.InOrder()) != 1 {
		t.Error("Single element tree should have one element in traversal")
	}
	if singleTree.Height() != 1 {
		t.Error("Single element tree should have height 1")
	}
}

// ==================== COMPREHENSIVE ALL CONTAINERS TESTS ====================

func TestComprehensiveAllContainers(t *testing.T) {
	arr := NewArray()
	sll := NewSinglyLinkedList()
	dll := NewDoublyLinkedList()
	queue := NewQueue()
	stack := NewStack()
	ht := NewHashTable()
	tree := NewBinaryTree()

	for i := 0; i < 10; i++ {
		value := string(rune('a' + i))
		arr.Insert(value)
		sll.PushBack(value)
		dll.PushBack(value)
		queue.Enqueue(value)
		stack.Push(value)
		ht.Insert(i, value)
		tree.Insert(value)
	}

	if arr.Size() != 10 {
		t.Error("Array size should be 10")
	}
	if sll.Size() != 10 {
		t.Error("Singly linked list size should be 10")
	}
	if dll.Size() != 10 {
		t.Error("Doubly linked list size should be 10")
	}
	if queue.Size() != 10 {
		t.Error("Queue size should be 10")
	}
	if stack.Size() != 10 {
		t.Error("Stack size should be 10")
	}
	if ht.Size() != 10 {
		t.Error("Hash table size should be 10")
	}
	if tree.Size() != 10 {
		t.Error("Tree size should be 10")
	}

	containers := []Serializer{arr, sll, dll, queue, stack, ht, tree}
	for i, container := range containers {
		data, err := container.Serialize()
		if err != nil {
			t.Errorf("Container %d: Serialize should not fail: %v", i, err)
		}
		if data == nil {
			t.Errorf("Container %d: Serialize should return data", i)
		}
	}

	val, err := arr.Get(0)
	if err != nil || val != "a" {
		t.Error("Array should work correctly after comprehensive test")
	}

	val, err = sll.Get(0)
	if err != nil || val != "a" {
		t.Error("Singly linked list should work correctly after comprehensive test")
	}

	exists := ht.Contains(0)
	if !exists {
		t.Error("Hash table should work correctly after comprehensive test")
	}

	found := tree.Search("a")
	if !found {
		t.Error("Binary tree should work correctly after comprehensive test")
	}
}

// ==================== ERROR CONDITIONS COMPREHENSIVE TEST ====================

func TestErrorConditionsComprehensive(t *testing.T) {
	arr := NewArray()
	if _, err := arr.Get(0); err == nil {
		t.Error("Should return error for get on empty array")
	}
	if err := arr.Set(0, "test"); err == nil {
		t.Error("Should return error for set on empty array")
	}
	if err := arr.Remove(0); err == nil {
		t.Error("Should return error for remove on empty array")
	}
	if err := arr.InsertAt(1, "test"); err == nil {
		t.Error("Should return error for insert at invalid index")
	}

	sll := NewSinglyLinkedList()
	if _, err := sll.Get(0); err == nil {
		t.Error("Should return error for get on empty list")
	}
	if err := sll.Remove(0); err == nil {
		t.Error("Should return error for remove on empty list")
	}
	if err := sll.Insert(1, "test"); err == nil {
		t.Error("Should return error for insert at invalid index")
	}

	dll := NewDoublyLinkedList()
	if _, err := dll.Get(0); err == nil {
		t.Error("Should return error for get on empty list")
	}
	if err := dll.Remove(0); err == nil {
		t.Error("Should return error for remove on empty list")
	}
	if err := dll.Insert(1, "test"); err == nil {
		t.Error("Should return error for insert at invalid index")
	}
}

// ==================== SERIALIZATION COMPREHENSIVE TESTS ====================

func TestSerializationComprehensive(t *testing.T) {
	arr := NewArray()
	arr.Insert("hello")
	arr.Insert("world")
	arr.Insert("test with spaces")
	arr.Insert("test,with,commas")
	arr.Insert("test\"with\"quotes")

	data, err := arr.Serialize()
	if err != nil {
		t.Errorf("Array serialization should not fail: %v", err)
	}

	newArr := NewArray()
	if err := newArr.Deserialize(data); err != nil {
		t.Errorf("Array deserialization should not fail: %v", err)
	}

	if arr.Size() != newArr.Size() {
		t.Error("Array sizes should match after serialization")
	}

	ht := NewHashTable()
	ht.Insert(0, "zero")
	ht.Insert(100, "hundred")
	ht.Insert(-5, "negative")

	htData, err := ht.Serialize()
	if err != nil {
		t.Errorf("Hash table serialization should not fail: %v", err)
	}

	newHt := NewHashTable()
	if err := newHt.Deserialize(htData); err != nil {
		t.Errorf("Hash table deserialization should not fail: %v", err)
	}

	if ht.Size() != newHt.Size() {
		t.Error("Hash table sizes should match after serialization")
	}

	tree := NewBinaryTree()
	values := []string{"m", "f", "s", "a", "h", "r", "z", "c", "i"}
	for _, v := range values {
		tree.Insert(v)
	}

	treeData, err := tree.Serialize()
	if err != nil {
		t.Errorf("Binary tree serialization should not fail: %v", err)
	}

	newTree := NewBinaryTree()
	if err := newTree.Deserialize(treeData); err != nil {
		t.Errorf("Binary tree deserialization should not fail: %v", err)
	}

	if tree.Size() != newTree.Size() {
		t.Error("Binary tree sizes should match after serialization")
	}
}

// ==================== PERFORMANCE AND STRESS TESTS ====================

func TestStressTestContainers(t *testing.T) {
	const numElements = 1000

	arr := NewArray()
	for i := 0; i < numElements; i++ {
		arr.Insert(string(rune('a' + i%26)))
	}
	if arr.Size() != numElements {
		t.Error("Array should handle many insertions")
	}

	ht := NewHashTable()
	for i := 0; i < numElements; i++ {
		ht.Insert(i, string(rune('a'+i%26)))
	}
	if ht.Size() != numElements {
		t.Error("Hash table should handle many insertions")
	}

	tree := NewBinaryTree()
	for i := 0; i < numElements; i++ {
		tree.Insert(string(rune('a'+i%26)) + strconv.Itoa(i))
	}
	if tree.Size() != numElements {
		t.Error("Binary tree should handle many insertions")
	}
}

// ==================== UTILITY FUNCTION TESTS ====================

func TestUtilityFunctions(t *testing.T) {
	if err := checkIndex(0, 1); err != nil {
		t.Error("checkIndex should allow valid index")
	}
	if err := checkIndex(-1, 1); err == nil {
		t.Error("checkIndex should reject negative index")
	}
	if err := checkIndex(1, 1); err == nil {
		t.Error("checkIndex should reject index equal to size")
	}
	if err := checkIndex(2, 1); err == nil {
		t.Error("checkIndex should reject index greater than size")
	}

	arr := NewArray()
	arr.Insert("test")
	PrintContainer("TestArray", arr)
}

func TestComprehensiveEdgeCases(t *testing.T) {
	arr := NewArrayWithSize(0)
	if arr.Size() != 0 {
		t.Error("Array with size 0 should have size 0")
	}

	dll := NewDoublyLinkedList()
	dll.PushFront("test")
	dll.Clear()
	if !dll.IsEmpty() {
		t.Error("List should be empty after clear")
	}

	ht := NewHashTable()
	ht.Insert(999, "test")
	ht.Clear()
	if !ht.IsEmpty() {
		t.Error("HashTable should be empty after clear")
	}

	if ht.Capacity() != defaultCapacity {
		t.Errorf("HashTable should have default capacity after clear, got %d, expected %d", ht.Capacity(), defaultCapacity)
	}
}

func TestErrorMessages(t *testing.T) {
	if ErrIndexOutOfRange.Error() != "index out of range" {
		t.Error("ErrIndexOutOfRange should have correct message")
	}
	if ErrEmptyContainer.Error() != "container is empty" {
		t.Error("ErrEmptyContainer should have correct message")
	}
}

// ==================== ДОПОЛНИТЕЛЬНЫЕ ТЕСТЫ ДЛЯ ПОКРЫТИЯ ====================

func TestDoublyLinkedListEdgeCaseCoverage(t *testing.T) {
	list := NewDoublyLinkedList()

	// Test PopBack with single element
	list.PushBack("single")
	val, err := list.PopBack()
	if err != nil || val != "single" {
		t.Error("PopBack should work with single element")
	}
	if !list.IsEmpty() {
		t.Error("List should be empty after PopBack single element")
	}

	// Test Insert in the middle with forward traversal
	list.PushBack("a")
	list.PushBack("c")
	err = list.Insert(1, "b")
	if err != nil {
		t.Error("Insert should work with forward traversal")
	}
	if val, _ := list.Get(1); val != "b" {
		t.Error("Insert should place element correctly with forward traversal")
	}

	// Test Remove last element
	list.Clear()
	list.PushBack("x")
	list.PushBack("y")
	err = list.Remove(1)
	if err != nil {
		t.Error("Remove should work on last element")
	}
	if list.Size() != 1 {
		t.Error("Size should be 1 after removing last element")
	}
}

func TestHashTableEdgeCaseCoverage(t *testing.T) {
	// Test hash function with empty buckets
	ht := &HashTable{buckets: nil}
	if ht.hash(5) != 0 {
		t.Error("Hash should return 0 for nil buckets")
	}

	// Test rehash with empty buckets
	ht.rehash()
	if ht.Capacity() != defaultCapacity {
		t.Error("Rehash should initialize buckets with default capacity")
	}

	// Test Remove with collision chain
	ht = NewHashTableWithCapacity(4)
	ht.Insert(1, "val1")
	ht.Insert(5, "val5") // Collision with 1
	ht.Insert(9, "val9") // Collision with 1

	// Remove middle of chain
	if !ht.Remove(5) {
		t.Error("Should remove from middle of collision chain")
	}
	if ht.Size() != 2 {
		t.Error("Size should be 2 after removal from chain")
	}

	// Test GetShortestChain with all empty buckets
	ht.Clear()
	if ht.GetShortestChain() != 0 {
		t.Error("Shortest chain should be 0 for empty table")
	}
}

func TestBinaryTreePrintCoverage(t *testing.T) {
	tree := NewBinaryTree()

	// Test Print method
	tree.Insert("test")
	tree.Print() // Should not panic

	// Test empty tree Print
	emptyTree := NewBinaryTree()
	emptyTree.Print() // Should not panic
}

func TestSinglyLinkedListEdgeCaseCoverage(t *testing.T) {
	list := NewSinglyLinkedList()

	// Test Insert in middle with proper traversal
	list.PushBack("a")
	list.PushBack("c")
	err := list.Insert(1, "b")
	if err != nil {
		t.Error("Insert should work in middle")
	}
	if val, _ := list.Get(1); val != "b" {
		t.Error("Insert should place element correctly in middle")
	}

	// Test Remove updating tail
	list.Clear()
	list.PushBack("first")
	list.PushBack("second")
	list.PushBack("third")

	err = list.Remove(2)
	if err != nil {
		t.Error("Remove should work on tail")
	}
	if list.Size() != 2 {
		t.Error("Size should be 2 after removing tail")
	}
}

func TestQueueStackPrintCoverage(t *testing.T) {
	// Test Queue Print
	queue := NewQueue()
	queue.Enqueue("test")
	queue.Print() // Should not panic

	// Test Stack Print
	stack := NewStack()
	stack.Push("test")
	stack.Print() // Should not panic

	// Test empty Print methods
	emptyQueue := NewQueue()
	emptyQueue.Print()

	emptyStack := NewStack()
	emptyStack.Print()
}

func TestHashTablePrintCoverage(t *testing.T) {
	ht := NewHashTable()
	ht.Insert(1, "test")
	ht.Print() // Should not panic

	// Test empty hash table print
	emptyHt := NewHashTable()
	emptyHt.Print()
}

func TestBinaryTreeDeserializeError(t *testing.T) {
	tree := NewBinaryTree()

	// Test deserialize with invalid JSON
	invalidData := []byte("invalid json")
	err := tree.Deserialize(invalidData)
	if err == nil {
		t.Error("Deserialize should return error for invalid JSON")
	}
}

func TestLinkedListDeserializeErrors(t *testing.T) {
	// Test SinglyLinkedList deserialize error
	sll := NewSinglyLinkedList()
	invalidData := []byte("invalid json")
	err := sll.Deserialize(invalidData)
	if err == nil {
		t.Error("SinglyLinkedList Deserialize should return error for invalid JSON")
	}

	// Test DoublyLinkedList deserialize error
	dll := NewDoublyLinkedList()
	err = dll.Deserialize(invalidData)
	if err == nil {
		t.Error("DoublyLinkedList Deserialize should return error for invalid JSON")
	}
}

func TestArrayDeserializeError(t *testing.T) {
	arr := NewArray()
	invalidData := []byte("invalid json")
	err := arr.Deserialize(invalidData)
	if err == nil {
		t.Error("Array Deserialize should return error for invalid JSON")
	}
}

func TestHashTableDeserializeErrors(t *testing.T) {
	ht := NewHashTable()

	// Test invalid JSON
	invalidData := []byte("invalid json")
	err := ht.Deserialize(invalidData)
	if err == nil {
		t.Error("HashTable Deserialize should return error for invalid JSON")
	}

	// Test invalid key format
	invalidKeyData := []byte(`{"not_a_number": "value"}`)
	err = ht.Deserialize(invalidKeyData)
	if err == nil {
		t.Error("HashTable Deserialize should return error for invalid key format")
	}
}

// Добавить в конец файла containers_test.go

// ==================== БИНАРНАЯ СЕРИАЛИЗАЦИЯ TESTS ====================

func TestArrayBinarySerialization(t *testing.T) {
	arr := NewArray()
	arr.Insert("test1")
	arr.Insert("test2")
	arr.Insert("test3")

	// Тестируем бинарную сериализацию
	data, err := arr.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newArr := NewArray()
	err = newArr.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, arr.Size(), newArr.Size())
	for i := 0; i < arr.Size(); i++ {
		val1, _ := arr.Get(i)
		val2, _ := newArr.Get(i)
		assert.Equal(t, val1, val2)
	}
}

func TestSinglyLinkedListBinarySerialization(t *testing.T) {
	list := NewSinglyLinkedList()
	list.PushBack("a")
	list.PushBack("b")
	list.PushBack("c")

	data, err := list.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newList := NewSinglyLinkedList()
	err = newList.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, list.Size(), newList.Size())
	for i := 0; i < list.Size(); i++ {
		val1, _ := list.Get(i)
		val2, _ := newList.Get(i)
		assert.Equal(t, val1, val2)
	}
}

func TestDoublyLinkedListBinarySerialization(t *testing.T) {
	list := NewDoublyLinkedList()
	list.PushBack("x")
	list.PushBack("y")
	list.PushBack("z")

	data, err := list.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newList := NewDoublyLinkedList()
	err = newList.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, list.Size(), newList.Size())
	for i := 0; i < list.Size(); i++ {
		val1, _ := list.Get(i)
		val2, _ := newList.Get(i)
		assert.Equal(t, val1, val2)
	}
}

func TestQueueBinarySerialization(t *testing.T) {
	queue := NewQueue()
	queue.Enqueue("q1")
	queue.Enqueue("q2")
	queue.Enqueue("q3")

	data, err := queue.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newQueue := NewQueue()
	err = newQueue.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, queue.Size(), newQueue.Size())
	for !queue.IsEmpty() {
		val1, _ := queue.Dequeue()
		val2, _ := newQueue.Dequeue()
		assert.Equal(t, val1, val2)
	}
}

func TestStackBinarySerialization(t *testing.T) {
	stack := NewStack()
	stack.Push("s1")
	stack.Push("s2")
	stack.Push("s3")

	data, err := stack.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newStack := NewStack()
	err = newStack.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, stack.Size(), newStack.Size())
	for !stack.IsEmpty() {
		val1, _ := stack.Pop()
		val2, _ := newStack.Pop()
		assert.Equal(t, val1, val2)
	}
}

func TestHashTableBinarySerialization(t *testing.T) {
	ht := NewHashTable()
	ht.Insert(1, "one")
	ht.Insert(2, "two")
	ht.Insert(3, "three")

	data, err := ht.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newHt := NewHashTable()
	err = newHt.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, ht.Size(), newHt.Size())
	for i := 1; i <= 3; i++ {
		val1, exists1 := ht.Get(i)
		val2, exists2 := newHt.Get(i)
		assert.Equal(t, exists1, exists2)
		assert.Equal(t, val1, val2)
	}
}

func TestBinaryTreeBinarySerialization(t *testing.T) {
	tree := NewBinaryTree()
	tree.Insert("m")
	tree.Insert("f")
	tree.Insert("s")
	tree.Insert("a")
	tree.Insert("h")

	data, err := tree.SerializeBinary()
	require.NoError(t, err)
	require.NotEmpty(t, data)

	newTree := NewBinaryTree()
	err = newTree.DeserializeBinary(data)
	require.NoError(t, err)

	assert.Equal(t, tree.Size(), newTree.Size())

	inOrder1 := tree.InOrder()
	inOrder2 := newTree.InOrder()
	assert.Equal(t, inOrder1, inOrder2)
}

func TestUniversalSerializationInterfaces(t *testing.T) {
	// Проверяем что все контейнеры реализуют интерфейсы
	var _ UniversalContainer = NewArray()
	var _ UniversalContainer = NewSinglyLinkedList()
	var _ UniversalContainer = NewDoublyLinkedList()
	var _ UniversalContainer = NewQueue()
	var _ UniversalContainer = NewStack()
	var _ UniversalContainer = NewHashTable()
	var _ UniversalContainer = NewBinaryTree()
}

func TestSerializationFormatComparison(t *testing.T) {
	arr := NewArray()
	for i := 0; i < 10; i++ {
		arr.Insert(fmt.Sprintf("value%d", i))
	}

	jsonData, err := arr.Serialize()
	require.NoError(t, err)

	binData, err := arr.SerializeBinary()
	require.NoError(t, err)

	// Обычно бинарная сериализация дает меньший размер для сложных структур
	t.Logf("JSON size: %d bytes", len(jsonData))
	t.Logf("Binary size: %d bytes", len(binData))
	t.Logf("Ratio: %.2f", float64(len(jsonData))/float64(len(binData)))
}

func TestBinarySerializationErrorHandling(t *testing.T) {
	arr := NewArray()

	// Тестируем обработку ошибок невалидных данных
	invalidData := []byte("invalid binary data")
	err := arr.DeserializeBinary(invalidData)
	assert.Error(t, err)

	// Тестируем пустые данные
	emptyData := []byte{}
	err = arr.DeserializeBinary(emptyData)
	assert.Error(t, err)
}
