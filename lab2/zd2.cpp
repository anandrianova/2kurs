#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// Структура для элемента множества
struct SetElement {
    string value;        // Значение элемента
    SetElement* next;    // Указатель на следующий элемент
};

// Структура множества на основе связного списка
struct Set {
    SetElement* head;    // Указатель на первый элемент
    int size;           // Количество элементов в множестве
};

// Создание пустого множества
Set* create_set() {
    Set* set = new Set;
    set->head = nullptr;
    set->size = 0;
    return set;
}

// Добавление элемента в множество (SETADD)
bool SETADD(Set* set, const string& value) {
    // Проверяем, нет ли уже такого элемента в множестве
    SetElement* current = set->head;
    while (current != nullptr) {
        if (current->value == value) {
            return false;  // Элемент уже существует
        }
        current = current->next;
    }
    
    // Создаем новый элемент
    SetElement* new_element = new SetElement;
    new_element->value = value;
    new_element->next = set->head;  // Добавляем в начало списка
    set->head = new_element;
    set->size++;
    
    return true;
}

// Удаление элемента из множества (SETDEL)
bool SETDEL(Set* set, const string& value) {
    SetElement* current = set->head;
    SetElement* prev = nullptr;
    
    // Ищем элемент для удаления
    while (current != nullptr) {
        if (current->value == value) {
            // Нашли элемент для удаления
            if (prev == nullptr) {
                // Удаляем первый элемент
                set->head = current->next;
            } else {
                // Удаляем элемент из середины или конца
                prev->next = current->next;
            }
            
            delete current;
            set->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;  // Элемент не найден
}

// Проверка наличия элемента в множестве (SET_AT)
bool SET_AT(const Set* set, const string& value) {
    SetElement* current = set->head;
    
    while (current != nullptr) {
        if (current->value == value) {
            return true;  // Элемент найден
        }
        current = current->next;
    }
    
    return false;  // Элемент не найден
}

// Вывод множества
void print_set(const Set* set) {
    if (set->head == nullptr) {
        cout << "Множество пусто" << endl;
        return;
    }
    
    cout << "Множество [" << set->size << "]: {";
    SetElement* current = set->head;
    while (current != nullptr) {
        cout << current->value;
        if (current->next != nullptr) {
            cout << ", ";
        }
        current = current->next;
    }
    cout << "}" << endl;
}

// Загрузка множества из файла
bool load_set_from_file(Set* set, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось открыть файл " << filename << endl;
        return false;
    }
    
    string value;
    int loaded_count = 0;
    
    while (file >> value) {
        if (SETADD(set, value)) {
            loaded_count++;
        }
    }
    
    file.close();
    cout << "Загружено " << loaded_count << " элементов из файла " << filename << endl;
    return true;
}

// Сохранение множества в файл
bool save_set_to_file(const Set* set, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось создать файл " << filename << endl;
        return false;
    }
    
    SetElement* current = set->head;
    int saved_count = 0;
    
    while (current != nullptr) {
        file << current->value << endl;
        current = current->next;
        saved_count++;
    }
    
    file.close();
    cout << "Сохранено " << saved_count << " элементов в файл " << filename << endl;
    return true;
}

// Освобождение памяти множества
void free_set(Set* set) {
    SetElement* current = set->head;
    while (current != nullptr) {
        SetElement* temp = current;
        current = current->next;
        delete temp;
    }
    delete set;
}

// Функция для вывода справки
void print_help() {
    cout << "Использование:" << endl;
    cout << "  ./zd2 --file <файл> --query <команда>" << endl;
    cout << endl;
    cout << "Команды:" << endl;
    cout << "  SETADD <значение>    - добавить элемент в множество" << endl;
    cout << "  SETDEL <значение>    - удалить элемент из множества" << endl;
    cout << "  SET_AT <значение>    - проверить наличие элемента" << endl;
    cout << "  PRINT                - вывести все элементы множества" << endl;
    cout << "  SAVE                 - сохранить множество в файл" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  ./zd2 --file data.txt --query \"SETADD apple\"" << endl;
    cout << "  ./zd2 --file data.txt --query \"SET_AT banana\"" << endl;
    cout << "  ./zd2 --file data.txt --query \"PRINT\"" << endl;
}

// Функция для разбора аргументов командной строки
bool parse_arguments(int argc, char* argv[], string& filename, string& query) {
    filename = "";
    query = "";
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "--file" && i + 1 < argc) {
            filename = argv[++i];
        }
        else if (arg == "--query" && i + 1 < argc) {
            query = argv[++i];
        }
        else if (arg == "--help" || arg == "-h") {
            print_help();
            return false;
        }
    }
    
    if (filename.empty() || query.empty()) {
        cout << "Ошибка: Необходимо указать оба параметра --file и --query" << endl;
        cout << "Используйте --help для справки" << endl;
        return false;
    }
    
    return true;
}

// Функция для выполнения запроса
void execute_query(Set* set, const string& query, const string& filename) {
    // Разбиваем запрос на команду и аргумент
    string command;
    string value;
    
    size_t space_pos = query.find(' ');
    if (space_pos != string::npos) {
        command = query.substr(0, space_pos);
        value = query.substr(space_pos + 1);
    } else {
        command = query;
    }
    
    if (command == "SETADD") {
        if (value.empty()) {
            cout << "Ошибка: SETADD требует значение" << endl;
            return;
        }
        if (SETADD(set, value)) {
            cout << "Элемент '" << value << "' добавлен в множество" << endl;
            save_set_to_file(set, filename); // Автосохранение
        } else {
            cout << "Элемент '" << value << "' уже существует в множестве" << endl;
        }
    }
    else if (command == "SETDEL") {
        if (value.empty()) {
            cout << "Ошибка: SETDEL требует значение" << endl;
            return;
        }
        if (SETDEL(set, value)) {
            cout << "Элемент '" << value << "' удален из множества" << endl;
            save_set_to_file(set, filename); // Автосохранение
        } else {
            cout << "Элемент '" << value << "' не найден в множестве" << endl;
        }
    }
    else if (command == "SET_AT") {
        if (value.empty()) {
            cout << "Ошибка: SET_AT требует значение" << endl;
            return;
        }
        if (SET_AT(set, value)) {
            cout << "Элемент '" << value << "' присутствует в множестве" << endl;
        } else {
            cout << "Элемент '" << value << "' отсутствует в множестве" << endl;
        }
    }
    else if (command == "PRINT") {
        print_set(set);
    }
    else if (command == "SAVE") {
        if (save_set_to_file(set, filename)) {
            cout << "Множество сохранено в файл " << filename << endl;
        }
    }
    else {
        cout << "Ошибка: Неизвестная команда '" << command << "'" << endl;
        cout << "Доступные команды: SETADD, SETDEL, SET_AT, PRINT, SAVE" << endl;
    }
}

int main(int argc, char* argv[]) {
    string filename, query;
    
    // Разбор аргументов командной строки
    if (!parse_arguments(argc, argv, filename, query)) {
        return 1;
    }
    
    // Создание множества
    Set* mySet = create_set();
    
    // Загрузка данных из файла
    if (!load_set_from_file(mySet, filename)) {
        cout << "Создан новый файл " << filename << endl;
    }
    
    // Выполнение запроса
    execute_query(mySet, query, filename);
    
    // Освобождение памяти
    free_set(mySet);
    
    return 0;
}