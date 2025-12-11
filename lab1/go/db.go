package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// Структура для хранения всех контейнеров
type Database struct {
	arrays      map[string]*Array
	singlyLists map[string]*SingleList
	doublyLists map[string]*DoubleList
	stacks      map[string]*Stack
	queues      map[string]*Queue
	trees       map[string]*FullBinaryTree
}

// Создание и управление базой данных
func createDatabase() *Database {
	return &Database{
		arrays:      make(map[string]*Array),
		singlyLists: make(map[string]*SingleList),
		doublyLists: make(map[string]*DoubleList),
		stacks:      make(map[string]*Stack),
		queues:      make(map[string]*Queue),
		trees:       make(map[string]*FullBinaryTree),
	}
}

// освобождение памяти
func freeDatabase(db *Database) {
	// Освобождаем массивы
	for _, arr := range db.arrays {
		arrayFree(arr)
	}

	// Освобождаем односвязные списки
	for _, list := range db.singlyLists {
		sllFree(list)
	}

	// Освобождаем двусвязные списки
	for _, list := range db.doublyLists {
		dllFree(list)
	}

	// Освобождаем стеки
	for _, stack := range db.stacks {
		stackFree(stack)
	}

	// Освобождаем очереди
	for _, queue := range db.queues {
		queueFree(queue)
	}

	// Освобождаем деревья
	for _, tree := range db.trees {
		treeFree(tree)
	}
}

// сохранение в бд
func saveDatabase(db *Database, filename string) bool {
	file, err := os.Create(filename)
	if err != nil {
		return false
	}
	defer file.Close()

	// Сохраняем массивы
	for name, arr := range db.arrays {
		file.WriteString(fmt.Sprintf("ARRAY %s %d ", name, arrayLength(arr)))
		for i := 0; i < arrayLength(arr); i++ {
			file.WriteString(fmt.Sprintf("%s ", arrayGet(arr, i)))
		}
		file.WriteString("\n")
	}

	// Сохраняем односвязные списки
	for name, list := range db.singlyLists {
		file.WriteString(fmt.Sprintf("SINGLY_LIST %s %d ", name, sllSize(list)))
		current := list.head
		for current != nil {
			file.WriteString(fmt.Sprintf("%s ", current.data))
			current = current.next
		}
		file.WriteString("\n")
	}

	// Сохраняем двусвязные списки
	for name, list := range db.doublyLists {
		file.WriteString(fmt.Sprintf("DOUBLY_LIST %s %d ", name, dllSize(list)))
		current := list.head
		for current != nil {
			file.WriteString(fmt.Sprintf("%s ", current.data))
			current = current.next
		}
		file.WriteString("\n")
	}

	// Сохраняем стеки
	for name, stack := range db.stacks {
		file.WriteString(fmt.Sprintf("STACK %s %d ", name, stackSize(stack)))
		// Для стека сохраняем в обратном порядке
		var elements []string

		// Копируем стек
		copyStack := createStack(stack.capacity)
		copyStack.top = stack.top
		for i := 0; i <= stack.top; i++ {
			copyStack.data[i] = stack.data[i]
		}

		for !stackIsEmpty(copyStack) {
			elements = append(elements, stackPop(copyStack))
		}

		for i := len(elements) - 1; i >= 0; i-- {
			file.WriteString(fmt.Sprintf("%s ", elements[i]))
		}
		stackFree(copyStack)
		file.WriteString("\n")
	}

	// Сохраняем очереди
	for name, queue := range db.queues {
		file.WriteString(fmt.Sprintf("QUEUE %s %d ", name, queueSize(queue)))
		for i := 0; i < queueSize(queue); i++ {
			index := (queue.front + i) % queue.capacity
			file.WriteString(fmt.Sprintf("%s ", queue.data[index]))
		}
		file.WriteString("\n")
	}

	// Сохраняем деревья
	for name, tree := range db.trees {
		file.WriteString(fmt.Sprintf("TREE %s %d ", name, treeSize(tree)))

		// Сохраняем дерево в порядке уровней
		if tree.root != nil {
			queue := []*TreeNode{tree.root}

			for len(queue) > 0 {
				current := queue[0]
				queue = queue[1:]

				file.WriteString(fmt.Sprintf("%d %s ", current.key, current.value))

				if current.left != nil {
					queue = append(queue, current.left)
				}
				if current.right != nil {
					queue = append(queue, current.right)
				}
			}
		}
		file.WriteString("\n")
	}

	return true
}

