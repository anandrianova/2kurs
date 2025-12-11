package main

import (
	"fmt"
)

// Структура динамического массива
type Array struct {
	data   []string
	length int
}

// Создание нового массива
func createArray(initialCapacity ...int) *Array {
	capacity := 10 // емкость по умолчанию
	if len(initialCapacity) > 0 {
		capacity = initialCapacity[0]
	}

	return &Array{
		data:   make([]string, capacity),
		length: 0,
	}
}

// Добавление элемента в конец
func arrayPushBack(arr *Array, value string) {
	if arr.length >= len(arr.data) {
		// Увеличиваем емкость в 2 раза
		newCapacity := len(arr.data) * 2
		if newCapacity == 0 {
			newCapacity = 1
		}
		newData := make([]string, newCapacity)
		copy(newData, arr.data)
		arr.data = newData
	}

	arr.data[arr.length] = value
	arr.length++
}

// Вставка элемента по индексу
func arrayInsert(arr *Array, index int, value string) bool {
	if index < 0 || index > arr.length {
		return false
	}

	if arr.length >= len(arr.data) {
		// Увеличиваем емкость
		newCapacity := len(arr.data) * 2
		if newCapacity == 0 {
			newCapacity = 1
		}
		newData := make([]string, newCapacity)
		copy(newData, arr.data)
		arr.data = newData
	}

	// Сдвигаем элементы вправо
	for i := arr.length; i > index; i-- {
		arr.data[i] = arr.data[i-1]
	}

	arr.data[index] = value
	arr.length++
	return true
}

// Получение элемента по индексу
func arrayGet(arr *Array, index int) string {
	if index < 0 || index >= arr.length {
		return ""
	}
	return arr.data[index]
}

// Удаление элемента по индексу
func arrayRemove(arr *Array, index int) bool {
	if index < 0 || index >= arr.length {
		return false
	}

	// Сдвигаем элементы влево
	for i := index; i < arr.length-1; i++ {
		arr.data[i] = arr.data[i+1]
	}

	arr.length--
	arr.data[arr.length] = "" // Очищаем последний элемент

	// Уменьшаем емкость если нужно
	if arr.length > 0 && arr.length <= len(arr.data)/4 {
		newCapacity := len(arr.data) / 2
		newData := make([]string, newCapacity)
		copy(newData, arr.data)
		arr.data = newData
	}

	return true
}

// Замена элемента по индексу
func arrayReplace(arr *Array, index int, value string) bool {
	if index < 0 || index >= arr.length {
		return false
	}

	arr.data[index] = value
	return true
}

// Получение длины массива
func arrayLength(arr *Array) int {
	return arr.length
}

// Вывод массива
func arrayPrint(arr *Array) {
	fmt.Print("ARRAY: [")
	for i := 0; i < arr.length; i++ {
		fmt.Print(arr.data[i])
		if i < arr.length-1 {
			fmt.Print(", ")
		}
	}
	fmt.Println("]")
}

// Освобождение памяти
func arrayFree(arr *Array) {
	arr.data = nil
	arr.length = 0
}
