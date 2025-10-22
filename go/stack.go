package main

import (
	"fmt"
)

// Структура стека
type Stack struct {
	data     []string
	top      int
	capacity int
}

// Создание нового стека
func createStack(initialCapacity ...int) *Stack {
	capacity := 10 // емкость по умолчанию
	if len(initialCapacity) > 0 {
		capacity = initialCapacity[0]
	}

	return &Stack{
		data:     make([]string, capacity),
		top:      -1,
		capacity: capacity,
	}
}

// Добавление элемента в стек
func stackPush(stack *Stack, value string) {
	if stack.top >= stack.capacity-1 {
		// Увеличиваем емкость
		newCapacity := stack.capacity * 2
		newData := make([]string, newCapacity)
		copy(newData, stack.data)
		stack.data = newData
		stack.capacity = newCapacity
	}

	stack.top++
	stack.data[stack.top] = value
}

// Извлечение элемента из стека
func stackPop(stack *Stack) string {
	if stack.top < 0 {
		return ""
	}

	value := stack.data[stack.top]
	stack.top--
	return value
}

// Просмотр верхнего элемента
func stackPeek(stack *Stack) string {
	if stack.top < 0 {
		return ""
	}
	return stack.data[stack.top]
}

// Проверка на пустоту
func stackIsEmpty(stack *Stack) bool {
	return stack.top < 0
}

// Получение размера
func stackSize(stack *Stack) int {
	return stack.top + 1
}

// Вывод стека
func stackPrint(stack *Stack) {
	fmt.Print("STACK: [")
	for i := stack.top; i >= 0; i-- {
		fmt.Print(stack.data[i])
		if i > 0 {
			fmt.Print(", ")
		}
	}
	fmt.Println("]")
}

// Освобождение памяти
func stackFree(stack *Stack) {
	stack.data = nil
	stack.top = -1
	stack.capacity = 0
}
