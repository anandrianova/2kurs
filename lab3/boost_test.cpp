#define BOOST_TEST_MODULE DataStructuresTests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <random>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>

#include "Array.h"
#include "SingleList.h"
#include "DoubleList.h"
#include "Stack.h"
#include "Queue.h"
#include "FullBinaryTree.h"
#include "HashTable.h"
#include "DB.h"

using namespace std;
namespace fs = boost::filesystem;
namespace bt = boost::unit_test;

// Вспомогательные функции
string generateRandomString(int length = 10) {
    static const char alphanum[] = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static mt19937 rng(random_device{}());
    static uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);
    
    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += alphanum[dist(rng)];
    }
    return result;
}

void cleanupTestFiles() {
    vector<string> files = {
        "test_array.bin", "test_array.txt",
        "test_slist.bin", "test_slist.txt",
        "test_dlist.bin", "test_dlist.txt",
        "test_stack.bin", "test_stack.txt",
        "test_queue.bin", "test_queue.txt",
        "test_tree.bin", "test_tree.txt",
        "test_dhash.bin", "test_dhash.txt",
        "test_db.txt", "test_db.bin",
        "integration_test.txt", "perf_test.bin",
        "coverage_test.bin", "empty.txt",
        "corrupted.bin", "corrupted.txt"
    };
    
    for (const auto& file : files) {
        if (fs::exists(file)) {
            fs::remove(file);
        }
    }
}

// Фикстура для очистки тестовых файлов
struct CleanupFixture {
    CleanupFixture() {
        cleanupTestFiles();
    }
    
    ~CleanupFixture() {
        cleanupTestFiles();
    }
};

BOOST_GLOBAL_FIXTURE(CleanupFixture);

// ==================== Array Tests ====================
BOOST_AUTO_TEST_SUITE(ArrayTests)

BOOST_AUTO_TEST_CASE(DefaultConstructor) {
    Array arr;
    BOOST_CHECK_EQUAL(arr.length(), 0);
}

BOOST_AUTO_TEST_CASE(ConstructorWithCapacity) {
    Array arr(20);
    BOOST_CHECK_EQUAL(arr.length(), 0);
    
    // Проверяем, что можно добавить больше элементов
    for (int i = 0; i < 30; ++i) {
        arr.push_back("test");
    }
    BOOST_CHECK_EQUAL(arr.length(), 30);
}

BOOST_AUTO_TEST_CASE(CopyConstructor) {
    Array arr1;
    arr1.push_back("test1");
    arr1.push_back("test2");
    
    Array arr2(arr1);
    BOOST_CHECK_EQUAL(arr2.length(), 2);
    BOOST_CHECK_EQUAL(arr2.get(0), "test1");
    BOOST_CHECK_EQUAL(arr2.get(1), "test2");
}

BOOST_AUTO_TEST_CASE(AssignmentOperator) {
    Array arr1;
    arr1.push_back("A");
    arr1.push_back("B");
    
    Array arr2;
    arr2 = arr1;
    BOOST_CHECK_EQUAL(arr2.length(), 2);
    BOOST_CHECK_EQUAL(arr2.get(0), "A");
    BOOST_CHECK_EQUAL(arr2.get(1), "B");
    
    // Self-assignment
    arr2 = arr2;
    BOOST_CHECK_EQUAL(arr2.length(), 2);
    BOOST_CHECK_EQUAL(arr2.get(0), "A");
}

BOOST_AUTO_TEST_CASE(MoveSemantics) {
    Array arr1;
    arr1.push_back("item1");
    arr1.push_back("item2");
    
    int size_before = arr1.length();
    
    // Move constructor
    Array arr2(std::move(arr1));
    BOOST_CHECK_EQUAL(arr2.length(), size_before);
    BOOST_CHECK_EQUAL(arr2.get(0), "item1");
    BOOST_CHECK_EQUAL(arr1.length(), 0); // Проверяем, что исходный объект пуст
    
    // Move assignment
    Array arr3;
    arr3 = std::move(arr2);
    BOOST_CHECK_EQUAL(arr3.length(), size_before);
    BOOST_CHECK_EQUAL(arr3.get(1), "item2");
}

BOOST_AUTO_TEST_CASE(PushBackBasic) {
    Array arr;
    arr.push_back("first");
    arr.push_back("second");
    arr.push_back("third");
    
    BOOST_CHECK_EQUAL(arr.length(), 3);
    BOOST_CHECK_EQUAL(arr.get(0), "first");
    BOOST_CHECK_EQUAL(arr.get(1), "second");
    BOOST_CHECK_EQUAL(arr.get(2), "third");
}

BOOST_AUTO_TEST_CASE(PushBackWithResize) {
    Array arr(2); // Маленькая начальная емкость
    arr.push_back("1");
    arr.push_back("2");
    arr.push_back("3"); // Должен вызвать resize
    
    BOOST_CHECK_EQUAL(arr.length(), 3);
    BOOST_CHECK_EQUAL(arr.get(2), "3");
}

BOOST_AUTO_TEST_CASE(InsertValidPositions) {
    Array arr;
    arr.push_back("A");
    arr.push_back("C");
    
    // Вставка в середину
    BOOST_CHECK(arr.insert(1, "B"));
    BOOST_CHECK_EQUAL(arr.length(), 3);
    BOOST_CHECK_EQUAL(arr.get(0), "A");
    BOOST_CHECK_EQUAL(arr.get(1), "B");
    BOOST_CHECK_EQUAL(arr.get(2), "C");
    
    // Вставка в начало
    BOOST_CHECK(arr.insert(0, "Start"));
    BOOST_CHECK_EQUAL(arr.get(0), "Start");
    BOOST_CHECK_EQUAL(arr.length(), 4);
    
    // Вставка в конец
    BOOST_CHECK(arr.insert(arr.length(), "End"));
    BOOST_CHECK_EQUAL(arr.get(arr.length() - 1), "End");
    BOOST_CHECK_EQUAL(arr.length(), 5);
}

BOOST_AUTO_TEST_CASE(InsertInvalidPositions) {
    Array arr;
    arr.push_back("A");
    
    BOOST_CHECK(!arr.insert(-1, "invalid"));
    BOOST_CHECK(!arr.insert(5, "invalid"));
    BOOST_CHECK_EQUAL(arr.length(), 1);
}

BOOST_AUTO_TEST_CASE(InsertWithResize) {
    Array arr(2);
    arr.push_back("A");
    arr.push_back("C");
    
    BOOST_CHECK(arr.insert(1, "B")); // Должен вызвать resize
    BOOST_CHECK_EQUAL(arr.length(), 3);
    BOOST_CHECK_EQUAL(arr.get(1), "B");
}

BOOST_AUTO_TEST_CASE(GetValidIndex) {
    Array arr;
    arr.push_back("first");
    arr.push_back("second");
    arr.push_back("third");
    
    BOOST_CHECK_EQUAL(arr.get(0), "first");
    BOOST_CHECK_EQUAL(arr.get(1), "second");
    BOOST_CHECK_EQUAL(arr.get(2), "third");
}

BOOST_AUTO_TEST_CASE(GetInvalidIndex) {
    Array arr;
    arr.push_back("test");
    
    BOOST_CHECK_EQUAL(arr.get(-1), "");
    BOOST_CHECK_EQUAL(arr.get(5), "");
    BOOST_CHECK_EQUAL(arr.get(1), ""); // За пределами размера
}

BOOST_AUTO_TEST_CASE(GetEmptyArray) {
    Array arr;
    BOOST_CHECK_EQUAL(arr.get(0), "");
    BOOST_CHECK_EQUAL(arr.get(-1), "");
    BOOST_CHECK_EQUAL(arr.get(10), "");
}

BOOST_AUTO_TEST_CASE(RemoveValid) {
    Array arr;
    arr.push_back("A");
    arr.push_back("B");
    arr.push_back("C");
    
    BOOST_CHECK(arr.remove(1));
    BOOST_CHECK_EQUAL(arr.length(), 2);
    BOOST_CHECK_EQUAL(arr.get(0), "A");
    BOOST_CHECK_EQUAL(arr.get(1), "C");
}

BOOST_AUTO_TEST_CASE(RemoveFirst) {
    Array arr;
    arr.push_back("A");
    arr.push_back("B");
    
    BOOST_CHECK(arr.remove(0));
    BOOST_CHECK_EQUAL(arr.length(), 1);
    BOOST_CHECK_EQUAL(arr.get(0), "B");
}

BOOST_AUTO_TEST_CASE(RemoveLast) {
    Array arr;
    arr.push_back("A");
    arr.push_back("B");
    arr.push_back("C");
    
    BOOST_CHECK(arr.remove(2));
    BOOST_CHECK_EQUAL(arr.length(), 2);
    BOOST_CHECK_EQUAL(arr.get(0), "A");
    BOOST_CHECK_EQUAL(arr.get(1), "B");
}

BOOST_AUTO_TEST_CASE(RemoveInvalid) {
    Array arr;
    arr.push_back("A");
    
    BOOST_CHECK(!arr.remove(-1));
    BOOST_CHECK(!arr.remove(5));
    BOOST_CHECK_EQUAL(arr.length(), 1);
}

BOOST_AUTO_TEST_CASE(RemoveFromEmpty) {
    Array arr;
    BOOST_CHECK(!arr.remove(0));
    BOOST_CHECK(!arr.remove(-1));
    BOOST_CHECK(!arr.remove(1));
}

BOOST_AUTO_TEST_CASE(ReplaceValid) {
    Array arr;
    arr.push_back("old");
    arr.push_back("middle");
    arr.push_back("end");
    
    BOOST_CHECK(arr.replace(1, "new"));
    BOOST_CHECK_EQUAL(arr.get(1), "new");
    BOOST_CHECK_EQUAL(arr.length(), 3);
}

BOOST_AUTO_TEST_CASE(ReplaceInvalid) {
    Array arr;
    arr.push_back("test");
    
    BOOST_CHECK(!arr.replace(-1, "invalid"));
    BOOST_CHECK(!arr.replace(5, "invalid"));
    BOOST_CHECK_EQUAL(arr.get(0), "test");
}

BOOST_AUTO_TEST_CASE(ReplaceAllElements) {
    Array arr;
    for (int i = 0; i < 10; ++i) {
        arr.push_back("old_" + to_string(i));
    }
    
    for (int i = 0; i < 10; ++i) {
        BOOST_CHECK(arr.replace(i, "new_" + to_string(i)));
        BOOST_CHECK_EQUAL(arr.get(i), "new_" + to_string(i));
    }
    BOOST_CHECK_EQUAL(arr.length(), 10);
}

BOOST_AUTO_TEST_CASE(LengthOperations) {
    Array arr;
    BOOST_CHECK_EQUAL(arr.length(), 0);
    
    arr.push_back("A");
    BOOST_CHECK_EQUAL(arr.length(), 1);
    
    arr.push_back("B");
    BOOST_CHECK_EQUAL(arr.length(), 2);
    
    arr.remove(0);
    BOOST_CHECK_EQUAL(arr.length(), 1);
    
    arr.remove(0);
    BOOST_CHECK_EQUAL(arr.length(), 0);
}

BOOST_AUTO_TEST_CASE(ToVector) {
    Array arr;
    arr.push_back("A");
    arr.push_back("B");
    arr.push_back("C");
    
    vector<string> vec = arr.to_vector();
    BOOST_CHECK_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec[0], "A");
    BOOST_CHECK_EQUAL(vec[1], "B");
    BOOST_CHECK_EQUAL(vec[2], "C");
}

BOOST_AUTO_TEST_CASE(ToVectorEmpty) {
    Array arr;
    vector<string> vec = arr.to_vector();
    BOOST_CHECK(vec.empty());
}

BOOST_AUTO_TEST_CASE(SerializeBinary) {
    Array arr;
    arr.push_back("Hello");
    arr.push_back("World");
    arr.push_back("Test");
    
    BOOST_CHECK(arr.serialize_binary("test_array.bin"));
    BOOST_CHECK(fs::exists("test_array.bin"));
    
    // Проверяем размер файла
    uintmax_t size = fs::file_size("test_array.bin");
    BOOST_CHECK_GT(size, 0);
}

