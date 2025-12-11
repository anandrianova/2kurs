# Компилятор и флаги компиляции
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Имя исполняемого файла
TARGET = db_interface

# Исходные файлы (соответствуют вашим именам файлов)
SOURCES = main.cpp Array.cpp SingleList.cpp DoubleList.cpp Stack.cpp Queue.cpp FBT.cpp DB.cpp

# Заголовочные файлы
HEADERS = Array.h SingleList.h DoubleList.h Stack.h Queue.h FBT.h DB.h

# Правило сборки основной программы
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Очистка скомпилированных файлов
clean:
	rm -f $(TARGET) database.txt

# Запуск тестовой программы
test: $(TARGET)
	./$(TARGET)

# Объявление псевдоцелей
.PHONY: clean test