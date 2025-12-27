package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func main() {
	// Флаги командной строки
	fileFlag := flag.String("file", "", "Файл с командами для выполнения")
	queryFlag := flag.String("query", "", "Одиночная команда для выполнения")

	flag.Parse()

	cli := NewCLI()

	// Режим выполнения одиночной команды
	if *queryFlag != "" {
		parts := strings.Fields(*queryFlag)
		if len(parts) == 0 {
			fmt.Println("❌ Пустая команда")
			os.Exit(1)
		}

		command := strings.ToUpper(parts[0])
		args := parts[1:]

		if err := cli.executeCommand(command, args); err != nil {
			fmt.Printf("❌ Ошибка: %v\n", err)
			os.Exit(1)
		}
		return
	}

	// Режим выполнения из файла
	if *fileFlag != "" {
		if err := cli.executeFromFile(*fileFlag); err != nil {
			fmt.Printf("❌ Ошибка выполнения из файла: %v\n", err)
			os.Exit(1)
		}
		return
	}

	// Интерактивный режим (по умолчанию)
	fmt.Println("🚀 Go Containers CLI")
	fmt.Println("====================")
	fmt.Println("Введите HELP для списка команд или EXIT для выхода")

	cli.Run()
}

// CLI представляет интерфейс командной строки для работы с контейнерами
type CLI struct {
	containers map[string]UniversalContainer
	scanner    *bufio.Scanner
}

// NewCLI создает новый CLI
func NewCLI() *CLI {
	return &CLI{
		containers: make(map[string]UniversalContainer),
		scanner:    bufio.NewScanner(os.Stdin),
	}
}

// Run запускает интерактивный режим CLI
func (c *CLI) Run() {
	for {
		fmt.Print("> ")
		if !c.scanner.Scan() {
			break
		}

		input := strings.TrimSpace(c.scanner.Text())
		if input == "" {
			continue
		}

		if input == "EXIT" || input == "QUIT" {
			fmt.Println("👋 До свидания!")
			break
		}

		if input == "HELP" {
			c.printHelp()
			continue
		}

		if input == "LIST" {
			c.listContainers()
			continue
		}

		parts := strings.Fields(input)
		command := strings.ToUpper(parts[0])
		args := parts[1:]

		if err := c.executeCommand(command, args); err != nil {
			fmt.Printf("❌ Ошибка: %v\n", err)
		}
	}
}

// executeFromFile выполняет команды из файла
func (c *CLI) executeFromFile(filename string) error {
	file, err := os.Open(filename)
	if err != nil {
		return fmt.Errorf("ошибка открытия файла: %v", err)
	}
	defer func() {
		if closeErr := file.Close(); closeErr != nil {
			fmt.Printf("⚠️ Предупреждение: ошибка закрытия файла: %v\n", closeErr)
		}
	}()

	scanner := bufio.NewScanner(file)
	lineNum := 0

	for scanner.Scan() {
		lineNum++
		line := strings.TrimSpace(scanner.Text())

		// Пропускаем пустые строки и комментарии
		if line == "" || strings.HasPrefix(line, "#") || strings.HasPrefix(line, "//") {
			continue
		}

		parts := strings.Fields(line)
		if len(parts) == 0 {
			continue
		}

		command := strings.ToUpper(parts[0])
		args := parts[1:]

		fmt.Printf("[%d] %s\n", lineNum, line)

		if err := c.executeCommand(command, args); err != nil {
			return fmt.Errorf("ошибка в строке %d: %v", lineNum, err)
		}
	}

	if err := scanner.Err(); err != nil {
		return fmt.Errorf("ошибка чтения файла: %v", err)
	}

	return nil
}

// listContainers показывает все созданные контейнеры
func (c *CLI) listContainers() {
	if len(c.containers) == 0 {
		fmt.Println("📭 Контейнеры не созданы")
		return
	}

	fmt.Println("📦 Созданные контейнеры:")
	for name, container := range c.containers {
		var typeName string
		switch container.(type) {
		case *Array:
			typeName = "Массив"
		case *SinglyLinkedList:
			typeName = "Односвязный список"
		case *DoublyLinkedList:
			typeName = "Двусвязный список"
		case *Stack:
			typeName = "Стек"
		case *Queue:
			typeName = "Очередь"
		case *BinaryTree:
			typeName = "Бинарное дерево"
		case *HashTable:
			typeName = "Хеш-таблица"
		default:
			typeName = "Неизвестный тип"
		}
		fmt.Printf("  📍 %s (%s) - размер: %d\n", name, typeName, container.Size())
	}
}