BOOST_AUTO_TEST_CASE(DeserializeBinary) {
    Array arr;
    arr.push_back("Line1");
    arr.push_back("Line2");
    arr.push_back("Line3");
    
    BOOST_REQUIRE(arr.serialize_binary("test_array.bin"));
    
    Array arr2;
    BOOST_CHECK(arr2.deserialize_binary("test_array.bin"));
    BOOST_CHECK_EQUAL(arr2.length(), 3);
    BOOST_CHECK_EQUAL(arr2.get(0), "Line1");
    BOOST_CHECK_EQUAL(arr2.get(1), "Line2");
    BOOST_CHECK_EQUAL(arr2.get(2), "Line3");
}

BOOST_AUTO_TEST_CASE(DeserializeBinaryToNonEmpty) {
    Array arr1;
    arr1.push_back("Hello");
    arr1.push_back("World");
    BOOST_REQUIRE(arr1.serialize_binary("test_array.bin"));
    
    Array arr2;
    arr2.push_back("Old1");
    arr2.push_back("Old2");
    arr2.push_back("Old3");
    
    BOOST_CHECK(arr2.deserialize_binary("test_array.bin"));
    BOOST_CHECK_EQUAL(arr2.length(), 2); // Старые данные должны быть очищены
    BOOST_CHECK_EQUAL(arr2.get(0), "Hello");
}

BOOST_AUTO_TEST_CASE(DeserializeBinaryNonexistent) {
    Array arr;
    BOOST_CHECK(!arr.deserialize_binary("nonexistent.bin"));
}

BOOST_AUTO_TEST_CASE(SerializeText) {
    Array arr;
    arr.push_back("First line");
    arr.push_back("Second line");
    arr.push_back("Third line with spaces");
    
    BOOST_CHECK(arr.serialize_text("test_array.txt"));
    BOOST_CHECK(fs::exists("test_array.txt"));
    
    // Читаем файл и проверяем содержимое
    ifstream file("test_array.txt");
    string line;
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "3"); // Размер
    
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "First line");
    
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "Second line");
    
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "Third line with spaces");
    
    file.close();
}

BOOST_AUTO_TEST_CASE(DeserializeText) {
    Array arr;
    arr.push_back("Test1");
    arr.push_back("Test2");
    arr.push_back("Test3");
    
    BOOST_REQUIRE(arr.serialize_text("test_array.txt"));
    
    Array arr2;
    BOOST_CHECK(arr2.deserialize_text("test_array.txt"));
    BOOST_CHECK_EQUAL(arr2.length(), 3);
    BOOST_CHECK_EQUAL(arr2.get(0), "Test1");
    BOOST_CHECK_EQUAL(arr2.get(1), "Test2");
    BOOST_CHECK_EQUAL(arr2.get(2), "Test3");
}

BOOST_AUTO_TEST_CASE(DeserializeTextCorrupted) {
    // Создаем поврежденный файл
    ofstream file("corrupted.txt");
    file << "not_a_number" << endl;
    file << "some data" << endl;
    file.close();
    
    Array arr;
    BOOST_CHECK(!arr.deserialize_text("corrupted.txt"));
    
    // Другой поврежденный файл - недостаточно строк
    ofstream file2("corrupted2.txt");
    file2 << "3" << endl; // Говорим, что 3 строки
    file2 << "line1" << endl;
    file2 << "line2" << endl;
    // Третьей строки нет
    file2.close();
    
    Array arr2;
    BOOST_CHECK(!arr2.deserialize_text("corrupted2.txt"));
}

BOOST_AUTO_TEST_CASE(PrintEmptyArray) {
    Array arr;
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    arr.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.is_equal("массив пуст\n") || 
                output.str().find("массив пуст") != string::npos);
}

BOOST_AUTO_TEST_CASE(PrintNonEmptyArray) {
    Array arr;
    arr.push_back("A");
    arr.push_back("B");
    arr.push_back("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    arr.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("массив [3]") != string::npos);
    BOOST_CHECK(output.str().find("A") != string::npos);
    BOOST_CHECK(output.str().find("B") != string::npos);
    BOOST_CHECK(output.str().find("C") != string::npos);
}

BOOST_AUTO_TEST_CASE(StressTest) {
    Array arr;
    const int N = 10000;
    
    // Добавление большого количества элементов
    for (int i = 0; i < N; ++i) {
        arr.push_back("item_" + to_string(i));
    }
    
    BOOST_CHECK_EQUAL(arr.length(), N);
    
    // Проверка случайных элементов
    mt19937 rng(42);
    for (int i = 0; i < 100; ++i) {
        int idx = rng() % N;
        BOOST_CHECK_EQUAL(arr.get(idx), "item_" + to_string(idx));
    }
    
    // Удаление половины элементов
    for (int i = 0; i < N/2; ++i) {
        BOOST_CHECK(arr.remove(0));
    }
    
    BOOST_CHECK_EQUAL(arr.length(), N/2);
    
    // Добавление еще элементов
    for (int i = 0; i < 5000; ++i) {
        arr.push_back("new_" + to_string(i));
    }
    
    BOOST_CHECK_EQUAL(arr.length(), N/2 + 5000);
}

BOOST_AUTO_TEST_CASE(SerializationRoundTrip) {
    Array original;
    for (int i = 0; i < 100; ++i) {
        original.push_back(generateRandomString(20));
    }
    
    // Бинарный round-trip
    BOOST_REQUIRE(original.serialize_binary("roundtrip.bin"));
    
    Array from_binary;
    BOOST_REQUIRE(from_binary.deserialize_binary("roundtrip.bin"));
    
    BOOST_CHECK_EQUAL(original.length(), from_binary.length());
    for (int i = 0; i < original.length(); ++i) {
        BOOST_CHECK_EQUAL(original.get(i), from_binary.get(i));
    }
    
    // Текстовый round-trip
    BOOST_REQUIRE(original.serialize_text("roundtrip.txt"));
    
    Array from_text;
    BOOST_REQUIRE(from_text.deserialize_text("roundtrip.txt"));
    
    BOOST_CHECK_EQUAL(original.length(), from_text.length());
    for (int i = 0; i < original.length(); ++i) {
        BOOST_CHECK_EQUAL(original.get(i), from_text.get(i));
    }
    
    fs::remove("roundtrip.bin");
    fs::remove("roundtrip.txt");
}

BOOST_AUTO_TEST_CASE(EmptySerialization) {
    Array empty;
    
    // Бинарная сериализация пустого массива
    BOOST_CHECK(empty.serialize_binary("empty.bin"));
    BOOST_CHECK(fs::exists("empty.bin"));
    
    Array loaded_empty;
    BOOST_CHECK(loaded_empty.deserialize_binary("empty.bin"));
    BOOST_CHECK_EQUAL(loaded_empty.length(), 0);
    
    // Текстовая сериализация пустого массива
    BOOST_CHECK(empty.serialize_text("empty.txt"));
    
    ifstream file("empty.txt");
    string line;
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "0");
    file.close();
    
    fs::remove("empty.bin");
    fs::remove("empty.txt");
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== SingleList Tests ====================
BOOST_AUTO_TEST_SUITE(SingleListTests)

BOOST_AUTO_TEST_CASE(SingleListDefaultConstructor) {
    SingleList list;
    BOOST_CHECK_EQUAL(list.get_size(), 0);
}

BOOST_AUTO_TEST_CASE(SingleListCopyConstructor) {
    SingleList list1;
    list1.push_back("A");
    list1.push_back("B");
    list1.push_back("C");
    
    SingleList list2(list1);
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
    
    // Проверяем содержимое
    SNode* node = list2.find_first();
    vector<string> elements;
    while (node != nullptr) {
        elements.push_back(node->data);
        node = list2.find_next(node);
    }
    
    BOOST_CHECK_EQUAL(elements.size(), 3);
    BOOST_CHECK_EQUAL(elements[0], "A");
    BOOST_CHECK_EQUAL(elements[1], "B");
    BOOST_CHECK_EQUAL(elements[2], "C");
}

BOOST_AUTO_TEST_CASE(SingleListAssignmentOperator) {
    SingleList list1;
    list1.push_back("X");
    list1.push_back("Y");
    list1.push_back("Z");
    
    SingleList list2;
    list2 = list1;
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
    
    // Самоприсваивание
    list2 = list2;
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
}

BOOST_AUTO_TEST_CASE(SingleListPushFront) {
    SingleList list;
    list.push_front("C");
    list.push_front("B");
    list.push_front("A");
    
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    SNode* first = list.find_first();
    BOOST_REQUIRE(first != nullptr);
    BOOST_CHECK_EQUAL(first->data, "A");
    BOOST_CHECK_EQUAL(list.find_next(first)->data, "B");
}

BOOST_AUTO_TEST_CASE(SingleListPushBack) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок
    SNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "B");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    current = list.find_next(current);
    BOOST_CHECK(current == nullptr);
}

BOOST_AUTO_TEST_CASE(SingleListInsertBefore) {
    SingleList list;
    list.push_back("A");
    list.push_back("C");
    
    BOOST_CHECK(list.insert_before("C", "B"));
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок: A -> B -> C
    SNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "B");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    
    // Вставка перед первым элементом
    BOOST_CHECK(list.insert_before("A", "First"));
    BOOST_CHECK_EQUAL(list.get_size(), 4);
    BOOST_CHECK_EQUAL(list.find_first()->data, "First");
    
    // Вставка перед несуществующим элементом
    BOOST_CHECK(!list.insert_before("X", "NotExists"));
}

BOOST_AUTO_TEST_CASE(SingleListInsertAfter) {
    SingleList list;
    list.push_back("A");
    list.push_back("C");
    
    BOOST_CHECK(list.insert_after("A", "B"));
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок: A -> B -> C
    SNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "B");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    
    // Вставка после последнего элемента
    BOOST_CHECK(list.insert_after("C", "D"));
    BOOST_CHECK_EQUAL(list.get_size(), 4);
    
    // Вставка после несуществующего элемента
    BOOST_CHECK(!list.insert_after("X", "NotExists"));
}

BOOST_AUTO_TEST_CASE(SingleListPopFront) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK(list.pop_front());
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    BOOST_CHECK_EQUAL(list.find_first()->data, "B");
    
    BOOST_CHECK(list.pop_front());
    BOOST_CHECK_EQUAL(list.get_size(), 1);
    BOOST_CHECK_EQUAL(list.find_first()->data, "C");
    
    BOOST_CHECK(list.pop_front());
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    BOOST_CHECK(list.find_first() == nullptr);
    
    BOOST_CHECK(!list.pop_front()); // Пустой список
}

BOOST_AUTO_TEST_CASE(SingleListPopBack) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK(list.pop_back());
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    
    // Проверяем, что последний элемент удален
    SNode* current = list.find_first();
    current = list.find_next(current); // Переходим к последнему
    BOOST_CHECK_EQUAL(current->data, "B");
    BOOST_CHECK(list.find_next(current) == nullptr);
    
    BOOST_CHECK(list.pop_back());
    BOOST_CHECK_EQUAL(list.get_size(), 1);
    
    BOOST_CHECK(list.pop_back());
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    
    BOOST_CHECK(!list.pop_back()); // Пустой список
}

BOOST_AUTO_TEST_CASE(SingleListRemoveBefore) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    
    BOOST_CHECK(list.remove_before("C")); // Удаляем B
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок: A -> C -> D
    SNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    
    BOOST_CHECK(list.remove_before("D")); // Удаляем C
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    
    // Проверяем порядок: A -> D
    current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "D");
    
    // Нельзя удалить перед первым элементом
    BOOST_CHECK(!list.remove_before("A"));
    // Несуществующий элемент
    BOOST_CHECK(!list.remove_before("X"));
}

BOOST_AUTO_TEST_CASE(SingleListRemoveAfter) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    
    BOOST_CHECK(list.remove_after("A")); // Удаляем B
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок: A -> C -> D
    SNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    
    BOOST_CHECK(list.remove_after("C")); // Удаляем D
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    
    // Проверяем порядок: A -> C
    current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    BOOST_CHECK(list.find_next(current) == nullptr);
    
    // Нельзя удалить после последнего элемента
    BOOST_CHECK(!list.remove_after("C"));
    // Несуществующий элемент
    BOOST_CHECK(!list.remove_after("X"));
}

BOOST_AUTO_TEST_CASE(SingleListRemoveValue) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    
    BOOST_CHECK(list.remove_value("B"));
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок: A -> C -> D
    SNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    
    BOOST_CHECK(list.remove_value("A")); // Удаляем первый
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    BOOST_CHECK_EQUAL(list.find_first()->data, "C");
    
    BOOST_CHECK(list.remove_value("D")); // Удаляем последний
    BOOST_CHECK_EQUAL(list.get_size(), 1);
    BOOST_CHECK_EQUAL(list.find_first()->data, "C");
    
    BOOST_CHECK(list.remove_value("C")); // Удаляем единственный
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    BOOST_CHECK(list.find_first() == nullptr);
    
    // Несуществующее значение
    BOOST_CHECK(!list.remove_value("X"));
}