func loadDatabase(db *Database, filename string) bool {
	file, err := os.Open(filename)
	if err != nil {
		return false
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		fields := strings.Fields(line)
		if len(fields) < 3 {
			continue
		}

		typeName := fields[0]
		name := fields[1]
		size, err := strconv.Atoi(fields[2])
		if err != nil {
			continue
		}

		switch typeName {
		case "ARRAY":
			arr := createArray(size)
			for i := 0; i < size && i+3 < len(fields); i++ {
				arrayPushBack(arr, fields[i+3])
			}
			db.arrays[name] = arr
		case "SINGLY_LIST":
			list := createSinglyLinkedList()
			for i := 0; i < size && i+3 < len(fields); i++ {
				sllPushBack(list, fields[i+3])
			}
			db.singlyLists[name] = list
		case "DOUBLY_LIST":
			list := createDoublyLinkedList()
			for i := 0; i < size && i+3 < len(fields); i++ {
				dllPushBack(list, fields[i+3])
			}
			db.doublyLists[name] = list
		case "STACK":
			stack := createStack(size)
			for i := 0; i < size && i+3 < len(fields); i++ {
				stackPush(stack, fields[i+3])
			}
			db.stacks[name] = stack
		case "QUEUE":
			queue := createQueue(size)
			for i := 0; i < size && i+3 < len(fields); i++ {
				queuePush(queue, fields[i+3])
			}
			db.queues[name] = queue
		case "TREE":
			tree := createFullBinaryTree()
			for i := 0; i < size && i+5 < len(fields); i += 2 {
				key, err1 := strconv.Atoi(fields[i+3])
				if err1 != nil {
					continue
				}
				value := fields[i+4]
				treeInsert(tree, key, value)
			}
			db.trees[name] = tree
		}
	}

	return true
}

// Вспомогательные функции для разбора команд
func splitCommand(command string) []string {
	return strings.Fields(command)
}