// executeCommand выполняет команду
func (c *CLI) executeCommand(command string, args []string) error {
	switch command {
	// Создание контейнеров
	case "MCREATE": // Массив
		return c.createArray(args)
	case "FCREATE": // Односвязный список
		return c.createSinglyLinkedList(args)
	case "LCREATE": // Двусвязный список
		return c.createDoublyLinkedList(args)
	case "SCREATE": // Стек
		return c.createStack(args)
	case "QCREATE": // Очередь
		return c.createQueue(args)
	case "TCREATE": // Бинарное дерево
		return c.createBinaryTree(args)
	case "DCREATE": // Хеш-таблица
		return c.createHashTable(args)
	case "CCREATE": // Пользовательский контейнер
		return c.createCustomContainer(args)

	// Операции с массивом
	case "MPUSH":
		return c.arrayPush(args)
	case "MPUSHINOEX":
		return c.arrayPushIfNotExists(args)
	case "MREPLACE":
		return c.arrayReplace(args)
	case "MDEL":
		return c.arrayDelete(args)
	case "MGET":
		return c.arrayGet(args)

	// Операции с односвязным списком
	case "FPUSHHEAD":
		return c.sllPushHead(args)
	case "FPUSHTAIL":
		return c.sllPushTail(args)
	case "FPUSHAFTER":
		return c.sllPushAfter(args)
	case "FPUSHBEFORE":
		return c.sllPushBefore(args)
	case "FDELHEAD":
		return c.sllDeleteHead(args)
	case "FDELTAIL":
		return c.sllDeleteTail(args)
	case "FDELVALUE":
		return c.sllDeleteValue(args)
	case "FGET":
		return c.sllGet(args)

	// Операции с двусвязным списком
	case "LPUSHHEAD":
		return c.dllPushHead(args)
	case "LPUSHTAIL":
		return c.dllPushTail(args)
	case "LPUSHAFTER":
		return c.dllPushAfter(args)
	case "LPUSHBEFORE":
		return c.dllPushBefore(args)
	case "LDELHEAD":
		return c.dllDeleteHead(args)
	case "LDELTAIL":
		return c.dllDeleteTail(args)
	case "LDELVALUE":
		return c.dllDeleteValue(args)
	case "LGET":
		return c.dllGet(args)

	// Операции со стеком
	case "SPUSH":
		return c.stackPush(args)
	case "SPOP":
		return c.stackPop(args)

	// Операции с очередью
	case "QPUSH":
		return c.queuePush(args)
	case "QPOP":
		return c.queuePop(args)

	// Операции с бинарным деревом
	case "TINSERT":
		return c.treeInsert(args)
	case "TDEL":
		return c.treeDelete(args)
	case "TGET":
		return c.treeGet(args)

	// Операции с хеш-таблицей
	case "DINSERT":
		return c.hashTableInsert(args)
	case "DSEARCH":
		return c.hashTableSearch(args)
	case "DREMOVE":
		return c.hashTableRemove(args)

	// Универсальные операции
	case "PRINT":
		return c.printContainer(args)
	case "SIZE":
		return c.containerSize(args)
	case "CLEAR":
		return c.clearContainer(args)
	case "VALUES":
		return c.containerValues(args)
	case "LIST":
		c.listContainers()
		return nil

	default:
		return fmt.Errorf("неизвестная команда: %s. Введите HELP для списка команд", command)
	}
}

// Вспомогательные методы для создания контейнеров
func (c *CLI) createArray(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: MCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewArray()
	fmt.Printf("✅ Массив '%s' создан\n", name)
	return nil
}

