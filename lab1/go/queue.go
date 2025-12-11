package main

import (
	"fmt"
)

// Структура очереди
type Queue struct {
	data     []string
	front    int
	rear     int
	size     int
	capacity int
}

// Создание новой очереди
func createQueue(initialCapacity ...int) *Queue {
	capacity := 10 // емкость по умолчанию
	if len(initialCapacity) > 0 {
		capacity = initialCapacity[0]
	}

	return &Queue{
		data:     make([]string, capacity),
		front:    0,
		rear:     -1,
		size:     0,
		capacity: capacity,
	}
}

// Добавление элемента в очередь
func queuePush(queue *Queue, value string) {
	if queue.size >= queue.capacity {
		// Увеличиваем емкость
		newCapacity := queue.capacity * 2
		newData := make([]string, newCapacity)

		// Копируем элементы
		for i := 0; i < queue.size; i++ {
			index := (queue.front + i) % queue.capacity
			newData[i] = queue.data[index]
		}

		queue.data = newData
		queue.front = 0
		queue.rear = queue.size - 1
		queue.capacity = newCapacity
	}

	queue.rear = (queue.rear + 1) % queue.capacity
	queue.data[queue.rear] = value
	queue.size++
}

// Извлечение элемента из очереди
func queuePop(queue *Queue) string {
	if queue.size <= 0 {
		return ""
	}

	value := queue.data[queue.front]
	queue.front = (queue.front + 1) % queue.capacity
	queue.size--
	return value
}

// Просмотр первого элемента
func queuePeek(queue *Queue) string {
	if queue.size <= 0 {
		return ""
	}
	return queue.data[queue.front]
}

// Получение размера
func queueSize(queue *Queue) int {
	return queue.size
}

// Вывод очереди
func queuePrint(queue *Queue) {
	fmt.Print("QUEUE: [")
	for i := 0; i < queue.size; i++ {
		index := (queue.front + i) % queue.capacity
		fmt.Print(queue.data[index])
		if i < queue.size-1 {
			fmt.Print(", ")
		}
	}
	fmt.Println("]")
}

// Освобождение памяти
func queueFree(queue *Queue) {
	queue.data = nil
	queue.front = 0
	queue.rear = -1
	queue.size = 0
	queue.capacity = 0
}
