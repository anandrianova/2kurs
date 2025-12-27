package main

import (
	"encoding/json"
	"fmt"
)

// Array представляет динамический массив строк
type Array struct {
	data []string
	size int
}

// NewArray создает новый пустой массив
func NewArray() *Array {
	return &Array{
		data: make([]string, 0),
		size: 0,
	}
}

// NewArrayWithSize создает массив заданного размера
func NewArrayWithSize(size int) *Array {
	if size < 0 {
		size = 0
	}
	arr := &Array{
		data: make([]string, size),
		size: size,
	}
	for i := range arr.data {
		arr.data[i] = ""
	}
	return arr
}

// Insert добавляет элемент в конец массива
func (a *Array) Insert(value string) {
	a.data = append(a.data, value)
	a.size++
}

// InsertAt вставляет элемент по указанному индексу
func (a *Array) InsertAt(index int, value string) error {
	if index < 0 || index > a.size {
		return ErrIndexOutOfRange
	}

	if index == a.size {
		a.data = append(a.data, value)
	} else {
		a.data = append(a.data[:index+1], a.data[index:]...)
		a.data[index] = value
	}
	a.size++
	return nil
}

// Remove удаляет элемент по индексу
func (a *Array) Remove(index int) error {
	if err := checkIndex(index, a.size); err != nil {
		return err
	}

	a.data = append(a.data[:index], a.data[index+1:]...)
	a.size--
	return nil
}

// RemoveValue удаляет первое вхождение значения
func (a *Array) RemoveValue(value string) bool {
	for i := 0; i < a.size; i++ {
		if a.data[i] == value {
			err := a.Remove(i)
			return err == nil
		}
	}
	return false
}

// Get возвращает элемент по индексу
func (a *Array) Get(index int) (string, error) {
	if err := checkIndex(index, a.size); err != nil {
		return "", err
	}
	return a.data[index], nil
}

// Set устанавливает значение элемента по индексу
func (a *Array) Set(index int, value string) error {
	if err := checkIndex(index, a.size); err != nil {
		return err
	}
	a.data[index] = value
	return nil
}

// IndexOf возвращает индекс первого вхождения значения
func (a *Array) IndexOf(value string) int {
	for i, v := range a.data {
		if v == value {
			return i
		}
	}
	return -1
}

// Contains проверяет наличие значения в массиве
func (a *Array) Contains(value string) bool {
	return a.IndexOf(value) != -1
}

// Size возвращает количество элементов
func (a *Array) Size() int {
	return a.size
}

// IsEmpty проверяет пуст ли массив
func (a *Array) IsEmpty() bool {
	return a.size == 0
}

// Clear очищает массив
func (a *Array) Clear() {
	a.data = make([]string, 0)
	a.size = 0
}

// Values возвращает все элементы массива
func (a *Array) Values() []string {
	result := make([]string, a.size)
	copy(result, a.data)
	return result
}

// Print выводит массив в stdout
func (a *Array) Print() {
	fmt.Printf("Array(size=%d): %v\n", a.size, a.data)
}

// Serialize сериализует массив в JSON
func (a *Array) Serialize() ([]byte, error) {
	return json.Marshal(a.data)
}

// Deserialize десериализует массив из JSON
func (a *Array) Deserialize(data []byte) error {
	var items []string
	if err := json.Unmarshal(data, &items); err != nil {
		return err
	}

	a.data = items
	a.size = len(items)
	return nil
}

// SerializeBinary сериализует массив в бинарный формат
func (a *Array) SerializeBinary() ([]byte, error) {
	type ArrayData struct {
		Size int
		Data []string
	}

	arrayData := ArrayData{
		Size: a.size,
		Data: a.data,
	}

	return serializeToBinary(arrayData)
}

// DeserializeBinary десериализует массив из бинарного формата
func (a *Array) DeserializeBinary(data []byte) error {
	type ArrayData struct {
		Size int
		Data []string
	}

	var arrayData ArrayData
	if err := deserializeFromBinary(data, &arrayData); err != nil {
		return err
	}

	a.data = arrayData.Data
	a.size = arrayData.Size
	return nil
}