func (c *CLI) createSinglyLinkedList(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: FCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewSinglyLinkedList()
	fmt.Printf("✅ Односвязный список '%s' создан\n", name)
	return nil
}

func (c *CLI) createDoublyLinkedList(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: LCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewDoublyLinkedList()
	fmt.Printf("✅ Двусвязный список '%s' создан\n", name)
	return nil
}

func (c *CLI) createStack(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: SCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewStack()
	fmt.Printf("✅ Стек '%s' создан\n", name)
	return nil
}

func (c *CLI) createQueue(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: QCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewQueue()
	fmt.Printf("✅ Очередь '%s' создан\n", name)
	return nil
}

func (c *CLI) createBinaryTree(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: TCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewBinaryTree()
	fmt.Printf("✅ Бинарное дерево '%s' создан\n", name)
	return nil
}

func (c *CLI) createHashTable(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: DCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	c.containers[name] = NewHashTable()
	fmt.Printf("✅ Хеш-таблица '%s' создан\n", name)
	return nil
}

func (c *CLI) createCustomContainer(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: CCREATE <имя>")
	}
	name := args[0]

	if _, exists := c.containers[name]; exists {
		return fmt.Errorf("контейнер с именем '%s' уже существует", name)
	}

	// По умолчанию создаем массив
	c.containers[name] = NewArray()
	fmt.Printf("✅ Контейнер '%s' создан (массив по умолчанию)\n", name)
	return nil
}

// Методы для работы с массивом
func (c *CLI) arrayPush(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: MPUSH <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	arr, ok := container.(*Array)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является массивом", name)
	}

	arr.Insert(value)
	fmt.Printf("✅ Значение '%s' добавлено в массив '%s'\n", value, name)
	return nil
}

func (c *CLI) arrayPushIfNotExists(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: MPUSHINOEX <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	arr, ok := container.(*Array)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является массивом", name)
	}

	if !arr.Contains(value) {
		arr.Insert(value)
		fmt.Printf("✅ Значение '%s' добавлено в массив '%s'\n", value, name)
	} else {
		fmt.Printf("ℹ️ Значение '%s' уже существует в массиве '%s'\n", value, name)
	}
	return nil
}

func (c *CLI) arrayReplace(args []string) error {
	if len(args) != 3 {
		return fmt.Errorf("использование: MREPLACE <имя> <индекс> <значение>")
	}
	name := args[0]
	indexStr := args[1]
	value := args[2]

	index, err := strconv.Atoi(indexStr)
	if err != nil {
		return fmt.Errorf("неверный индекс: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	arr, ok := container.(*Array)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является массивом", name)
	}

	if err := arr.Set(index, value); err != nil {
		return fmt.Errorf("ошибка замены: %v", err)
	}

	fmt.Printf("✅ Элемент [%d] заменен на '%s' в массиве '%s'\n", index, value, name)
	return nil
}

func (c *CLI) arrayDelete(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: MDEL <имя> <индекс>")
	}
	name := args[0]
	indexStr := args[1]

	index, err := strconv.Atoi(indexStr)
	if err != nil {
		return fmt.Errorf("неверный индекс: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	arr, ok := container.(*Array)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является массивом", name)
	}

	if err := arr.Remove(index); err != nil {
		return fmt.Errorf("ошибка удаления: %v", err)
	}

	fmt.Printf("✅ Элемент [%d] удален из массива '%s'\n", index, name)
	return nil
}

func (c *CLI) arrayGet(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: MGET <имя> <индекс>")
	}
	name := args[0]
	indexStr := args[1]

	index, err := strconv.Atoi(indexStr)
	if err != nil {
		return fmt.Errorf("неверный индекс: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	arr, ok := container.(*Array)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является массивом", name)
	}

	value, err := arr.Get(index)
	if err != nil {
		return fmt.Errorf("ошибка получения: %v", err)
	}

	fmt.Printf("✅ Элемент [%d] = '%s'\n", index, value)
	return nil
}

// Методы для работы с односвязным списком
func (c *CLI) sllPushHead(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: FPUSHHEAD <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	sll.PushFront(value)
	fmt.Printf("✅ Значение '%s' добавлено в начало списка '%s'\n", value, name)
	return nil
}