BOOST_AUTO_TEST_CASE(SingleListFind) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    SNode* found = list.find("B");
    BOOST_REQUIRE(found != nullptr);
    BOOST_CHECK_EQUAL(found->data, "B");
    
    found = list.find("A");
    BOOST_REQUIRE(found != nullptr);
    BOOST_CHECK_EQUAL(found->data, "A");
    
    found = list.find("C");
    BOOST_REQUIRE(found != nullptr);
    BOOST_CHECK_EQUAL(found->data, "C");
    
    // Несуществующее значение
    BOOST_CHECK(list.find("X") == nullptr);
    
    // Пустой список
    SingleList empty;
    BOOST_CHECK(empty.find("anything") == nullptr);
}

BOOST_AUTO_TEST_CASE(SingleListPrintForward) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    list.print_forward();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("Односвязный список [3]") != string::npos);
    BOOST_CHECK(output.str().find("A -> B -> C") != string::npos);
}

BOOST_AUTO_TEST_CASE(SingleListPrintBackward) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    list.print_backward();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("обратном порядке") != string::npos);
    BOOST_CHECK(output.str().find("C") != string::npos);
}

BOOST_AUTO_TEST_CASE(SingleListPrintEmpty) {
    SingleList list;
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    list.print_forward();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("пуст") != string::npos);
}

BOOST_AUTO_TEST_CASE(SingleListSerializeBinary) {
    SingleList list;
    list.push_back("First");
    list.push_back("Second");
    list.push_back("Third");
    
    BOOST_CHECK(list.serialize_binary("test_slist.bin"));
    BOOST_CHECK(fs::exists("test_slist.bin"));
    
    SingleList list2;
    BOOST_CHECK(list2.deserialize_binary("test_slist.bin"));
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
    
    // Проверяем содержимое
    SNode* node1 = list2.find_first();
    BOOST_REQUIRE(node1 != nullptr);
    BOOST_CHECK_EQUAL(node1->data, "First");
    
    SNode* node2 = list2.find_next(node1);
    BOOST_REQUIRE(node2 != nullptr);
    BOOST_CHECK_EQUAL(node2->data, "Second");
    
    SNode* node3 = list2.find_next(node2);
    BOOST_REQUIRE(node3 != nullptr);
    BOOST_CHECK_EQUAL(node3->data, "Third");
    BOOST_CHECK(list2.find_next(node3) == nullptr);
}

BOOST_AUTO_TEST_CASE(SingleListSerializeText) {
    SingleList list;
    list.push_back("Line1");
    list.push_back("Line2");
    list.push_back("Line3");
    
    BOOST_CHECK(list.serialize_text("test_slist.txt"));
    BOOST_CHECK(fs::exists("test_slist.txt"));
    
    // Проверяем содержимое файла
    ifstream file("test_slist.txt");
    string line;
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "3"); // Размер
    
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "Line1");
    
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "Line2");
    
    getline(file, line);
    BOOST_CHECK_EQUAL(line, "Line3");
    
    file.close();
    
    SingleList list2;
    BOOST_CHECK(list2.deserialize_text("test_slist.txt"));
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
}

BOOST_AUTO_TEST_CASE(SingleListStressTest) {
    SingleList list;
    const int N = 5000;
    
    // Добавление в начало (O(1))
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        list.push_front("item_" + to_string(i));
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "SingleList push_front " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    BOOST_CHECK_EQUAL(list.get_size(), N);
    
    // Удаление всех элементов с начала
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        BOOST_CHECK(list.pop_front());
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "SingleList pop_front " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    
    // Добавление в конец
    for (int i = 0; i < N; ++i) {
        list.push_back("item_" + to_string(i));
    }
    BOOST_CHECK_EQUAL(list.get_size(), N);
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== DoubleList Tests ====================
BOOST_AUTO_TEST_SUITE(DoubleListTests)

BOOST_AUTO_TEST_CASE(DoubleListDefaultConstructor) {
    DoubleList list;
    BOOST_CHECK_EQUAL(list.get_size(), 0);
}

BOOST_AUTO_TEST_CASE(DoubleListCopyConstructor) {
    DoubleList list1;
    list1.push_back("A");
    list1.push_back("B");
    list1.push_back("C");
    
    DoubleList list2(list1);
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
    
    // Проверяем двунаправленные связи
    DNode* node = list2.find_first();
    BOOST_REQUIRE(node != nullptr);
    BOOST_CHECK_EQUAL(node->data, "A");
    BOOST_CHECK(node->prev == nullptr);
    
    node = list2.find_next(node);
    BOOST_REQUIRE(node != nullptr);
    BOOST_CHECK_EQUAL(node->data, "B");
    
    node = list2.find_next(node);
    BOOST_REQUIRE(node != nullptr);
    BOOST_CHECK_EQUAL(node->data, "C");
    BOOST_CHECK(list2.find_next(node) == nullptr);
}

BOOST_AUTO_TEST_CASE(DoubleListPushFront) {
    DoubleList list;
    list.push_front("C");
    list.push_front("B");
    list.push_front("A");
    
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    BOOST_CHECK_EQUAL(list.find_first()->data, "A");
}

BOOST_AUTO_TEST_CASE(DoubleListPushBack) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Идем от начала до конца
    DNode* current = list.find_first();
    BOOST_CHECK_EQUAL(current->data, "A");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "B");
    current = list.find_next(current);
    BOOST_CHECK_EQUAL(current->data, "C");
    BOOST_CHECK(list.find_next(current) == nullptr);
}

BOOST_AUTO_TEST_CASE(DoubleListInsertBefore) {
    DoubleList list;
    list.push_back("A");
    list.push_back("C");
    
    BOOST_CHECK(list.insert_before("C", "B"));
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем порядок и связи
    DNode* nodeB = list.find("B");
    BOOST_REQUIRE(nodeB != nullptr);
    DNode* nodeA = nodeB->prev;
    DNode* nodeC = nodeB->next;
    
    BOOST_CHECK_EQUAL(nodeA->data, "A");
    BOOST_CHECK_EQUAL(nodeB->data, "B");
    BOOST_CHECK_EQUAL(nodeC->data, "C");
    
    BOOST_CHECK(nodeA->next == nodeB);
    BOOST_CHECK(nodeB->prev == nodeA);
    BOOST_CHECK(nodeB->next == nodeC);
    BOOST_CHECK(nodeC->prev == nodeB);
}

BOOST_AUTO_TEST_CASE(DoubleListInsertAfter) {
    DoubleList list;
    list.push_back("A");
    list.push_back("C");
    
    BOOST_CHECK(list.insert_after("A", "B"));
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    DNode* nodeA = list.find("A");
    DNode* nodeB = nodeA->next;
    DNode* nodeC = nodeB->next;
    
    BOOST_CHECK_EQUAL(nodeA->data, "A");
    BOOST_CHECK_EQUAL(nodeB->data, "B");
    BOOST_CHECK_EQUAL(nodeC->data, "C");
    
    BOOST_CHECK(nodeA->next == nodeB);
    BOOST_CHECK(nodeB->prev == nodeA);
    BOOST_CHECK(nodeB->next == nodeC);
    BOOST_CHECK(nodeC->prev == nodeB);
}

BOOST_AUTO_TEST_CASE(DoubleListPopFront) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK(list.pop_front());
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    BOOST_CHECK_EQUAL(list.find_first()->data, "B");
    BOOST_CHECK(list.find_first()->prev == nullptr);
    
    BOOST_CHECK(list.pop_front());
    BOOST_CHECK_EQUAL(list.get_size(), 1);
    BOOST_CHECK_EQUAL(list.find_first()->data, "C");
    
    BOOST_CHECK(list.pop_front());
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    BOOST_CHECK(list.find_first() == nullptr);
    
    BOOST_CHECK(!list.pop_front()); // Пустой список
}

BOOST_AUTO_TEST_CASE(DoubleListPopBack) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK(list.pop_back());
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    
    // Проверяем, что последний элемент - B
    DNode* first = list.find_first();
    DNode* second = list.find_next(first);
    BOOST_CHECK_EQUAL(second->data, "B");
    BOOST_CHECK(list.find_next(second) == nullptr);
    
    BOOST_CHECK(list.pop_back());
    BOOST_CHECK_EQUAL(list.get_size(), 1);
    BOOST_CHECK_EQUAL(list.find_first()->data, "A");
    
    BOOST_CHECK(list.pop_back());
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    
    BOOST_CHECK(!list.pop_back()); // Пустой список
}

BOOST_AUTO_TEST_CASE(DoubleListRemoveBefore) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    
    BOOST_CHECK(list.remove_before("C")); // Удаляем B
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем связи: A -> C -> D
    DNode* nodeA = list.find("A");
    DNode* nodeC = list.find("C");
    DNode* nodeD = list.find("D");
    
    BOOST_REQUIRE(nodeA != nullptr);
    BOOST_REQUIRE(nodeC != nullptr);
    BOOST_REQUIRE(nodeD != nullptr);
    
    BOOST_CHECK(nodeA->next == nodeC);
    BOOST_CHECK(nodeC->prev == nodeA);
    BOOST_CHECK(nodeC->next == nodeD);
    
    // Нельзя удалить перед первым элементом
    BOOST_CHECK(!list.remove_before("A"));
}

BOOST_AUTO_TEST_CASE(DoubleListRemoveAfter) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    
    BOOST_CHECK(list.remove_after("B")); // Удаляем C
    BOOST_CHECK_EQUAL(list.get_size(), 3);
    
    // Проверяем связи: A -> B -> D
    DNode* nodeA = list.find("A");
    DNode* nodeB = list.find("B");
    DNode* nodeD = list.find("D");
    
    BOOST_REQUIRE(nodeA != nullptr);
    BOOST_REQUIRE(nodeB != nullptr);
    BOOST_REQUIRE(nodeD != nullptr);
    
    BOOST_CHECK(nodeA->next == nodeB);
    BOOST_CHECK(nodeB->prev == nodeA);
    BOOST_CHECK(nodeB->next == nodeD);
    BOOST_CHECK(nodeD->prev == nodeB);
    
    // Нельзя удалить после последнего элемента
    BOOST_CHECK(!list.remove_after("D"));
}

BOOST_AUTO_TEST_CASE(DoubleListRemoveValue) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    BOOST_CHECK(list.remove_value("B"));
    BOOST_CHECK_EQUAL(list.get_size(), 2);
    
    // Проверяем связи: A -> C
    DNode* nodeA = list.find("A");
    DNode* nodeC = list.find("C");
    
    BOOST_REQUIRE(nodeA != nullptr);
    BOOST_REQUIRE(nodeC != nullptr);
    
    BOOST_CHECK(nodeA->next == nodeC);
    BOOST_CHECK(nodeC->prev == nodeA);
    
    BOOST_CHECK(list.remove_value("A")); // Удаляем первый
    BOOST_CHECK_EQUAL(list.get_size(), 1);
    BOOST_CHECK_EQUAL(list.find_first()->data, "C");
    BOOST_CHECK(list.find_first()->prev == nullptr);
    
    BOOST_CHECK(list.remove_value("C")); // Удаляем последний
    BOOST_CHECK_EQUAL(list.get_size(), 0);
    BOOST_CHECK(list.find_first() == nullptr);
    
    // Несуществующее значение
    BOOST_CHECK(!list.remove_value("X"));
}

BOOST_AUTO_TEST_CASE(DoubleListPrintForward) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    list.print_forward();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("Двусвязный список [3]") != string::npos);
    BOOST_CHECK(output.str().find("A <-> B <-> C") != string::npos);
}

BOOST_AUTO_TEST_CASE(DoubleListPrintBackward) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    list.print_backward();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("обратном порядке") != string::npos);
    BOOST_CHECK(output.str().find("C <-> B <-> A") != string::npos);
}