// Командный интерфейс
func executeCommand(db *Database, command string) string {
	tokens := splitCommand(command)

	if len(tokens) == 0 {
		return "ERROR: Empty command"
	}

	cmd := tokens[0]

	// Команда PRINT для любого контейнера
	if cmd == "PRINT" {
		if len(tokens) < 2 {
			return "ERROR: PRINT requires container name"
		}

		containerName := tokens[1]

		// Проверяем все типы контейнеров
		if arr, exists := db.arrays[containerName]; exists {
			arrayPrint(arr)
			return "SUCCESS"
		} else if list, exists := db.singlyLists[containerName]; exists {
			sllPrintForward(list)
			return "SUCCESS"
		} else if list, exists := db.doublyLists[containerName]; exists {
			dllPrintForward(list)
			return "SUCCESS"
		} else if stack, exists := db.stacks[containerName]; exists {
			stackPrint(stack)
			return "SUCCESS"
		} else if queue, exists := db.queues[containerName]; exists {
			queuePrint(queue)
			return "SUCCESS"
		} else if tree, exists := db.trees[containerName]; exists {
			treePrint(tree)
			return "SUCCESS"
		} else {
			return "ERROR: Container not found: " + containerName
		}
	}

	// Обработка команд для массивов (M)
	if len(cmd) > 0 && cmd[0] == 'M' {
		if len(tokens) < 2 {
			return "ERROR: Array command requires container name"
		}

		arrayName := tokens[1]

		switch cmd {
		case "MCREATE":
			if _, exists := db.arrays[arrayName]; exists {
				return "ERROR: Array already exists: " + arrayName
			}
			db.arrays[arrayName] = createArray()
			return "SUCCESS: Array created: " + arrayName
		case "MPUSH":
			if len(tokens) < 3 {
				return "ERROR: MPUSH requires value"
			}
			if arr, exists := db.arrays[arrayName]; exists {
				arrayPushBack(arr, tokens[2])
				return "SUCCESS: Value pushed to array"
			} else {
				return "ERROR: Array not found: " + arrayName
			}
		case "MINSERT":
			if len(tokens) < 4 {
				return "ERROR: MINSERT requires index and value"
			}
			if arr, exists := db.arrays[arrayName]; exists {
				index, err := strconv.Atoi(tokens[2])
				if err != nil {
					return "ERROR: Invalid index"
				}
				if arrayInsert(arr, index, tokens[3]) {
					return "SUCCESS: Value inserted at index " + tokens[2]
				} else {
					return "ERROR: Invalid index"
				}
			} else {
				return "ERROR: Array not found: " + arrayName
			}
		case "MGET":
			if len(tokens) < 3 {
				return "ERROR: MGET requires index"
			}
			if arr, exists := db.arrays[arrayName]; exists {
				index, err := strconv.Atoi(tokens[2])
				if err != nil {
					return "ERROR: Invalid index"
				}
				value := arrayGet(arr, index)
				if value != "" {
					return "VALUE: " + value
				} else {
					return "ERROR: Invalid index or empty value"
				}
			} else {
				return "ERROR: Array not found: " + arrayName
			}
		case "MDEL":
			if len(tokens) < 3 {
				return "ERROR: MDEL requires index"
			}
			if arr, exists := db.arrays[arrayName]; exists {
				index, err := strconv.Atoi(tokens[2])
				if err != nil {
					return "ERROR: Invalid index"
				}
				if arrayRemove(arr, index) {
					return "SUCCESS: Element removed at index " + tokens[2]
				} else {
					return "ERROR: Invalid index"
				}
			} else {
				return "ERROR: Array not found: " + arrayName
			}
		case "MREPLACE":
			if len(tokens) < 4 {
				return "ERROR: MREPLACE requires index and value"
			}
			if arr, exists := db.arrays[arrayName]; exists {
				index, err := strconv.Atoi(tokens[2])
				if err != nil {
					return "ERROR: Invalid index"
				}
				if arrayReplace(arr, index, tokens[3]) {
					return "SUCCESS: Value replaced at index " + tokens[2]
				} else {
					return "ERROR: Invalid index"
				}
			} else {
				return "ERROR: Array not found: " + arrayName
			}
		case "MSIZE":
			if arr, exists := db.arrays[arrayName]; exists {
				return "SIZE: " + strconv.Itoa(arrayLength(arr))
			} else {
				return "ERROR: Array not found: " + arrayName
			}
		}
	}

	// Обработка команд для односвязных списков (F)
	if len(cmd) > 0 && cmd[0] == 'F' {
		if len(tokens) < 2 {
			return "ERROR: Singly list command requires container name"
		}

		listName := tokens[1]

		switch cmd {
		case "FCREATE":
			if _, exists := db.singlyLists[listName]; exists {
				return "ERROR: Singly list already exists: " + listName
			}
			db.singlyLists[listName] = createSinglyLinkedList()
			return "SUCCESS: Singly list created: " + listName
		case "FPUSH":
			if len(tokens) < 4 {
				return "ERROR: FPUSH requires type and value"
			}
			if list, exists := db.singlyLists[listName]; exists {
				pushType := tokens[2]

				switch pushType {
				case "FRONT":
					value := tokens[3]
					sllPushFront(list, value)
					return "SUCCESS: Value pushed to front"
				case "BACK":
					value := tokens[3]
					sllPushBack(list, value)
					return "SUCCESS: Value pushed to back"
				case "BEFORE":
					if len(tokens) < 5 {
						return "ERROR: FPUSH BEFORE requires target value"
					}
					target := tokens[3]
					value := tokens[4]
					if sllInsertBefore(list, target, value) {
						return "SUCCESS: Value inserted before target"
					} else {
						return "ERROR: Target not found"
					}
				case "AFTER":
					if len(tokens) < 5 {
						return "ERROR: FPUSH AFTER requires target value"
					}
					target := tokens[3]
					value := tokens[4]
					if sllInsertAfter(list, target, value) {
						return "SUCCESS: Value inserted after target"
					} else {
						return "ERROR: Target not found"
					}
				default:
					return "ERROR: Invalid push type. Use FRONT/BACK/BEFORE/AFTER"
				}
			} else {
				return "ERROR: Singly list not found: " + listName
			}
		case "FDEL":
			if len(tokens) < 3 {
				return "ERROR: FDEL requires type"
			}
			if list, exists := db.singlyLists[listName]; exists {
				delType := tokens[2]

				switch delType {
				case "FRONT":
					if sllPopFront(list) {
						return "SUCCESS: Front element removed"
					} else {
						return "ERROR: List is empty"
					}
				case "BACK":
					if sllPopBack(list) {
						return "SUCCESS: Back element removed"
					} else {
						return "ERROR: List is empty"
					}
				case "VALUE":
					if len(tokens) < 4 {
						return "ERROR: FDEL VALUE requires target value"
					}
					if sllRemoveValue(list, tokens[3]) {
						return "SUCCESS: Value removed"
					} else {
						return "ERROR: Value not found"
					}
				case "BEFORE":
					if len(tokens) < 4 {
						return "ERROR: FDEL BEFORE requires target value"
					}
					if sllRemoveBefore(list, tokens[3]) {
						return "SUCCESS: Element before target removed"
					} else {
						return "ERROR: Cannot remove before target"
					}
				case "AFTER":
					if len(tokens) < 4 {
						return "ERROR: FDEL AFTER requires target value"
					}
					if sllRemoveAfter(list, tokens[3]) {
						return "SUCCESS: Element after target removed"
					} else {
						return "ERROR: Cannot remove after target"
					}
				default:
					return "ERROR: Invalid delete type. Use FRONT/BACK/VALUE/BEFORE/AFTER"
				}
			} else {
				return "ERROR: Singly list not found: " + listName
			}
		case "FGET":
			if list, exists := db.singlyLists[listName]; exists {
				if len(tokens) == 2 {
					// Возвращаем весь список
					result := "LIST: "
					current := list.head
					for current != nil {
						result += current.data
						if current.next != nil {
							result += " -> "
						}
						current = current.next
					}
					return result
				} else if len(tokens) == 3 {
					// Поиск элемента
					found := sllFind(list, tokens[2])
					if found != nil {
						return "FOUND: " + found.data
					} else {
						return "NOT_FOUND"
					}
				} else {
					return "ERROR: FGET requires either no arguments (to display list) or one argument (to search)"
				}
			} else {
				return "ERROR: Singly list not found: " + listName
			}
		case "FSIZE":
			if list, exists := db.singlyLists[listName]; exists {
				return "SIZE: " + strconv.Itoa(sllSize(list))
			} else {
				return "ERROR: Singly list not found: " + listName
			}
		case "FPRINT_BACKWARD":
			if list, exists := db.singlyLists[listName]; exists {
				sllPrintBackward(list)
				return "SUCCESS"
			} else {
				return "ERROR: Singly list not found: " + listName
			}
		}
	}

	// Обработка команд для двусвязных списков (L)
	if len(cmd) > 0 && cmd[0] == 'L' {
		if len(tokens) < 2 {
			return "ERROR: Doubly list command requires container name"
		}

		listName := tokens[1]

		switch cmd {
		case "LCREATE":
			if _, exists := db.doublyLists[listName]; exists {
				return "ERROR: Doubly list already exists: " + listName
			}
			db.doublyLists[listName] = createDoublyLinkedList()
			return "SUCCESS: Doubly list created: " + listName
		case "LPUSH":
			if len(tokens) < 4 {
				return "ERROR: LPUSH requires type and value"
			}
			if list, exists := db.doublyLists[listName]; exists {
				pushType := tokens[2]
				value := tokens[3]

				switch pushType {
				case "FRONT":
					dllPushFront(list, value)
					return "SUCCESS: Value pushed to front"
				case "BACK":
					dllPushBack(list, value)
					return "SUCCESS: Value pushed to back"
				case "BEFORE":
					if len(tokens) < 5 {
						return "ERROR: LPUSH BEFORE requires target value"
					}
					target := tokens[3]
					value := tokens[4]
					if dllInsertBefore(list, target, value) {
						return "SUCCESS: Value inserted before target"
					} else {
						return "ERROR: Target not found"
					}
				case "AFTER":
					if len(tokens) < 5 {
						return "ERROR: LPUSH AFTER requires target value"
					}
					target := tokens[3]
					value := tokens[4]
					if dllInsertAfter(list, target, value) {
						return "SUCCESS: Value inserted after target"
					} else {
						return "ERROR: Target not found"
					}
				default:
					return "ERROR: Invalid push type. Use FRONT/BACK/BEFORE/AFTER"
				}
			} else {
				return "ERROR: Doubly list not found: " + listName
			}
		case "LDEL":
			if len(tokens) < 3 {
				return "ERROR: LDEL requires type"
			}
			if list, exists := db.doublyLists[listName]; exists {
				delType := tokens[2]

				switch delType {
				case "FRONT":
					if dllPopFront(list) {
						return "SUCCESS: Front element removed"
					} else {
						return "ERROR: List is empty"
					}
				case "BACK":
					if dllPopBack(list) {
						return "SUCCESS: Back element removed"
					} else {
						return "ERROR: List is empty"
					}
				case "VALUE":
					if len(tokens) < 4 {
						return "ERROR: LDEL VALUE requires target value"
					}
					if dllRemoveValue(list, tokens[3]) {
						return "SUCCESS: Value removed"
					} else {
						return "ERROR: Value not found"
					}
				case "BEFORE":
					if len(tokens) < 4 {
						return "ERROR: LDEL BEFORE requires target value"
					}
					if dllRemoveBefore(list, tokens[3]) {
						return "SUCCESS: Value removed"
					} else {
						return "ERROR: Value not found"
					}
				case "AFTER":
					if len(tokens) < 4 {
						return "ERROR: LDEL AFTER requires target value"
					}
					if dllRemoveAfter(list, tokens[3]) {
						return "SUCCESS: Value removed"
					} else {
						return "ERROR: Value not found"
					}
				default:
					return "ERROR: Invalid delete type. Use FRONT/BACK/VALUE"
				}
			} else {
				return "ERROR: Doubly list not found: " + listName
			}
		case "LGET":
			if list, exists := db.doublyLists[listName]; exists {
				if len(tokens) == 2 {
					// Возвращаем весь список
					result := "LIST: "
					current := list.head
					for current != nil {
						result += current.data
						if current.next != nil {
							result += " <-> "
						}
						current = current.next
					}
					return result
				} else if len(tokens) == 3 {
					// Поиск элемента
					found := dllFind(list, tokens[2])
					if found != nil {
						return "FOUND: " + found.data
					} else {
						return "NOT_FOUND"
					}
				}
			} else {
				return "ERROR: Doubly list not found: " + listName
			}
		case "LPRINT_BACKWARD":
			if list, exists := db.doublyLists[listName]; exists {
				dllPrintBackward(list)
				return "SUCCESS"
			} else {
				return "ERROR: Doubly list not found: " + listName
			}
		case "LSIZE":
			if list, exists := db.doublyLists[listName]; exists {
				return "SIZE: " + strconv.Itoa(dllSize(list))
			} else {
				return "ERROR: Doubly list not found: " + listName
			}
		}
	}

	// Обработка команд для стеков (S)
	if len(cmd) > 0 && cmd[0] == 'S' {
		if len(tokens) < 2 {
			return "ERROR: Stack command requires container name"
		}

		stackName := tokens[1]

		switch cmd {
		case "SCREATE":
			if _, exists := db.stacks[stackName]; exists {
				return "ERROR: Stack already exists: " + stackName
			}
			db.stacks[stackName] = createStack()
			return "SUCCESS: Stack created: " + stackName
		case "SPUSH":
			if len(tokens) < 3 {
				return "ERROR: SPUSH requires value"
			}
			if stack, exists := db.stacks[stackName]; exists {
				stackPush(stack, tokens[2])
				return "SUCCESS: Value pushed to stack"
			} else {
				return "ERROR: Stack not found: " + stackName
			}
		case "SPOP":
			if stack, exists := db.stacks[stackName]; exists {
				value := stackPop(stack)
				if value != "" {
					return "POPPED: " + value
				} else {
					return "ERROR: Stack is empty"
				}
			} else {
				return "ERROR: Stack not found: " + stackName
			}
		case "SPEEK":
			if stack, exists := db.stacks[stackName]; exists {
				value := stackPeek(stack)
				if value != "" {
					return "PEEK: " + value
				} else {
					return "ERROR: Stack is empty"
				}
			} else {
				return "ERROR: Stack not found: " + stackName
			}
		case "SSIZE":
			if stack, exists := db.stacks[stackName]; exists {
				return "SIZE: " + strconv.Itoa(stackSize(stack))
			} else {
				return "ERROR: Stack not found: " + stackName
			}
		}
	}

	// Обработка команд для очередей (Q)
	if len(cmd) > 0 && cmd[0] == 'Q' {
		if len(tokens) < 2 {
			return "ERROR: Queue command requires container name"
		}

		queueName := tokens[1]

		switch cmd {
		case "QCREATE":
			if _, exists := db.queues[queueName]; exists {
				return "ERROR: Queue already exists: " + queueName
			}
			db.queues[queueName] = createQueue()
			return "SUCCESS: Queue created: " + queueName
		case "QPUSH":
			if len(tokens) < 3 {
				return "ERROR: QPUSH requires value"
			}
			if queue, exists := db.queues[queueName]; exists {
				queuePush(queue, tokens[2])
				return "SUCCESS: Value pushed to queue"
			} else {
				return "ERROR: Queue not found: " + queueName
			}
		case "QPOP":
			if queue, exists := db.queues[queueName]; exists {
				value := queuePop(queue)
				if value != "" {
					return "POPPED: " + value
				} else {
					return "ERROR: Queue is empty"
				}
			} else {
				return "ERROR: Queue not found: " + queueName
			}
		case "QPEEK":
			if queue, exists := db.queues[queueName]; exists {
				value := queuePeek(queue)
				if value != "" {
					return "PEEK: " + value
				} else {
					return "ERROR: Queue is empty"
				}
			} else {
				return "ERROR: Queue not found: " + queueName
			}
		case "QSIZE":
			if queue, exists := db.queues[queueName]; exists {
				return "SIZE: " + strconv.Itoa(queueSize(queue))
			} else {
				return "ERROR: Queue not found: " + queueName
			}
		}
	}

	// Обработка команд для деревьев (T)
	if len(cmd) > 0 && cmd[0] == 'T' {
		if len(tokens) < 2 {
			return "ERROR: Tree command requires container name"
		}

		treeName := tokens[1]

		switch cmd {
		case "TCREATE":
			if _, exists := db.trees[treeName]; exists {
				return "ERROR: Tree already exists: " + treeName
			}
			db.trees[treeName] = createFullBinaryTree()
			return "SUCCESS: Tree created: " + treeName
		case "TINSERT":
			if len(tokens) < 4 {
				return "ERROR: TINSERT requires key and value"
			}
			if tree, exists := db.trees[treeName]; exists {
				key, err := strconv.Atoi(tokens[2])
				if err != nil {
					return "ERROR: Invalid key format"
				}

				if treeInsert(tree, key, tokens[3]) {
					return "SUCCESS: Value inserted with key " + tokens[2]
				} else {
					return "ERROR: Failed to insert value"
				}
			} else {
				return "ERROR: Tree not found: " + treeName
			}
		case "TSEARCH":
			if len(tokens) < 3 {
				return "ERROR: TSEARCH requires key"
			}
			if tree, exists := db.trees[treeName]; exists {
				key, err := strconv.Atoi(tokens[2])
				if err != nil {
					return "ERROR: Invalid key format"
				}

				value := treeSearch(tree, key)
				if value != "" {
					return "FOUND: " + value
				} else {
					return "NOT_FOUND"
				}
			} else {
				return "ERROR: Tree not found: " + treeName
			}
		case "TISFULL":
			if tree, exists := db.trees[treeName]; exists {
				isFull := isFullBinaryTree(tree)
				if isFull {
					return "IS_FULL: YES"
				} else {
					return "IS_FULL: NO"
				}
			} else {
				return "ERROR: Tree not found: " + treeName
			}
		case "THEIGHT":
			if tree, exists := db.trees[treeName]; exists {
				height := treeHeight(tree.root)
				return "HEIGHT: " + strconv.Itoa(height)
			} else {
				return "ERROR: Tree not found: " + treeName
			}
		case "TSIZE":
			if tree, exists := db.trees[treeName]; exists {
				return "SIZE: " + strconv.Itoa(treeSize(tree))
			} else {
				return "ERROR: Tree not found: " + treeName
			}
		case "TTRAVERSE":
			if len(tokens) < 3 {
				return "ERROR: TTRAVERSE requires type (INORDER/PREORDER/POSTORDER/LEVEL)"
			}
			if tree, exists := db.trees[treeName]; exists {
				traverseType := tokens[2]

				switch traverseType {
				case "INORDER":
					treeInorder(tree)
					return "SUCCESS"
				case "PREORDER":
					treePreorder(tree)
					return "SUCCESS"
				case "POSTORDER":
					treePostorder(tree)
					return "SUCCESS"
				case "LEVEL":
					treeLevelOrder(tree)
					return "SUCCESS"
				default:
					return "ERROR: Invalid traverse type. Use INORDER/PREORDER/POSTORDER/LEVEL"
				}
			} else {
				return "ERROR: Tree not found: " + treeName
			}
		}
	}

	// Команды управления базой данных
	switch cmd {
	case "SAVE", "save":
		if len(tokens) < 2 {
			return "ERROR: SAVE requires filename"
		}
		if saveDatabase(db, tokens[1]) {
			return "SUCCESS: Database saved to " + tokens[1]
		} else {
			return "ERROR: Failed to save database"
		}
	case "LOAD", "load":
		if len(tokens) < 2 {
			return "ERROR: LOAD requires filename"
		}
		if loadDatabase(db, tokens[1]) {
			return "SUCCESS: Database loaded from " + tokens[1]
		} else {
			return "ERROR: Failed to load database"
		}
	case "HELP", "help":
		return `COMMANDS:
PRINT <container> - Print any container
MCREATE <имя> - Create array
MPUSH <имя> <значение> - Push to array
MINSERT <имя> <индекс> <значение> - Insert into array
MREPLACE <имя> <индекс> <значение> - Replace element at index
MGET <имя> <индекс> - Get from array
MDEL <имя> <индекс> - Delete from array
FCREATE <имя> - Create singly list
FPUSH <имя> <тип> <target> <value> - Push to list (types: FRONT/BACK/BEFORE/AFTER)
FDEL <имя> <тип> [значение] - Delete from list
FGET <имя> <индекс> - Get from list
LCREATE <имя> - Create doubly list
LPUSH <имя> <тип> <target> <value> - Push to list (types: FRONT/BACK/BEFORE/AFTER)
LDEL <имя> <тип> [значение] - Delete from list
LGET <имя> <индекс> - Get from list
SCREATE <имя> - Create stack
SPUSH <имя> <значение> - Push to stack
SPOP <имя> - Pop from stack
QCREATE <имя> - Create queue
QPUSH <имя> <значение> - Push to queue
QPOP <имя> - Pop from queue
TCREATE <имя> - Create full binary tree
TINSERT <имя> <ключ> <значение> - Insert into tree
TSEARCH <имя> <ключ> - Search in tree
TISFULL <имя> - Check if tree is full
TTRAVERSE <имя> <тип> - Traverse tree
SAVE <filename> - Save database
LOAD <filename> - Load database
HELP - Show this help`
	}

	// Обработка команд с префиксом
	if len(tokens) >= 3 {
		containerName := tokens[1]

		// Проверяем все типы контейнеров для команд SIZE
		if cmd == "SIZE" {
			if _, exists := db.arrays[containerName]; exists {
				return "SIZE: " + strconv.Itoa(arrayLength(db.arrays[containerName]))
			} else if _, exists := db.singlyLists[containerName]; exists {
				return "SIZE: " + strconv.Itoa(sllSize(db.singlyLists[containerName]))
			} else if _, exists := db.doublyLists[containerName]; exists {
				return "SIZE: " + strconv.Itoa(dllSize(db.doublyLists[containerName]))
			} else if _, exists := db.stacks[containerName]; exists {
				return "SIZE: " + strconv.Itoa(stackSize(db.stacks[containerName]))
			} else if _, exists := db.queues[containerName]; exists {
				return "SIZE: " + strconv.Itoa(queueSize(db.queues[containerName]))
			} else if _, exists := db.trees[containerName]; exists {
				return "SIZE: " + strconv.Itoa(treeSize(db.trees[containerName]))
			} else {
				return "ERROR: Container not found: " + containerName
			}
		}
	}

	return "ERROR: Unknown command: " + cmd
}