func (c *CLI) sllPushTail(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: FPUSHTAIL <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	sll.PushBack(value)
	fmt.Printf("✅ Значение '%s' добавлено в конец списка '%s'\n", value, name)
	return nil
}

func (c *CLI) sllPushAfter(args []string) error {
	if len(args) != 3 {
		return fmt.Errorf("использование: FPUSHAFTER <имя> <значение> <после>")
	}
	name := args[0]
	value := args[1]
	afterValue := args[2]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	index := sll.IndexOf(afterValue)
	if index == -1 {
		return fmt.Errorf("значение '%s' не найдено в списке", afterValue)
	}

	if err := sll.Insert(index+1, value); err != nil {
		return fmt.Errorf("ошибка вставки: %v", err)
	}

	fmt.Printf("✅ Значение '%s' добавлено после '%s' в список '%s'\n", value, afterValue, name)
	return nil
}

func (c *CLI) sllPushBefore(args []string) error {
	if len(args) != 3 {
		return fmt.Errorf("использование: FPUSHBEFORE <имя> <значение> <перед>")
	}
	name := args[0]
	value := args[1]
	beforeValue := args[2]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	index := sll.IndexOf(beforeValue)
	if index == -1 {
		return fmt.Errorf("значение '%s' не найдено в списке", beforeValue)
	}

	if err := sll.Insert(index, value); err != nil {
		return fmt.Errorf("ошибка вставки: %v", err)
	}

	fmt.Printf("✅ Значение '%s' добавлено перед '%s' в список '%s'\n", value, beforeValue, name)
	return nil
}

func (c *CLI) sllDeleteHead(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: FDELHEAD <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	value, err := sll.PopFront()
	if err != nil {
		return fmt.Errorf("ошибка удаления: %v", err)
	}

	fmt.Printf("✅ Удален первый элемент '%s' из списка '%s'\n", value, name)
	return nil
}

func (c *CLI) sllDeleteTail(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: FDELTAIL <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	value, err := sll.PopBack()
	if err != nil {
		return fmt.Errorf("ошибка удаления: %v", err)
	}

	fmt.Printf("✅ Удален последний элемент '%s' из списка '%s'\n", value, name)
	return nil
}

func (c *CLI) sllDeleteValue(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: FDELVALUE <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	if !sll.RemoveValue(value) {
		return fmt.Errorf("значение '%s' не найдено в списке", value)
	}

	fmt.Printf("✅ Значение '%s' удалено из списка '%s'\n", value, name)
	return nil
}

func (c *CLI) sllGet(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: FGET <имя> <индекс>")
	}
	name := args[0]
	indexStr := args[1]

	index, err := strconv.Atoi(indexStr)
	if err != nil {
		return fmt.Errorf("неверный индекс: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	sll, ok := container.(*SinglyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является односвязным списком", name)
	}

	value, err := sll.Get(index)
	if err != nil {
		return fmt.Errorf("ошибка получения: %v", err)
	}

	fmt.Printf("✅ Элемент [%d] = '%s'\n", index, value)
	return nil
}

// Методы для работы с двусвязным списком
func (c *CLI) dllPushHead(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: LPUSHHEAD <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	dll.PushFront(value)
	fmt.Printf("✅ Значение '%s' добавлено в начало списка '%s'\n", value, name)
	return nil
}

func (c *CLI) dllPushTail(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: LPUSHTAIL <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	dll.PushBack(value)
	fmt.Printf("✅ Значение '%s' добавлено в конец списка '%s'\n", value, name)
	return nil
}

func (c *CLI) dllPushAfter(args []string) error {
	if len(args) != 3 {
		return fmt.Errorf("использование: LPUSHAFTER <имя> <значение> <после>")
	}
	name := args[0]
	value := args[1]
	afterValue := args[2]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	index := dll.IndexOf(afterValue)
	if index == -1 {
		return fmt.Errorf("значение '%s' не найдено в списке", afterValue)
	}

	if err := dll.Insert(index+1, value); err != nil {
		return fmt.Errorf("ошибка вставки: %v", err)
	}

	fmt.Printf("✅ Значение '%s' добавлено после '%s' в список '%s'\n", value, afterValue, name)
	return nil
}