BOOST_AUTO_TEST_CASE(DoubleListSerializeBinary) {
    DoubleList list;
    list.push_back("First");
    list.push_back("Second");
    list.push_back("Third");
    
    BOOST_CHECK(list.serialize_binary("test_dlist.bin"));
    
    DoubleList list2;
    BOOST_CHECK(list2.deserialize_binary("test_dlist.bin"));
    BOOST_CHECK_EQUAL(list2.get_size(), 3);
    
    // Проверяем содержимое и связи
    DNode* node1 = list2.find_first();
    BOOST_REQUIRE(node1 != nullptr);
    BOOST_CHECK_EQUAL(node1->data, "First");
    BOOST_CHECK(node1->prev == nullptr);
    
    DNode* node2 = list2.find_next(node1);
    BOOST_REQUIRE(node2 != nullptr);
    BOOST_CHECK_EQUAL(node2->data, "Second");
    BOOST_CHECK(node2->prev == node1);
    
    DNode* node3 = list2.find_next(node2);
    BOOST_REQUIRE(node3 != nullptr);
    BOOST_CHECK_EQUAL(node3->data, "Third");
    BOOST_CHECK(node3->prev == node2);
    BOOST_CHECK(node3->next == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== Stack Tests ====================
BOOST_AUTO_TEST_SUITE(StackTests)

BOOST_AUTO_TEST_CASE(StackDefaultConstructor) {
    Stack s;
    BOOST_CHECK(s.is_empty());
    BOOST_CHECK_EQUAL(s.get_size(), 0);
}

BOOST_AUTO_TEST_CASE(StackPushAndPop) {
    Stack s;
    s.push("A");
    s.push("B");
    s.push("C");
    
    BOOST_CHECK(!s.is_empty());
    BOOST_CHECK_EQUAL(s.get_size(), 3);
    BOOST_CHECK_EQUAL(s.peek(), "C");
    
    BOOST_CHECK_EQUAL(s.pop(), "C");
    BOOST_CHECK_EQUAL(s.pop(), "B");
    BOOST_CHECK_EQUAL(s.pop(), "A");
    
    BOOST_CHECK(s.is_empty());
    BOOST_CHECK_EQUAL(s.get_size(), 0);
}

BOOST_AUTO_TEST_CASE(StackPopEmpty) {
    Stack s;
    BOOST_CHECK_EQUAL(s.pop(), "");
    BOOST_CHECK_EQUAL(s.peek(), "");
}

BOOST_AUTO_TEST_CASE(StackResize) {
    Stack s(2); // Маленькая емкость
    s.push("A");
    s.push("B");
    s.push("C"); // Должен вызвать resize
    
    BOOST_CHECK_EQUAL(s.get_size(), 3);
    BOOST_CHECK_EQUAL(s.pop(), "C");
    BOOST_CHECK_EQUAL(s.pop(), "B");
    BOOST_CHECK_EQUAL(s.pop(), "A");
}

BOOST_AUTO_TEST_CASE(StackCopyConstructor) {
    Stack s1;
    s1.push("A");
    s1.push("B");
    s1.push("C");
    
    Stack s2(s1);
    BOOST_CHECK_EQUAL(s2.get_size(), 3);
    BOOST_CHECK_EQUAL(s2.pop(), "C");
    BOOST_CHECK_EQUAL(s2.pop(), "B");
    BOOST_CHECK_EQUAL(s2.pop(), "A");
}

BOOST_AUTO_TEST_CASE(StackAssignmentOperator) {
    Stack s1;
    s1.push("X");
    s1.push("Y");
    s1.push("Z");
    
    Stack s2;
    s2 = s1;
    BOOST_CHECK_EQUAL(s2.get_size(), 3);
    
    // Исходный стек не должен измениться
    BOOST_CHECK_EQUAL(s1.get_size(), 3);
    BOOST_CHECK_EQUAL(s1.pop(), "Z");
}

BOOST_AUTO_TEST_CASE(StackPrint) {
    Stack s;
    s.push("A");
    s.push("B");
    s.push("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    s.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("Стопка") != string::npos);
    BOOST_CHECK(output.str().find("C | B | A") != string::npos);
}

BOOST_AUTO_TEST_CASE(StackPrintEmpty) {
    Stack s;
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    s.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("пуст") != string::npos);
}

BOOST_AUTO_TEST_CASE(StackSerializeBinary) {
    Stack s;
    s.push("First");
    s.push("Second");
    s.push("Third");
    
    BOOST_CHECK(s.serialize_binary("test_stack.bin"));
    
    Stack s2;
    BOOST_CHECK(s2.deserialize_binary("test_stack.bin"));
    BOOST_CHECK_EQUAL(s2.get_size(), 3);
    
    // Проверяем порядок (стек - LIFO)
    BOOST_CHECK_EQUAL(s2.pop(), "Third");
    BOOST_CHECK_EQUAL(s2.pop(), "Second");
    BOOST_CHECK_EQUAL(s2.pop(), "First");
}

BOOST_AUTO_TEST_CASE(StackStressTest) {
    Stack s;
    const int N = 10000;
    
    for (int i = 0; i < N; ++i) {
        s.push("item_" + to_string(i));
    }
    
    BOOST_CHECK_EQUAL(s.get_size(), N);
    
    for (int i = N - 1; i >= 0; --i) {
        BOOST_CHECK_EQUAL(s.pop(), "item_" + to_string(i));
    }
    
    BOOST_CHECK(s.is_empty());
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== Queue Tests ====================
BOOST_AUTO_TEST_SUITE(QueueTests)

BOOST_AUTO_TEST_CASE(QueueDefaultConstructor) {
    Queue q;
    BOOST_CHECK(q.is_empty());
    BOOST_CHECK_EQUAL(q.get_size(), 0);
}

BOOST_AUTO_TEST_CASE(QueuePushAndPop) {
    Queue q;
    q.push("A");
    q.push("B");
    q.push("C");
    
    BOOST_CHECK(!q.is_empty());
    BOOST_CHECK_EQUAL(q.get_size(), 3);
    BOOST_CHECK_EQUAL(q.peek(), "A");
    
    BOOST_CHECK_EQUAL(q.pop(), "A");
    BOOST_CHECK_EQUAL(q.pop(), "B");
    BOOST_CHECK_EQUAL(q.pop(), "C");
    
    BOOST_CHECK(q.is_empty());
    BOOST_CHECK_EQUAL(q.get_size(), 0);
}

BOOST_AUTO_TEST_CASE(QueuePopEmpty) {
    Queue q;
    BOOST_CHECK_EQUAL(q.pop(), "");
    BOOST_CHECK_EQUAL(q.peek(), "");
}

BOOST_AUTO_TEST_CASE(QueueCircularBuffer) {
    Queue q(3); // Маленькая емкость
    q.push("A");
    q.push("B");
    q.push("C");
    q.pop(); // Удаляем A
    q.push("D"); // Должен использовать освободившееся место
    
    BOOST_CHECK_EQUAL(q.get_size(), 3);
    BOOST_CHECK_EQUAL(q.peek(), "B");
    
    BOOST_CHECK_EQUAL(q.pop(), "B");
    BOOST_CHECK_EQUAL(q.pop(), "C");
    BOOST_CHECK_EQUAL(q.pop(), "D");
    BOOST_CHECK(q.is_empty());
}

BOOST_AUTO_TEST_CASE(QueueResize) {
    Queue q(2);
    q.push("A");
    q.push("B");
    q.push("C"); // Должен вызвать resize
    
    BOOST_CHECK_EQUAL(q.get_size(), 3);
    BOOST_CHECK_EQUAL(q.peek(), "A");
    
    BOOST_CHECK_EQUAL(q.pop(), "A");
    BOOST_CHECK_EQUAL(q.pop(), "B");
    BOOST_CHECK_EQUAL(q.pop(), "C");
}

BOOST_AUTO_TEST_CASE(QueueCopyConstructor) {
    Queue q1;
    q1.push("A");
    q1.push("B");
    q1.push("C");
    
    Queue q2(q1);
    BOOST_CHECK_EQUAL(q2.get_size(), 3);
    BOOST_CHECK_EQUAL(q2.pop(), "A");
    BOOST_CHECK_EQUAL(q2.pop(), "B");
    BOOST_CHECK_EQUAL(q2.pop(), "C");
}

BOOST_AUTO_TEST_CASE(QueuePrint) {
    Queue q;
    q.push("A");
    q.push("B");
    q.push("C");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    q.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("Очередь") != string::npos);
    BOOST_CHECK(output.str().find("A -> B -> C") != string::npos);
}

BOOST_AUTO_TEST_CASE(QueueSerializeBinary) {
    Queue q;
    q.push("First");
    q.push("Second");
    q.push("Third");
    
    BOOST_CHECK(q.serialize_binary("test_queue.bin"));
    
    Queue q2;
    BOOST_CHECK(q2.deserialize_binary("test_queue.bin"));
    BOOST_CHECK_EQUAL(q2.get_size(), 3);
    
    // Проверяем порядок (очередь - FIFO)
    BOOST_CHECK_EQUAL(q2.pop(), "First");
    BOOST_CHECK_EQUAL(q2.pop(), "Second");
    BOOST_CHECK_EQUAL(q2.pop(), "Third");
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== FullBinaryTree Tests ====================
BOOST_AUTO_TEST_SUITE(FullBinaryTreeTests)

BOOST_AUTO_TEST_CASE(FullBinaryTreeDefaultConstructor) {
    FullBinaryTree tree;
    BOOST_CHECK_EQUAL(tree.get_size(), 0);
    BOOST_CHECK(tree.is_full());
    BOOST_CHECK_EQUAL(tree.height(), 0);
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeInsertAndSearch) {
    FullBinaryTree tree;
    BOOST_CHECK(tree.insert(10, "Root"));
    BOOST_CHECK(tree.insert(5, "Left"));
    BOOST_CHECK(tree.insert(15, "Right"));
    
    BOOST_CHECK_EQUAL(tree.get_size(), 3);
    BOOST_CHECK_EQUAL(tree.search(10), "Root");
    BOOST_CHECK_EQUAL(tree.search(5), "Left");
    BOOST_CHECK_EQUAL(tree.search(15), "Right");
    BOOST_CHECK_EQUAL(tree.search(99), ""); // Несуществующий ключ
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeInsertDuplicate) {
    FullBinaryTree tree;
    tree.insert(10, "First");
    BOOST_CHECK(!tree.insert(10, "Second")); // Дубликат
    BOOST_CHECK_EQUAL(tree.search(10), "First");
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeLevelOrderInsert) {
    FullBinaryTree tree;
    // Вставка в порядке уровней для создания полного дерева
    tree.insert(1, "A");  // Уровень 1
    tree.insert(2, "B");  // Уровень 2, левый
    tree.insert(3, "C");  // Уровень 2, правый
    tree.insert(4, "D");  // Уровень 3, левый левого
    tree.insert(5, "E");  // Уровень 3, правый левого
    tree.insert(6, "F");  // Уровень 3, левый правого
    tree.insert(7, "G");  // Уровень 3, правый правого
    
    BOOST_CHECK_EQUAL(tree.get_size(), 7);
    BOOST_CHECK(tree.is_full());
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeIsFull) {
    FullBinaryTree tree;
    BOOST_CHECK(tree.is_full()); // Пустое дерево считается полным
    
    tree.insert(1, "A");
    BOOST_CHECK(tree.is_full()); // Узел без детей
    
    tree.insert(2, "B");
    BOOST_CHECK(!tree.is_full()); // Только левый ребенок
    
    tree.insert(3, "C");
    BOOST_CHECK(tree.is_full()); // Оба ребенка
    
    tree.insert(4, "D");
    BOOST_CHECK(!tree.is_full()); // Не полное на следующем уровне
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeHeight) {
    FullBinaryTree tree;
    BOOST_CHECK_EQUAL(tree.height(), 0);
    
    tree.insert(1, "A");
    BOOST_CHECK_EQUAL(tree.height(), 1);
    
    tree.insert(2, "B");
    tree.insert(3, "C");
    BOOST_CHECK_EQUAL(tree.height(), 2);
    
    tree.insert(4, "D");
    tree.insert(5, "E");
    tree.insert(6, "F");
    tree.insert(7, "G");
    BOOST_CHECK_EQUAL(tree.height(), 3);
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeTraversals) {
    FullBinaryTree tree;
    tree.insert(4, "D");
    tree.insert(2, "B");
    tree.insert(6, "F");
    tree.insert(1, "A");
    tree.insert(3, "C");
    tree.insert(5, "E");
    tree.insert(7, "G");
    
    // Inorder (симметричный): A B C D E F G
    boost::test_tools::output_test_stream output_inorder;
    auto old_cout = cout.rdbuf(output_inorder.rdbuf());
    tree.inorder();
    cout.rdbuf(old_cout);
    BOOST_CHECK(output_inorder.str().find("Inorder:") != string::npos);
    
    // Preorder (прямой): D B A C F E G
    boost::test_tools::output_test_stream output_preorder;
    old_cout = cout.rdbuf(output_preorder.rdbuf());
    tree.preorder();
    cout.rdbuf(old_cout);
    BOOST_CHECK(output_preorder.str().find("Preorder:") != string::npos);
    
    // Postorder (обратный): A C B E G F D
    boost::test_tools::output_test_stream output_postorder;
    old_cout = cout.rdbuf(output_postorder.rdbuf());
    tree.postorder();
    cout.rdbuf(old_cout);
    BOOST_CHECK(output_postorder.str().find("Postorder:") != string::npos);
    
    // Level order (по уровням): D B F A C E G
    boost::test_tools::output_test_stream output_level;
    old_cout = cout.rdbuf(output_level.rdbuf());
    tree.level_order();
    cout.rdbuf(old_cout);
    BOOST_CHECK(output_level.str().find("Level order:") != string::npos);
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeCopyConstructor) {
    FullBinaryTree tree1;
    tree1.insert(5, "Five");
    tree1.insert(3, "Three");
    tree1.insert(7, "Seven");
    
    FullBinaryTree tree2(tree1);
    BOOST_CHECK_EQUAL(tree2.get_size(), 3);
    BOOST_CHECK_EQUAL(tree2.search(5), "Five");
    BOOST_CHECK_EQUAL(tree2.search(3), "Three");
    BOOST_CHECK_EQUAL(tree2.search(7), "Seven");
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeAssignmentOperator) {
    FullBinaryTree tree1;
    tree1.insert(10, "Ten");
    tree1.insert(5, "Five");
    
    FullBinaryTree tree2;
    tree2 = tree1;
    BOOST_CHECK_EQUAL(tree2.get_size(), 2);
    BOOST_CHECK_EQUAL(tree2.search(10), "Ten");
    
    // Самоприсваивание
    tree2 = tree2;
    BOOST_CHECK_EQUAL(tree2.get_size(), 2);
}

BOOST_AUTO_TEST_CASE(FullBinaryTreePrint) {
    FullBinaryTree tree;
    tree.insert(8, "Root");
    tree.insert(4, "Left");
    tree.insert(12, "Right");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    tree.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("Full Binary Tree") != string::npos);
    BOOST_CHECK(output.str().find("Is full binary tree") != string::npos);
    BOOST_CHECK(output.str().find("Tree size") != string::npos);
    BOOST_CHECK(output.str().find("Tree height") != string::npos);
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeSerializeBinary) {
    FullBinaryTree tree;
    tree.insert(5, "Five");
    tree.insert(3, "Three");
    tree.insert(7, "Seven");
    tree.insert(2, "Two");
    tree.insert(4, "Four");
    tree.insert(6, "Six");
    tree.insert(8, "Eight");
    
    BOOST_CHECK(tree.serialize_binary("test_tree.bin"));
    
    FullBinaryTree tree2;
    BOOST_CHECK(tree2.deserialize_binary("test_tree.bin"));
    BOOST_CHECK_EQUAL(tree2.get_size(), 7);
    
    // Проверяем все узлы
    BOOST_CHECK_EQUAL(tree2.search(5), "Five");
    BOOST_CHECK_EQUAL(tree2.search(3), "Three");
    BOOST_CHECK_EQUAL(tree2.search(7), "Seven");
    BOOST_CHECK_EQUAL(tree2.search(2), "Two");
    BOOST_CHECK_EQUAL(tree2.search(4), "Four");
    BOOST_CHECK_EQUAL(tree2.search(6), "Six");
    BOOST_CHECK_EQUAL(tree2.search(8), "Eight");
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeSerializeText) {
    FullBinaryTree tree;
    tree.insert(10, "Ten");
    tree.insert(5, "Five");
    tree.insert(15, "Fifteen");
    
    BOOST_CHECK(tree.serialize_text("test_tree.txt"));
    
    FullBinaryTree tree2;
    BOOST_CHECK(tree2.deserialize_text("test_tree.txt"));
    BOOST_CHECK_EQUAL(tree2.get_size(), 3);
    BOOST_CHECK_EQUAL(tree2.search(10), "Ten");
    BOOST_CHECK_EQUAL(tree2.search(5), "Five");
    BOOST_CHECK_EQUAL(tree2.search(15), "Fifteen");
}

BOOST_AUTO_TEST_CASE(FullBinaryTreeStressTest) {
    FullBinaryTree tree;
    const int N = 1000;
    
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        tree.insert(i, "Value_" + to_string(i));
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "FullBinaryTree insert " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    BOOST_CHECK_EQUAL(tree.get_size(), N);
    
    // Поиск случайных элементов
    mt19937 rng(42);
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        int key = rng() % N;
        BOOST_CHECK_EQUAL(tree.search(key), "Value_" + to_string(key));
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "FullBinaryTree search 100 элементов: " 
         << duration.count() << " ms" << endl;
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== DoubleHashTable Tests ====================
BOOST_AUTO_TEST_SUITE(DoubleHashTableTests)

BOOST_AUTO_TEST_CASE(DoubleHashTableDefaultConstructor) {
    DoubleHashTable table(10);
    BOOST_CHECK_EQUAL(table.get_size(), 0);
    BOOST_CHECK_EQUAL(table.get_capacity(), 10);
    BOOST_CHECK_EQUAL(table.get_load_factor(), 0.0);
}

BOOST_AUTO_TEST_CASE(DoubleHashTableInsertAndSearch) {
    DoubleHashTable table(10);
    BOOST_CHECK(table.insert("key1", "value1"));
    BOOST_CHECK(table.insert("key2", "value2"));
    BOOST_CHECK(table.insert("key3", "value3"));
    
    BOOST_CHECK_EQUAL(table.get_size(), 3);
    BOOST_CHECK_EQUAL(table.search("key1"), "value1");
    BOOST_CHECK_EQUAL(table.search("key2"), "value2");
    BOOST_CHECK_EQUAL(table.search("key3"), "value3");
    BOOST_CHECK_EQUAL(table.search("key4"), ""); // Несуществующий ключ
}

BOOST_AUTO_TEST_CASE(DoubleHashTableInsertUpdate) {
    DoubleHashTable table(10);
    table.insert("key1", "value1");
    table.insert("key1", "value2"); // Обновление
    
    BOOST_CHECK_EQUAL(table.get_size(), 1);
    BOOST_CHECK_EQUAL(table.search("key1"), "value2");
}

BOOST_AUTO_TEST_CASE(DoubleHashTableRemove) {
    DoubleHashTable table(10);
    table.insert("key1", "value1");
    table.insert("key2", "value2");
    table.insert("key3", "value3");
    
    BOOST_CHECK(table.remove("key2"));
    BOOST_CHECK_EQUAL(table.get_size(), 2);
    BOOST_CHECK_EQUAL(table.search("key2"), "");
    BOOST_CHECK_EQUAL(table.search("key1"), "value1");
    BOOST_CHECK_EQUAL(table.search("key3"), "value3");
    
    BOOST_CHECK(!table.remove("key2")); // Уже удален
    BOOST_CHECK(!table.remove("key4")); // Никогда не существовал
}

BOOST_AUTO_TEST_CASE(DoubleHashTableCollisions) {
    DoubleHashTable table(5); // Маленькая таблица для создания коллизий
    BOOST_CHECK(table.insert("a", "1"));
    BOOST_CHECK(table.insert("b", "2"));
    BOOST_CHECK(table.insert("c", "3"));
    BOOST_CHECK(table.insert("d", "4"));
    BOOST_CHECK(table.insert("e", "5"));
    
    BOOST_CHECK_EQUAL(table.get_size(), 5);
    
    // Все должны быть найдены
    BOOST_CHECK_EQUAL(table.search("a"), "1");
    BOOST_CHECK_EQUAL(table.search("b"), "2");
    BOOST_CHECK_EQUAL(table.search("c"), "3");
    BOOST_CHECK_EQUAL(table.search("d"), "4");
    BOOST_CHECK_EQUAL(table.search("e"), "5");
}

BOOST_AUTO_TEST_CASE(DoubleHashTableRestructure) {
    DoubleHashTable table(5);
    table.insert("a", "1");
    table.insert("b", "2");
    table.insert("c", "3");
    table.insert("d", "4");
    
    // Следующая вставка должна вызвать реструктуризацию
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    table.insert("e", "5");
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("реструктуризация") != string::npos);
    BOOST_CHECK_EQUAL(table.get_size(), 5);
    BOOST_CHECK_GT(table.get_capacity(), 5); // Емкость должна увеличиться
}

BOOST_AUTO_TEST_CASE(DoubleHashTableCopyConstructor) {
    DoubleHashTable table1(10);
    table1.insert("name", "John");
    table1.insert("age", "30");
    
    DoubleHashTable table2(table1);
    BOOST_CHECK_EQUAL(table2.get_size(), 2);
    BOOST_CHECK_EQUAL(table2.search("name"), "John");
    BOOST_CHECK_EQUAL(table2.search("age"), "30");
}

BOOST_AUTO_TEST_CASE(DoubleHashTableAssignmentOperator) {
    DoubleHashTable table1(10);
    table1.insert("x", "10");
    table1.insert("y", "20");
    
    DoubleHashTable table2(5);
    table2 = table1;
    BOOST_CHECK_EQUAL(table2.get_size(), 2);
    BOOST_CHECK_EQUAL(table2.search("x"), "10");
    BOOST_CHECK_EQUAL(table2.search("y"), "20");
}

BOOST_AUTO_TEST_CASE(DoubleHashTablePrint) {
    DoubleHashTable table(5);
    table.insert("test", "value");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    
    table.print();
    
    cout.rdbuf(old_cout);
    
    BOOST_CHECK(output.str().find("Таблица с двойным хешированием") != string::npos);
    BOOST_CHECK(output.str().find("test -> value") != string::npos);
}

BOOST_AUTO_TEST_CASE(DoubleHashTableSerializeBinary) {
    DoubleHashTable table(10);
    table.insert("name", "John");
    table.insert("age", "30");
    table.insert("city", "New York");
    
    BOOST_CHECK(table.serialize_binary("test_dhash.bin"));
    
    DoubleHashTable table2(5);
    BOOST_CHECK(table2.deserialize_binary("test_dhash.bin"));
    BOOST_CHECK_EQUAL(table2.get_size(), 3);
    BOOST_CHECK_EQUAL(table2.search("name"), "John");
    BOOST_CHECK_EQUAL(table2.search("age"), "30");
    BOOST_CHECK_EQUAL(table2.search("city"), "New York");
}

BOOST_AUTO_TEST_CASE(DoubleHashTableSerializeText) {
    DoubleHashTable table(10);
    table.insert("user", "admin");
    table.insert("pass", "secret123");
    
    BOOST_CHECK(table.serialize_text("test_dhash.txt"));
    
    DoubleHashTable table2(5);
    BOOST_CHECK(table2.deserialize_text("test_dhash.txt"));
    BOOST_CHECK_EQUAL(table2.get_size(), 2);
    BOOST_CHECK_EQUAL(table2.search("user"), "admin");
    BOOST_CHECK_EQUAL(table2.search("pass"), "secret123");
}

BOOST_AUTO_TEST_CASE(DoubleHashTableStressTest) {
    const int N = 1000;
    DoubleHashTable table(1000);
    
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        table.insert("key_" + to_string(i), "value_" + to_string(i));
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "DoubleHashTable insert " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    BOOST_CHECK_EQUAL(table.get_size(), N);
    
    // Поиск всех элементов
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i += 10) {
        BOOST_CHECK_EQUAL(table.search("key_" + to_string(i)), 
                         "value_" + to_string(i));
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "DoubleHashTable search 100 элементов: " 
         << duration.count() << " ms" << endl;
    
    // Удаление половины элементов
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N/2; ++i) {
        BOOST_CHECK(table.remove("key_" + to_string(i)));
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "DoubleHashTable remove " << N/2 << " элементов: " 
         << duration.count() << " ms" << endl;
    
    BOOST_CHECK_EQUAL(table.get_size(), N/2);
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== Database Tests ====================
BOOST_AUTO_TEST_SUITE(DatabaseTests)

BOOST_AUTO_TEST_CASE(DatabaseArrayCommands) {
    Database db;
    
    // Создание массива
    BOOST_CHECK_EQUAL(db.executeCommand("MCREATE arr1"), 
                     "SUCCESS: Array created: arr1");
    BOOST_CHECK(db.hasArray("arr1"));
    
    // Добавление элементов
    BOOST_CHECK_EQUAL(db.executeCommand("MPUSH arr1 value1"), 
                     "SUCCESS: Value pushed to array");
    BOOST_CHECK_EQUAL(db.executeCommand("MPUSH arr1 value2"), 
                     "SUCCESS: Value pushed to array");
    
    // Получение элемента
    BOOST_CHECK_EQUAL(db.executeCommand("MGET arr1 0"), "VALUE: value1");
    BOOST_CHECK_EQUAL(db.executeCommand("MGET arr1 1"), "VALUE: value2");
    
    // Вставка
    BOOST_CHECK_EQUAL(db.executeCommand("MINSERT arr1 1 middle"), 
                     "SUCCESS: Value inserted at index 1");
    
    // Замена
    BOOST_CHECK_EQUAL(db.executeCommand("MREPLACE arr1 0 first"), 
                     "SUCCESS: Value replaced at index 0");
    
    // Размер
    BOOST_CHECK_EQUAL(db.executeCommand("MSIZE arr1"), "SIZE: 3");
    
    // Удаление
    BOOST_CHECK_EQUAL(db.executeCommand("MDEL arr1 1"), 
                     "SUCCESS: Element removed at index 1");
    
    // Печать
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    db.executeCommand("PRINT arr1");
    cout.rdbuf(old_cout);
    BOOST_CHECK(output.str().find("SUCCESS") != string::npos || 
                output.str().find("массив") != string::npos);
    
    // Ошибки
    BOOST_CHECK(db.executeCommand("MCREATE arr1").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("MPUSH arr2 value").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("MGET arr1 10").find("ERROR") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseSingleListCommands) {
    Database db;
    
    BOOST_CHECK_EQUAL(db.executeCommand("FCREATE list1"), 
                     "SUCCESS: Singly list created: list1");
    BOOST_CHECK(db.hasSinglyList("list1"));
    
    // Push operations
    BOOST_CHECK_EQUAL(db.executeCommand("FPUSH list1 FRONT A"), 
                     "SUCCESS: Value pushed to front");
    BOOST_CHECK_EQUAL(db.executeCommand("FPUSH list1 BACK C"), 
                     "SUCCESS: Value pushed to back");
    BOOST_CHECK_EQUAL(db.executeCommand("FPUSH list1 BEFORE C B"), 
                     "SUCCESS: Value inserted before target");
    
    // Get and search
    string result = db.executeCommand("FGET list1");
    BOOST_CHECK(result.find("LIST:") != string::npos);
    
    BOOST_CHECK_EQUAL(db.executeCommand("FGET list1 B"), "FOUND: B");
    BOOST_CHECK_EQUAL(db.executeCommand("FGET list1 X"), "NOT_FOUND");
    
    // Size
    BOOST_CHECK_EQUAL(db.executeCommand("FSIZE list1"), "SIZE: 4");
    
    // Delete operations
    BOOST_CHECK_EQUAL(db.executeCommand("FDEL list1 FRONT"), 
                     "SUCCESS: Front element removed");
    BOOST_CHECK_EQUAL(db.executeCommand("FDEL list1 BACK"), 
                     "SUCCESS: Back element removed");
    BOOST_CHECK_EQUAL(db.executeCommand("FDEL list1 VALUE B"), 
                     "SUCCESS: Value removed");
    
    // Print backward
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    db.executeCommand("FPRINT_BACKWARD list1");
    cout.rdbuf(old_cout);
    BOOST_CHECK(output.str().find("SUCCESS") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseDoubleListCommands) {
    Database db;
    
    BOOST_CHECK_EQUAL(db.executeCommand("LCREATE dlist1"), 
                     "SUCCESS: Doubly list created: dlist1");
    BOOST_CHECK(db.hasDoublyList("dlist1"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("LPUSH dlist1 FRONT A"), 
                     "SUCCESS: Value pushed to front");
    BOOST_CHECK_EQUAL(db.executeCommand("LPUSH dlist1 BACK C"), 
                     "SUCCESS: Value pushed to back");
    BOOST_CHECK_EQUAL(db.executeCommand("LPUSH dlist1 BEFORE C B"), 
                     "SUCCESS: Value inserted before target");
    
    BOOST_CHECK_EQUAL(db.executeCommand("LSIZE dlist1"), "SIZE: 3");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    db.executeCommand("LPRINT_BACKWARD dlist1");
    cout.rdbuf(old_cout);
    BOOST_CHECK(output.str().find("SUCCESS") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseStackCommands) {
    Database db;
    
    BOOST_CHECK_EQUAL(db.executeCommand("SCREATE stack1"), 
                     "SUCCESS: Stack created: stack1");
    BOOST_CHECK(db.hasStack("stack1"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("SPUSH stack1 first"), 
                     "SUCCESS: Value pushed to stack");
    BOOST_CHECK_EQUAL(db.executeCommand("SPUSH stack1 second"), 
                     "SUCCESS: Value pushed to stack");
    
    BOOST_CHECK_EQUAL(db.executeCommand("SPEEK stack1"), "PEEK: second");
    BOOST_CHECK_EQUAL(db.executeCommand("SPOP stack1"), "POPPED: second");
    BOOST_CHECK_EQUAL(db.executeCommand("SSIZE stack1"), "SIZE: 1");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    db.executeCommand("PRINT stack1");
    cout.rdbuf(old_cout);
    BOOST_CHECK(output.str().find("SUCCESS") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseQueueCommands) {
    Database db;
    
    BOOST_CHECK_EQUAL(db.executeCommand("QCREATE queue1"), 
                     "SUCCESS: Queue created: queue1");
    BOOST_CHECK(db.hasQueue("queue1"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("QPUSH queue1 first"), 
                     "SUCCESS: Value pushed to queue");
    BOOST_CHECK_EQUAL(db.executeCommand("QPUSH queue1 second"), 
                     "SUCCESS: Value pushed to queue");
    
    BOOST_CHECK_EQUAL(db.executeCommand("QPEEK queue1"), "PEEK: first");
    BOOST_CHECK_EQUAL(db.executeCommand("QPOP queue1"), "POPPED: first");
    BOOST_CHECK_EQUAL(db.executeCommand("QSIZE queue1"), "SIZE: 1");
}

BOOST_AUTO_TEST_CASE(DatabaseTreeCommands) {
    Database db;
    
    BOOST_CHECK_EQUAL(db.executeCommand("TCREATE tree1"), 
                     "SUCCESS: Tree created: tree1");
    BOOST_CHECK(db.hasTree("tree1"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("TINSERT tree1 10 root"), 
                     "SUCCESS: Value inserted with key 10");
    BOOST_CHECK_EQUAL(db.executeCommand("TINSERT tree1 5 left"), 
                     "SUCCESS: Value inserted with key 5");
    BOOST_CHECK_EQUAL(db.executeCommand("TINSERT tree1 15 right"), 
                     "SUCCESS: Value inserted with key 15");
    
    BOOST_CHECK_EQUAL(db.executeCommand("TSEARCH tree1 10"), "FOUND: root");
    BOOST_CHECK_EQUAL(db.executeCommand("TSEARCH tree1 99"), "NOT_FOUND");
    
    BOOST_CHECK_EQUAL(db.executeCommand("TSIZE tree1"), "SIZE: 3");
    BOOST_CHECK_EQUAL(db.executeCommand("THEIGHT tree1"), "HEIGHT: 2");
    BOOST_CHECK_EQUAL(db.executeCommand("TISFULL tree1"), "IS_FULL: YES");
    
    boost::test_tools::output_test_stream output;
    auto old_cout = cout.rdbuf(output.rdbuf());
    db.executeCommand("TTRAVERSE tree1 INORDER");
    cout.rdbuf(old_cout);
    BOOST_CHECK(output.str().find("SUCCESS") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseHashTableCommands) {
    Database db;
    
    // Double hash table
    BOOST_CHECK_EQUAL(db.executeCommand("HCREATE dhash1"), 
                     "SUCCESS: Double hash table created: dhash1");
    BOOST_CHECK(db.hasHashTable("dhash1"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("HINSERT dhash1 key1 value1"), 
                     "SUCCESS: Key-Value inserted");
    BOOST_CHECK_EQUAL(db.executeCommand("HSEARCH dhash1 key1"), "FOUND: value1");
    BOOST_CHECK_EQUAL(db.executeCommand("HDELETE dhash1 key1"), 
                     "SUCCESS: Key deleted");
    BOOST_CHECK_EQUAL(db.executeCommand("HSEARCH dhash1 key1"), "NOT_FOUND");

}

BOOST_AUTO_TEST_CASE(DatabaseManagementCommands) {
    Database db;
    
    // Help
    string help = db.executeCommand("HELP");
    BOOST_CHECK(help.find("COMMANDS") != string::npos);
    BOOST_CHECK(help.find("ARRAYS") != string::npos);
    BOOST_CHECK(help.find("LISTS") != string::npos);
    BOOST_CHECK(help.find("STACK") != string::npos);
    BOOST_CHECK(help.find("QUEUE") != string::npos);
    BOOST_CHECK(help.find("TREE") != string::npos);
    BOOST_CHECK(help.find("HASH") != string::npos);
    
    // List (empty)
    BOOST_CHECK_EQUAL(db.executeCommand("LIST"), 
                     "CONTAINERS:\nNo containers found.");
    
    // Create some containers
    db.executeCommand("MCREATE test_array");
    db.executeCommand("FCREATE test_list");
    db.executeCommand("SCREATE test_stack");
    
    string listOutput = db.executeCommand("LIST");
    BOOST_CHECK(listOutput.find("Arrays") != string::npos);
    BOOST_CHECK(listOutput.find("Singly Linked Lists") != string::npos);
    BOOST_CHECK(listOutput.find("Stacks") != string::npos);
    
    // Clear
    BOOST_CHECK_EQUAL(db.executeCommand("CLEAR"), "SUCCESS: Database cleared");
    BOOST_CHECK_EQUAL(db.executeCommand("LIST"), 
                     "CONTAINERS:\nNo containers found.");
    
    // Unknown command
    BOOST_CHECK(db.executeCommand("UNKNOWN_COMMAND").find("ERROR") != string::npos);
    
    // Empty command
    BOOST_CHECK_EQUAL(db.executeCommand(""), "ERROR: Empty command");
    
    // Command with insufficient arguments
    BOOST_CHECK(db.executeCommand("MCREATE").find("ERROR") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseSaveAndLoad) {
    Database db1;
    
    // Create test data
    db1.executeCommand("MCREATE arr1");
    db1.executeCommand("MPUSH arr1 data1");
    db1.executeCommand("MPUSH arr1 data2");
    
    db1.executeCommand("FCREATE list1");
    db1.executeCommand("FPUSH list1 FRONT item1");
    db1.executeCommand("FPUSH list1 BACK item2");
    
    db1.executeCommand("SCREATE stack1");
    db1.executeCommand("SPUSH stack1 top");
    
    // Save
    BOOST_CHECK_EQUAL(db1.executeCommand("SAVE test_db.txt"), 
                     "SUCCESS: Database saved to test_db.txt");
    BOOST_CHECK(fs::exists("test_db.txt"));
    
    // Load into new database
    Database db2;
    BOOST_CHECK_EQUAL(db2.executeCommand("LOAD test_db.txt"), 
                     "SUCCESS: Database loaded from test_db.txt");
    
    // Verify loaded data
    BOOST_CHECK(db2.hasArray("arr1"));
    BOOST_CHECK(db2.hasSinglyList("list1"));
    BOOST_CHECK(db2.hasStack("stack1"));
    
    const Array* arr = db2.getArray("arr1");
    BOOST_REQUIRE(arr != nullptr);
    BOOST_CHECK_EQUAL(arr->length(), 2);
    BOOST_CHECK_EQUAL(arr->get(0), "data1");
    BOOST_CHECK_EQUAL(arr->get(1), "data2");
    
    // Load non-existent file
    BOOST_CHECK(db2.executeCommand("LOAD non_existent.txt").find("ERROR") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseIntegrationTest) {
    Database db;
    
    // Use all data structures
    db.executeCommand("MCREATE my_array");
    db.executeCommand("MPUSH my_array apple");
    db.executeCommand("MPUSH my_array banana");
    db.executeCommand("MPUSH my_array cherry");
    
    db.executeCommand("FCREATE my_list");
    db.executeCommand("FPUSH my_list FRONT first");
    db.executeCommand("FPUSH my_list BACK last");
    
    db.executeCommand("LCREATE my_dlist");
    db.executeCommand("LPUSH my_dlist FRONT A");
    db.executeCommand("LPUSH my_dlist BACK B");
    
    db.executeCommand("SCREATE my_stack");
    db.executeCommand("SPUSH my_stack bottom");
    db.executeCommand("SPUSH my_stack middle");
    db.executeCommand("SPUSH my_stack top");
    
    db.executeCommand("QCREATE my_queue");
    db.executeCommand("QPUSH my_queue job1");
    db.executeCommand("QPUSH my_queue job2");
    db.executeCommand("QPUSH my_queue job3");
    
    db.executeCommand("TCREATE my_tree");
    db.executeCommand("TINSERT my_tree 50 root");
    db.executeCommand("TINSERT my_tree 25 left");
    db.executeCommand("TINSERT my_tree 75 right");
    
    db.executeCommand("HCREATE my_dhash");
    db.executeCommand("HINSERT my_dhash username alice");
    db.executeCommand("HINSERT my_dhash password secret");
    
    // Verify all created
    BOOST_CHECK(db.hasArray("my_array"));
    BOOST_CHECK(db.hasSinglyList("my_list"));
    BOOST_CHECK(db.hasDoublyList("my_dlist"));
    BOOST_CHECK(db.hasStack("my_stack"));
    BOOST_CHECK(db.hasQueue("my_queue"));
    BOOST_CHECK(db.hasTree("my_tree"));
    BOOST_CHECK(db.hasHashTable("my_dhash"));
    
    // Save and reload
    db.executeCommand("SAVE integration_test.txt");
    
    Database db2;
    db2.executeCommand("LOAD integration_test.txt");
    
    // Verify reloaded data
    BOOST_CHECK(db2.hasArray("my_array"));
    const Array* arr = db2.getArray("my_array");
    BOOST_REQUIRE(arr != nullptr);
    BOOST_CHECK_EQUAL(arr->length(), 3);
    
    BOOST_CHECK(db2.hasSinglyList("my_list"));
    BOOST_CHECK(db2.hasStack("my_stack"));
    BOOST_CHECK(db2.hasTree("my_tree"));
    
    // Cleanup
    fs::remove("integration_test.txt");
}

BOOST_AUTO_TEST_CASE(DatabasePerformanceTest) {
    Database db;
    
    // Create array with many elements
    db.executeCommand("MCREATE big_array");
    
    const int N = 1000;
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < N; ++i) {
        db.executeCommand("MPUSH big_array element_" + to_string(i));
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Database MPUSH " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    const Array* arr = db.getArray("big_array");
    BOOST_REQUIRE(arr != nullptr);
    BOOST_CHECK_EQUAL(arr->length(), N);
    
    // Test saving large database
    start = chrono::high_resolution_clock::now();
    db.executeCommand("SAVE performance_test.txt");
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Database save " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    // Test loading
    Database db2;
    start = chrono::high_resolution_clock::now();
    db2.executeCommand("LOAD performance_test.txt");
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Database load " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    BOOST_CHECK(db2.hasArray("big_array"));
    
    // Cleanup
    fs::remove("performance_test.txt");
}

BOOST_AUTO_TEST_CASE(DatabaseEdgeCases) {
    Database db;
    
    // Case insensitive commands
    BOOST_CHECK_EQUAL(db.executeCommand("mcreate test1"), 
                     "SUCCESS: Array created: test1");
    BOOST_CHECK(db.hasArray("test1"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("MCREATE test2"), 
                     "SUCCESS: Array created: test2");
    BOOST_CHECK(db.hasArray("test2"));
    
    BOOST_CHECK_EQUAL(db.executeCommand("McReAtE test3"), 
                     "SUCCESS: Array created: test3");
    BOOST_CHECK(db.hasArray("test3"));
    
    // Commands with extra spaces
    BOOST_CHECK_EQUAL(db.executeCommand("  MPUSH   test1   value1  "), 
                     "SUCCESS: Value pushed to array");
    
    // Invalid container names
    BOOST_CHECK(db.executeCommand("PRINT ").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("PRINT nonexistent").find("ERROR") != string::npos);
    
    // Invalid indices
    BOOST_CHECK(db.executeCommand("MGET test1 -1").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("MGET test1 100").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("MGET test1 not_a_number").find("ERROR") != string::npos);
    
    // Invalid command syntax
    BOOST_CHECK(db.executeCommand("MPUSH").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("MINSERT test1").find("ERROR") != string::npos);
    BOOST_CHECK(db.executeCommand("MINSERT test1 0").find("ERROR") != string::npos);
}

BOOST_AUTO_TEST_CASE(DatabaseSerializationCompatibility) {
    // Test that serialization is compatible between different instances
    Database db1;
    
    // Create complex structure
    db1.executeCommand("MCREATE compatible_array");
    for (int i = 0; i < 10; ++i) {
        db1.executeCommand("MPUSH compatible_array value_" + to_string(i));
    }
    
    db1.executeCommand("FCREATE compatible_list");
    db1.executeCommand("FPUSH compatible_list FRONT start");
    db1.executeCommand("FPUSH compatible_list BACK end");
    
    // Save in binary format
    BOOST_REQUIRE(db1.saveToFile("compatible_db.bin"));
    
    // Load in another instance
    Database db2;
    BOOST_REQUIRE(db2.loadFromFile("compatible_db.bin"));
    
    // Verify compatibility
    BOOST_CHECK(db2.hasArray("compatible_array"));
    BOOST_CHECK(db2.hasSinglyList("compatible_list"));
    
    const Array* arr = db2.getArray("compatible_array");
    BOOST_REQUIRE(arr != nullptr);
    BOOST_CHECK_EQUAL(arr->length(), 10);
    for (int i = 0; i < 10; ++i) {
        BOOST_CHECK_EQUAL(arr->get(i), "value_" + to_string(i));
    }
    
    // Cleanup
    fs::remove("compatible_db.bin");
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== Integration Tests ====================
BOOST_AUTO_TEST_SUITE(IntegrationTests)

BOOST_AUTO_TEST_CASE(AllDataStructuresSerializationRoundTrip) {
    // Create instances of all data structures
    Array arr;
    SingleList slist;
    DoubleList dlist;
    Stack stack;
    Queue queue;
    FullBinaryTree tree;
    DoubleHashTable dhash(10);
    
    // Populate with data
    for (int i = 0; i < 10; ++i) {
        string val = "value_" + to_string(i);
        arr.push_back(val);
        slist.push_back(val);
        dlist.push_back(val);
        stack.push(val);
        queue.push(val);
        tree.insert(i, val);
        dhash.insert("key_" + val, val);
    }
    
    // Serialize all to binary
    BOOST_CHECK(arr.serialize_binary("arr.bin"));
    BOOST_CHECK(slist.serialize_binary("slist.bin"));
    BOOST_CHECK(dlist.serialize_binary("dlist.bin"));
    BOOST_CHECK(stack.serialize_binary("stack.bin"));
    BOOST_CHECK(queue.serialize_binary("queue.bin"));
    BOOST_CHECK(tree.serialize_binary("tree.bin"));
    BOOST_CHECK(dhash.serialize_binary("dhash.bin"));
    
    // Create new instances and deserialize
    Array arr2;
    SingleList slist2;
    DoubleList dlist2;
    Stack stack2;
    Queue queue2;
    FullBinaryTree tree2;
    DoubleHashTable dhash2(5);
    
    BOOST_CHECK(arr2.deserialize_binary("arr.bin"));
    BOOST_CHECK(slist2.deserialize_binary("slist.bin"));
    BOOST_CHECK(dlist2.deserialize_binary("dlist.bin"));
    BOOST_CHECK(stack2.deserialize_binary("stack.bin"));
    BOOST_CHECK(queue2.deserialize_binary("queue.bin"));
    BOOST_CHECK(tree2.deserialize_binary("tree.bin"));
    BOOST_CHECK(dhash2.deserialize_binary("dhash.bin"));
    
    // Verify data integrity
    BOOST_CHECK_EQUAL(arr.length(), arr2.length());
    BOOST_CHECK_EQUAL(slist.get_size(), slist2.get_size());
    BOOST_CHECK_EQUAL(dlist.get_size(), dlist2.get_size());
    BOOST_CHECK_EQUAL(stack.get_size(), stack2.get_size());
    BOOST_CHECK_EQUAL(queue.get_size(), queue2.get_size());
    BOOST_CHECK_EQUAL(tree.get_size(), tree2.get_size());
    BOOST_CHECK_EQUAL(dhash.get_size(), dhash2.get_size());
    // Cleanup
    vector<string> files = {"arr.bin", "slist.bin", "dlist.bin", "stack.bin", 
                           "queue.bin", "tree.bin", "dhash.bin"};
    for (const auto& file : files) {
        fs::remove(file);
    }
}

BOOST_AUTO_TEST_CASE(DatabaseComplexScenario) {
    Database db;
    
    // Simulate a real-world scenario
    db.executeCommand("MCREATE students");
    db.executeCommand("MPUSH students Alice");
    db.executeCommand("MPUSH students Bob");
    db.executeCommand("MPUSH students Charlie");
    db.executeCommand("MINSERT students 1 David");
    
    db.executeCommand("FCREATE courses");
    db.executeCommand("FPUSH courses FRONT Math");
    db.executeCommand("FPUSH courses BACK Physics");
    db.executeCommand("FPUSH courses AFTER Math CS");
    
    db.executeCommand("SCREATE browser_history");
    db.executeCommand("SPUSH browser_history google.com");
    db.executeCommand("SPUSH browser_history github.com");
    db.executeCommand("SPUSH browser_history stackoverflow.com");
    db.executeCommand("SPOP browser_history"); // Back button
    
    db.executeCommand("QCREATE print_queue");
    db.executeCommand("QPUSH print_queue document1.pdf");
    db.executeCommand("QPUSH print_queue document2.docx");
    db.executeCommand("QPUSH print_queue image.png");
    db.executeCommand("QPOP print_queue"); // Print first document
    
    db.executeCommand("TCREATE file_system");
    db.executeCommand("TINSERT file_system 100 /");
    db.executeCommand("TINSERT file_system 50 /home");
    db.executeCommand("TINSERT file_system 150 /etc");
    db.executeCommand("TINSERT file_system 25 /home/user1");
    db.executeCommand("TINSERT file_system 75 /home/user2");
    
    db.executeCommand("HCREATE user_db");
    db.executeCommand("HINSERT user_db alice password123");
    db.executeCommand("HINSERT user_db bob secret456");
    db.executeCommand("HINSERT user_db charlie qwerty789");
    
    db.executeCommand("CCREATE session_cache");
    db.executeCommand("CINSERT session_cache session1 user_data_alice");
    db.executeCommand("CINSERT session_cache session2 user_data_bob");
    
    // Verify the state
    BOOST_CHECK_EQUAL(db.executeCommand("MSIZE students"), "SIZE: 4");
    BOOST_CHECK_EQUAL(db.executeCommand("FSIZE courses"), "SIZE: 3");
    BOOST_CHECK_EQUAL(db.executeCommand("SSIZE browser_history"), "SIZE: 2");
    BOOST_CHECK_EQUAL(db.executeCommand("QSIZE print_queue"), "SIZE: 2");
    BOOST_CHECK_EQUAL(db.executeCommand("TSIZE file_system"), "SIZE: 5");
    BOOST_CHECK_EQUAL(db.executeCommand("HSIZE user_db"), "SIZE: 3");
    BOOST_CHECK_EQUAL(db.executeCommand("CSIZE session_cache"), "SIZE: 2");
    
    // Save and restore
    db.executeCommand("SAVE complex_scenario.txt");
    
    Database db2;
    db2.executeCommand("LOAD complex_scenario.txt");
    
    // Verify restored state
    BOOST_CHECK(db2.hasArray("students"));
    BOOST_CHECK(db2.hasSinglyList("courses"));
    BOOST_CHECK(db2.hasStack("browser_history"));
    BOOST_CHECK(db2.hasQueue("print_queue"));
    BOOST_CHECK(db2.hasTree("file_system"));
    BOOST_CHECK(db2.hasHashTable("user_db"));
    
    // Cleanup
    fs::remove("complex_scenario.txt");
}

BOOST_AUTO_TEST_CASE(PerformanceComparison) {
    cout << "\n=== Сравнение производительности структур данных ===" << endl;
    
    // Test Array performance
    {
        Array arr;
        const int N = 10000;
        
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            arr.push_back("item_" + to_string(i));
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "Array push_back " << N << " элементов: " 
             << duration.count() << " μs" << endl;
    }
    
    // Test SingleList performance
    {
        SingleList list;
        const int N = 10000;
        
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            list.push_back("item_" + to_string(i));
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "SingleList push_back " << N << " элементов: " 
             << duration.count() << " μs" << endl;
    }
    
    // Test DoubleHashTable performance
    {
        DoubleHashTable table(1000);
        const int N = 1000;
        
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            table.insert("key_" + to_string(i), "value_" + to_string(i));
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "DoubleHashTable insert " << N << " элементов: " 
             << duration.count() << " μs" << endl;
    }
    
    // Test serialization performance
    {
        Array arr;
        const int N = 1000;
        for (int i = 0; i < N; ++i) {
            arr.push_back(generateRandomString(100));
        }
        
        auto start = chrono::high_resolution_clock::now();
        arr.serialize_binary("perf_serialize.bin");
        auto end = chrono::high_resolution_clock::now();
        auto serialize_time = chrono::duration_cast<chrono::microseconds>(end - start);
        
        start = chrono::high_resolution_clock::now();
        Array arr2;
        arr2.deserialize_binary("perf_serialize.bin");
        end = chrono::high_resolution_clock::now();
        auto deserialize_time = chrono::duration_cast<chrono::microseconds>(end - start);
        
        cout << "Сериализация " << N << " строк: " << serialize_time.count() << " μs" << endl;
        cout << "Десериализация " << N << " строк: " << deserialize_time.count() << " μs" << endl;
        
        fs::remove("perf_serialize.bin");
    }
    
    BOOST_CHECK(true); // Все тесты производительности прошли
}

BOOST_AUTO_TEST_CASE(MemoryManagementTest) {
    // Test for memory leaks by creating and destroying many objects
    const int ITERATIONS = 1000;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        {
            Array* arr = new Array();
            for (int j = 0; j < 100; ++j) {
                arr->push_back("test_" + to_string(j));
            }
            delete arr;
        }
        
        {
            SingleList* list = new SingleList();
            for (int j = 0; j < 100; ++j) {
                list->push_back("node_" + to_string(j));
            }
            delete list;
        }
        
        {
            FullBinaryTree* tree = new FullBinaryTree();
            for (int j = 0; j < 50; ++j) {
                tree->insert(j, "tree_node_" + to_string(j));
            }
            delete tree;
        }
        
        {
            DoubleHashTable* table = new DoubleHashTable(100);
            for (int j = 0; j < 50; ++j) {
                table->insert("key_" + to_string(j), "val_" + to_string(j));
            }
            delete table;
        }
    }
    
    // If we get here without crashing, memory management is working
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(ConcurrentAccessTest) {
    // Test that structures can be used in different threads
    Array shared_array;
    const int N = 1000;
    
    // Function to add elements
    auto add_elements = [&shared_array](int start, int count) {
        for (int i = 0; i < count; ++i) {
            shared_array.push_back("thread_" + to_string(start + i));
        }
    };
    
    // Create multiple threads
    vector<thread> threads;
    int elements_per_thread = N / 4;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(add_elements, i * elements_per_thread, elements_per_thread);
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Check total number of elements
    BOOST_CHECK_EQUAL(shared_array.length(), N);
    
    // Note: This test doesn't guarantee thread safety, just that the 
    // structures can be used in multi-threaded environments without crashing
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== Special Cases Tests ====================
BOOST_AUTO_TEST_SUITE(SpecialCasesTests)

BOOST_AUTO_TEST_CASE(EmptyStructuresOperations) {
    // Test operations on empty structures
    Array empty_arr;
    BOOST_CHECK_EQUAL(empty_arr.length(), 0);
    BOOST_CHECK_EQUAL(empty_arr.get(0), "");
    BOOST_CHECK(!empty_arr.remove(0));
    BOOST_CHECK(!empty_arr.replace(0, "test"));
    
    SingleList empty_list;
    BOOST_CHECK_EQUAL(empty_list.get_size(), 0);
    BOOST_CHECK(!empty_list.pop_front());
    BOOST_CHECK(!empty_list.pop_back());
    BOOST_CHECK(!empty_list.remove_value("anything"));
    BOOST_CHECK(empty_list.find_first() == nullptr);
    
    Stack empty_stack;
    BOOST_CHECK(empty_stack.is_empty());
    BOOST_CHECK_EQUAL(empty_stack.pop(), "");
    BOOST_CHECK_EQUAL(empty_stack.peek(), "");
    
    Queue empty_queue;
    BOOST_CHECK(empty_queue.is_empty());
    BOOST_CHECK_EQUAL(empty_queue.pop(), "");
    BOOST_CHECK_EQUAL(empty_queue.peek(), "");
    
    FullBinaryTree empty_tree;
    BOOST_CHECK_EQUAL(empty_tree.get_size(), 0);
    BOOST_CHECK_EQUAL(empty_tree.search(1), "");
    BOOST_CHECK(empty_tree.is_full());
    BOOST_CHECK_EQUAL(empty_tree.height(), 0);
    
    DoubleHashTable empty_table(10);
    BOOST_CHECK_EQUAL(empty_table.get_size(), 0);
    BOOST_CHECK_EQUAL(empty_table.search("any"), "");
    BOOST_CHECK(!empty_table.remove("any"));
    
    // Empty serialization/deserialization
    BOOST_CHECK(empty_arr.serialize_binary("empty.bin"));
    BOOST_CHECK(empty_arr.serialize_text("empty.txt"));
    
    Array loaded_arr;
    BOOST_CHECK(loaded_arr.deserialize_binary("empty.bin"));
    BOOST_CHECK_EQUAL(loaded_arr.length(), 0);
    
    BOOST_CHECK(loaded_arr.deserialize_text("empty.txt"));
    BOOST_CHECK_EQUAL(loaded_arr.length(), 0);
    
    fs::remove("empty.bin");
    fs::remove("empty.txt");
}

BOOST_AUTO_TEST_CASE(SingleElementStructures) {
    // Test structures with exactly one element
    Array single_arr;
    single_arr.push_back("only");
    BOOST_CHECK_EQUAL(single_arr.length(), 1);
    BOOST_CHECK_EQUAL(single_arr.get(0), "only");
    BOOST_CHECK(single_arr.remove(0));
    BOOST_CHECK_EQUAL(single_arr.length(), 0);
    
    SingleList single_list;
    single_list.push_back("sole");
    BOOST_CHECK_EQUAL(single_list.get_size(), 1);
    BOOST_CHECK_EQUAL(single_list.find_first()->data, "sole");
    BOOST_CHECK(single_list.pop_front());
    BOOST_CHECK_EQUAL(single_list.get_size(), 0);
    
    Stack single_stack;
    single_stack.push("top");
    BOOST_CHECK_EQUAL(single_stack.get_size(), 1);
    BOOST_CHECK_EQUAL(single_stack.peek(), "top");
    BOOST_CHECK_EQUAL(single_stack.pop(), "top");
    BOOST_CHECK(single_stack.is_empty());
    
    // ... аналогично для других структур
}

BOOST_AUTO_TEST_CASE(LargeDataStructures) {
    // Test with very large data structures
    const int LARGE_N = 100000;
    
    Array large_array;
    for (int i = 0; i < LARGE_N; ++i) {
        large_array.push_back("data_" + to_string(i));
    }
    BOOST_CHECK_EQUAL(large_array.length(), LARGE_N);
    
    // Access elements at different positions
    BOOST_CHECK_EQUAL(large_array.get(0), "data_0");
    BOOST_CHECK_EQUAL(large_array.get(LARGE_N/2), "data_" + to_string(LARGE_N/2));
    BOOST_CHECK_EQUAL(large_array.get(LARGE_N-1), "data_" + to_string(LARGE_N-1));
    
    // Remove many elements
    for (int i = 0; i < LARGE_N/2; ++i) {
        BOOST_CHECK(large_array.remove(0));
    }
    BOOST_CHECK_EQUAL(large_array.length(), LARGE_N/2);
    
    cout << "Large array test completed with " << LARGE_N << " elements" << endl;
}

BOOST_AUTO_TEST_CASE(SpecialCharactersData) {
    // Test with special characters and Unicode
    Array arr;
    string special = "Line with:\nNewline\tTab\rCarriage return\\Backslash\"Quote'";
    arr.push_back(special);
    
    string unicode = "Unicode: café München €20 αβγ 日本語 русский язык";
    arr.push_back(unicode);
    
    string emoji = "Emoji: 😀🎉🚀";
    arr.push_back(emoji);
    
    BOOST_CHECK_EQUAL(arr.length(), 3);
    
    // Serialize and deserialize
    BOOST_CHECK(arr.serialize_binary("special.bin"));
    BOOST_CHECK(arr.serialize_text("special.txt"));
    
    Array arr_bin;
    BOOST_CHECK(arr_bin.deserialize_binary("special.bin"));
    BOOST_CHECK_EQUAL(arr_bin.get(0), special);
    BOOST_CHECK_EQUAL(arr_bin.get(1), unicode);
    BOOST_CHECK_EQUAL(arr_bin.get(2), emoji);
    
    Array arr_txt;
    BOOST_CHECK(arr_txt.deserialize_text("special.txt"));
    BOOST_CHECK_EQUAL(arr_txt.get(0), special);
    BOOST_CHECK_EQUAL(arr_txt.get(1), unicode);
    BOOST_CHECK_EQUAL(arr_txt.get(2), emoji);
    
    fs::remove("special.bin");
    fs::remove("special.txt");
}

BOOST_AUTO_TEST_CASE(CorruptedFilesHandling) {
    // Test handling of corrupted files
    // Create corrupted binary file
    {
        ofstream file("corrupted.bin", ios::binary);
        file.write("NOT_VALID_BINARY_DATA", 21);
        file.close();
        
        Array arr;
        BOOST_CHECK(!arr.deserialize_binary("corrupted.bin"));
    }
    
    // Create corrupted text file
    {
        ofstream file("corrupted.txt");
        file << "not a valid size" << endl;
        file << "some data" << endl;
        file.close();
        
        Array arr;
        BOOST_CHECK(!arr.deserialize_text("corrupted.txt"));
    }
    
    // Empty file
    {
        ofstream file("empty.txt");
        file.close();
        
        Array arr;
        BOOST_CHECK(!arr.deserialize_text("empty.txt"));
    }
    
    // File with wrong format
    {
        ofstream file("wrong_format.txt");
        file << "10" << endl; // Size
        // But no data lines
        file.close();
        
        Array arr;
        BOOST_CHECK(!arr.deserialize_text("wrong_format.txt"));
    }
    
    fs::remove("corrupted.bin");
    fs::remove("corrupted.txt");
    fs::remove("empty.txt");
    fs::remove("wrong_format.txt");
}

BOOST_AUTO_TEST_CASE(BoundaryValueTests) {
    // Test boundary values
    Array arr;
    
    // Insert at boundary positions
    arr.push_back("first");
    BOOST_CHECK(arr.insert(0, "before_first")); // Вставка перед первым
    BOOST_CHECK(arr.insert(arr.length(), "after_last")); // Вставка после последнего
    
    BOOST_CHECK_EQUAL(arr.length(), 3);
    BOOST_CHECK_EQUAL(arr.get(0), "before_first");
    BOOST_CHECK_EQUAL(arr.get(2), "after_last");
    
    // Remove boundary elements
    BOOST_CHECK(arr.remove(0)); // Удаление первого
    BOOST_CHECK(arr.remove(arr.length() - 1)); // Удаление последнего
    
    BOOST_CHECK_EQUAL(arr.length(), 1);
    BOOST_CHECK_EQUAL(arr.get(0), "first");
    
    // Get boundary indices
    BOOST_CHECK_EQUAL(arr.get(0), "first");
    BOOST_CHECK_EQUAL(arr.get(-1), ""); // Перед первым
    BOOST_CHECK_EQUAL(arr.get(arr.length()), ""); // После последнего
    
    // Replace boundary elements
    BOOST_CHECK(arr.replace(0, "replaced"));
    BOOST_CHECK_EQUAL(arr.get(0), "replaced");
    BOOST_CHECK(!arr.replace(-1, "invalid"));
    BOOST_CHECK(!arr.replace(arr.length(), "invalid"));
}

BOOST_AUTO_TEST_SUITE_END()

// ==================== Main Test Runner ====================
BOOST_AUTO_TEST_SUITE(AllTests)

BOOST_AUTO_TEST_CASE(RunAllTests) {
    // This is just a placeholder to run all tests
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()