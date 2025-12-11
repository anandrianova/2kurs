package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	db := createDatabase()
	filename := "database.txt"

	fmt.Println("=== Data Structures Database Interface ===")
	fmt.Println("Type 'HELP' for available commands")
	fmt.Println("Type 'EXIT' to quit")
	fmt.Println()

	// Автозагрузка базы данных при запуске
	if loadDatabase(db, filename) {
		fmt.Printf("Database loaded from %s\n", filename)
	}

	scanner := bufio.NewScanner(os.Stdin)
	for {
		fmt.Print("> ")
		if !scanner.Scan() {
			break
		}

		command := strings.TrimSpace(scanner.Text())

		if command == "EXIT" || command == "exit" {
			break
		}

		if command != "" {
			result := executeCommand(db, command)
			fmt.Println(result)

			// Автосохранение после каждой команды
			saveDatabase(db, filename)
		}
	}

	// Финальное сохранение
	saveDatabase(db, filename)
	freeDatabase(db)

	fmt.Printf("Database saved to %s\n", filename)
	fmt.Println("Goodbye!")
}