func (c *CLI) dllPushBefore(args []string) error {
	if len(args) != 3 {
		return fmt.Errorf("использование: LPUSHBEFORE <имя> <значение> <перед>")
	}
	name := args[0]
	value := args[1]
	beforeValue := args[2]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	index := dll.IndexOf(beforeValue)
	if index == -1 {
		return fmt.Errorf("значение '%s' не найдено в списке", beforeValue)
	}

	if err := dll.Insert(index, value); err != nil {
		return fmt.Errorf("ошибка вставки: %v", err)
	}

	fmt.Printf("✅ Значение '%s' добавлено перед '%s' в список '%s'\n", value, beforeValue, name)
	return nil
}

func (c *CLI) dllDeleteHead(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: LDELHEAD <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	value, err := dll.PopFront()
	if err != nil {
		return fmt.Errorf("ошибка удаления: %v", err)
	}

	fmt.Printf("✅ Удален первый элемент '%s' из списка '%s'\n", value, name)
	return nil
}

func (c *CLI) dllDeleteTail(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: LDELTAIL <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	value, err := dll.PopBack()
	if err != nil {
		return fmt.Errorf("ошибка удаления: %v", err)
	}

	fmt.Printf("✅ Удален последний элемент '%s' из списка '%s'\n", value, name)
	return nil
}

func (c *CLI) dllDeleteValue(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: LDELVALUE <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	if !dll.RemoveValue(value) {
		return fmt.Errorf("значение '%s' не найдено в списке", value)
	}

	fmt.Printf("✅ Значение '%s' удалено из списка '%s'\n", value, name)
	return nil
}

func (c *CLI) dllGet(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: LGET <имя> <индекс>")
	}
	name := args[0]
	indexStr := args[1]

	index, err := strconv.Atoi(indexStr)
	if err != nil {
		return fmt.Errorf("неверный индекс: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	dll, ok := container.(*DoublyLinkedList)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является двусвязным списком", name)
	}

	value, err := dll.Get(index)
	if err != nil {
		return fmt.Errorf("ошибка получения: %v", err)
	}

	fmt.Printf("✅ Элемент [%d] = '%s'\n", index, value)
	return nil
}

// Методы для работы со стеком
func (c *CLI) stackPush(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: SPUSH <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	stack, ok := container.(*Stack)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является стеком", name)
	}

	stack.Push(value)
	fmt.Printf("✅ Значение '%s' добавлено в стек '%s'\n", value, name)
	return nil
}

func (c *CLI) stackPop(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: SPOP <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	stack, ok := container.(*Stack)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является стеком", name)
	}

	value, err := stack.Pop()
	if err != nil {
		return fmt.Errorf("ошибка извлечения: %v", err)
	}

	fmt.Printf("✅ Извлечено значение '%s' из стека '%s'\n", value, name)
	return nil
}

// Методы для работы с очередью
func (c *CLI) queuePush(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: QPUSH <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	queue, ok := container.(*Queue)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является очередью", name)
	}

	queue.Enqueue(value)
	fmt.Printf("✅ Значение '%s' добавлено в очередь '%s'\n", value, name)
	return nil
}

func (c *CLI) queuePop(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: QPOP <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	queue, ok := container.(*Queue)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является очередью", name)
	}

	value, err := queue.Dequeue()
	if err != nil {
		return fmt.Errorf("ошибка извлечения: %v", err)
	}

	fmt.Printf("✅ Извлечено значение '%s' из очереди '%s'\n", value, name)
	return nil
}

// Методы для работы с бинарным деревом
func (c *CLI) treeInsert(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: TINSERT <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	tree, ok := container.(*BinaryTree)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является бинарным деревом", name)
	}

	tree.Insert(value)
	fmt.Printf("✅ Значение '%s' добавлено в дерево '%s'\n", value, name)
	return nil
}

func (c *CLI) treeDelete(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: TDEL <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	tree, ok := container.(*BinaryTree)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является бинарным деревом", name)
	}

	if !tree.Remove(value) {
		return fmt.Errorf("значение '%s' не найдено в дереве", value)
	}

	fmt.Printf("✅ Значение '%s' удалено из дерева '%s'\n", value, name)
	return nil
}

func (c *CLI) treeGet(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: TGET <имя> <значение>")
	}
	name := args[0]
	value := args[1]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	tree, ok := container.(*BinaryTree)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является бинарным деревом", name)
	}

	found := tree.Search(value)
	if found {
		fmt.Printf("✅ Значение '%s' найдено в дереве '%s'\n", value, name)
	} else {
		fmt.Printf("❌ Значение '%s' не найдено в дереве '%s'\n", value, name)
	}
	return nil
}

// Методы для работы с хеш-таблицей
func (c *CLI) hashTableInsert(args []string) error {
	if len(args) != 3 {
		return fmt.Errorf("использование: DINSERT <имя> <ключ> <значение>")
	}
	name := args[0]
	keyStr := args[1]
	value := args[2]

	key, err := strconv.Atoi(keyStr)
	if err != nil {
		return fmt.Errorf("неверный ключ: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	ht, ok := container.(*HashTable)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является хеш-таблицей", name)
	}

	ht.Insert(key, value)
	fmt.Printf("✅ Пара (%d:'%s') добавлена в хеш-таблицу '%s'\n", key, value, name)
	return nil
}

func (c *CLI) hashTableSearch(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: DSEARCH <имя> <ключ>")
	}
	name := args[0]
	keyStr := args[1]

	key, err := strconv.Atoi(keyStr)
	if err != nil {
		return fmt.Errorf("неверный ключ: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	ht, ok := container.(*HashTable)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является хеш-таблицей", name)
	}

	foundValue, exists := ht.Get(key)
	if exists {
		fmt.Printf("✅ Ключ %d найден: '%s'\n", key, foundValue)
	} else {
		fmt.Printf("❌ Ключ %d не найден в хеш-таблице '%s'\n", key, name)
	}
	return nil
}

func (c *CLI) hashTableRemove(args []string) error {
	if len(args) != 2 {
		return fmt.Errorf("использование: DREMOVE <имя> <ключ>")
	}
	name := args[0]
	keyStr := args[1]

	key, err := strconv.Atoi(keyStr)
	if err != nil {
		return fmt.Errorf("неверный ключ: %v", err)
	}

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	ht, ok := container.(*HashTable)
	if !ok {
		return fmt.Errorf("контейнер '%s' не является хеш-таблицей", name)
	}

	if !ht.Remove(key) {
		return fmt.Errorf("ключ %d не найден в хеш-таблице", key)
	}

	fmt.Printf("✅ Ключ %d удален из хеш-таблицы '%s'\n", key, name)
	return nil
}

// Универсальные методы
func (c *CLI) printContainer(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: PRINT <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	fmt.Printf("📊 Контейнер '%s': ", name)
	container.Print()
	return nil
}

func (c *CLI) containerSize(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: SIZE <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	size := container.Size()
	fmt.Printf("📏 Размер контейнера '%s': %d\n", name, size)
	return nil
}

func (c *CLI) clearContainer(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: CLEAR <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	container.Clear()
	fmt.Printf("🧹 Контейнер '%s' очищен\n", name)
	return nil
}

func (c *CLI) containerValues(args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("использование: VALUES <имя>")
	}
	name := args[0]

	container, ok := c.containers[name]
	if !ok {
		return fmt.Errorf("контейнер '%s' не найден", name)
	}

	var values []string
	switch cont := container.(type) {
	case *Array:
		values = cont.Values()
	case *SinglyLinkedList:
		values = cont.Values()
	case *DoublyLinkedList:
		values = cont.Values()
	case *Stack:
		// Для стека нужно специальное получение значений
		temp := NewStack()
		var stackValues []string
		for !cont.IsEmpty() {
			val, err := cont.Pop()
			if err != nil {
				break
			}
			stackValues = append([]string{val}, stackValues...)
			temp.Push(val)
		}
		// Восстанавливаем стек
		for !temp.IsEmpty() {
			val, _ := temp.Pop()
			cont.Push(val)
		}
		values = stackValues
	case *Queue:
		// Для очереди нужно специальное получение значений
		temp := NewQueue()
		var queueValues []string
		for !cont.IsEmpty() {
			val, err := cont.Dequeue()
			if err != nil {
				break
			}
			queueValues = append(queueValues, val)
			temp.Enqueue(val)
		}
		// Восстанавливаем очередь
		for !temp.IsEmpty() {
			val, _ := temp.Dequeue()
			cont.Enqueue(val)
		}
		values = queueValues
	case *BinaryTree:
		values = cont.InOrder()
	case *HashTable:
		values = cont.Values()
	default:
		return fmt.Errorf("неподдерживаемый тип контейнера")
	}

	fmt.Printf("📋 Значения контейнера '%s': %v\n", name, values)
	return nil
}

// printHelp выводит справку по командам
func (c *CLI) printHelp() {
	fmt.Print(`
Доступные команды:

Создание контейнеров:
  MCREATE <имя>     - Создать массив
  FCREATE <имя>     - Создать односвязный список  
  LCREATE <имя>     - Создать двусвязный список
  SCREATE <имя>     - Создать стек
  QCREATE <имя>     - Создать очередь
  TCREATE <имя>     - Создать бинарное дерево
  DCREATE <имя>     - Создать хеш-таблицу
  CCREATE <имя>     - Создать пользовательский контейнер

Операции с массивом:
  MPUSH <имя> <значение>        - Добавить элемент
  MPUSHINOEX <имя> <значение>   - Добавить если не существует
  MREPLACE <имя> <индекс> <значение> - Заменить элемент
  MDEL <имя> <индекс>           - Удалить элемент
  MGET <имя> <индекс>           - Получить элемент

Операции с односвязным списком:
  FPUSHHEAD <имя> <значение>    - Добавить в начало
  FPUSHTAIL <имя> <значение>    - Добавить в конец
  FPUSHAFTER <имя> <значение> <после> - Добавить после значения
  FPUSHBEFORE <имя> <значение> <перед> - Добавить перед значением
  FDELHEAD <имя>                - Удалить первый элемент
  FDELTAIL <имя>                - Удалить последний элемент
  FDELVALUE <имя> <значение>    - Удалить значение
  FGET <имя> <индекс>           - Получить элемент

Операции с двусвязным списком:
  LPUSHHEAD <имя> <значение>    - Добавить в начало
  LPUSHTAIL <имя> <значение>    - Добавить в конец
  LPUSHAFTER <имя> <значение> <после> - Добавить после значения
  LPUSHBEFORE <имя> <значение> <перед> - Добавить перед значением
  LDELHEAD <имя>                - Удалить первый элемент
  LDELTAIL <имя>                - Удалить последний элемент
  LDELVALUE <имя> <значение>    - Удалить значение
  LGET <имя> <индекс>           - Получить элемент

Операции со стеком:
  SPUSH <имя> <значение>        - Добавить в стек
  SPOP <имя>                    - Извлечь из стека

Операции с очередью:
  QPUSH <имя> <значение>        - Добавить в очередь
  QPOP <имя>                    - Извлечь из очереди

Операции с бинарным деревом:
  TINSERT <имя> <значение>      - Вставить значение
  TDEL <имя> <значение>         - Удалить значение
  TGET <имя> <значение>         - Найти значение

Операции с хеш-таблицей:
  DINSERT <имя> <ключ> <значение> - Вставить пару ключ-значение
  DSEARCH <имя> <ключ>          - Найти по ключу
  DREMOVE <имя> <ключ>          - Удалить по ключу

Универсальные операции:
  PRINT <имя>                   - Вывести контейнер
  SIZE <имя>                    - Показать размер
  CLEAR <имя>                   - Очистить контейнер
  VALUES <имя>                  - Показать все значения
  LIST                          - Показать все контейнеры

Системные команды:
  HELP                          - Показать эту справку
  EXIT, QUIT                    - Выход

Пример использования:
  FCREATE mylist
  FPUSHHEAD mylist Alice
  FPUSHTAIL mylist Bob
  PRINT mylist
  SIZE mylist

`)
}
