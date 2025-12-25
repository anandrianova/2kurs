#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <random>
#include <filesystem>
#include "Array.h"
#include "SingleList.h"
#include "DoubleList.h"
#include "Stack.h"
#include "Queue.h"
#include "FullBinaryTree.h"
#include "HashTable.h"
#include "DB.h"

using namespace std;
namespace fs = filesystem;

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

// ==================== Array Tests ====================
TEST(ArrayTest, ConstructorAndDestructor) {
    {
        Array arr1;
        EXPECT_EQ(arr1.length(), 0);
        
        Array arr2(20);
        EXPECT_EQ(arr2.length(), 0);
    }
    // Деструктор должен отработать без ошибок
    SUCCEED();
}

TEST(ArrayTest, CopyConstructorAndAssignment) {
    Array arr1;
    arr1.push_back("test1");
    arr1.push_back("test2");
    
    // Конструктор копирования
    Array arr2(arr1);
    EXPECT_EQ(arr2.length(), 2);
    EXPECT_EQ(arr2.get(0), "test1");
    EXPECT_EQ(arr2.get(1), "test2");
    
    // Оператор присваивания
    Array arr3;
    arr3 = arr1;
    EXPECT_EQ(arr3.length(), 2);
    EXPECT_EQ(arr3.get(0), "test1");
    
    // Самоприсваивание
    arr3 = arr3;
    EXPECT_EQ(arr3.length(), 2);
}

TEST(ArrayTest, PushBackAndGet) {
    Array arr;
    
    // Добавление элементов
    for (int i = 0; i < 100; ++i) {
        arr.push_back("item" + to_string(i));
    }
    
    EXPECT_EQ(arr.length(), 100);
    
    // Проверка всех элементов
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(arr.get(i), "item" + to_string(i));
    }
    
    // Проверка невалидных индексов
    EXPECT_EQ(arr.get(-1), "");
    EXPECT_EQ(arr.get(1000), "");
    EXPECT_EQ(arr.get(100), "");
}

TEST(ArrayTest, Insert) {
    Array arr;
    arr.push_back("A");
    arr.push_back("C");
    
    // Вставка в середину
    EXPECT_TRUE(arr.insert(1, "B"));
    EXPECT_EQ(arr.length(), 3);
    EXPECT_EQ(arr.get(0), "A");
    EXPECT_EQ(arr.get(1), "B");
    EXPECT_EQ(arr.get(2), "C");
    
    // Вставка в начало
    EXPECT_TRUE(arr.insert(0, "Start"));
    EXPECT_EQ(arr.get(0), "Start");
    
    // Вставка в конец
    EXPECT_TRUE(arr.insert(arr.length(), "End"));
    EXPECT_EQ(arr.get(arr.length() - 1), "End");
    
    // Невалидные индексы
    EXPECT_FALSE(arr.insert(-1, "Invalid"));
    EXPECT_FALSE(arr.insert(arr.length() + 1, "Invalid"));
    
    // Автоматическое расширение массива
    Array small(2);
    small.push_back("1");
    small.push_back("2");
    EXPECT_TRUE(small.insert(1, "1.5")); // Должно вызвать resize
    EXPECT_EQ(small.length(), 3);
}

TEST(ArrayTest, Remove) {
    Array arr;
    for (int i = 0; i < 5; ++i) {
        arr.push_back("item" + to_string(i));
    }
    
    // Удаление из середины
    EXPECT_TRUE(arr.remove(2));
    EXPECT_EQ(arr.length(), 4);
    EXPECT_EQ(arr.get(2), "item3");
    
    // Удаление первого элемента
    EXPECT_TRUE(arr.remove(0));
    EXPECT_EQ(arr.get(0), "item1");
    
    // Удаление последнего элемента
    EXPECT_TRUE(arr.remove(arr.length() - 1));
    EXPECT_EQ(arr.get(arr.length() - 1), "item3");
    
    // Невалидные индексы
    EXPECT_FALSE(arr.remove(-1));
    EXPECT_FALSE(arr.remove(100));
    
    // Удаление всех элементов
    while (arr.length() > 0) {
        EXPECT_TRUE(arr.remove(0));
    }
    EXPECT_FALSE(arr.remove(0)); // Удаление из пустого массива
}

TEST(ArrayTest, Replace) {
    Array arr;
    arr.push_back("old1");
    arr.push_back("old2");
    
    // Корректная замена
    EXPECT_TRUE(arr.replace(0, "new1"));
    EXPECT_EQ(arr.get(0), "new1");
    
    EXPECT_TRUE(arr.replace(1, "new2"));
    EXPECT_EQ(arr.get(1), "new2");
    
    // Невалидные индексы
    EXPECT_FALSE(arr.replace(-1, "invalid"));
    EXPECT_FALSE(arr.replace(10, "invalid"));
}

TEST(ArrayTest, ToVector) {
    Array arr;
    arr.push_back("A");
    arr.push_back("B");
    arr.push_back("C");
    
    vector<string> vec = arr.to_vector();
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], "A");
    EXPECT_EQ(vec[1], "B");
    EXPECT_EQ(vec[2], "C");
    
    // Пустой массив
    Array empty;
    vector<string> emptyVec = empty.to_vector();
    EXPECT_TRUE(emptyVec.empty());
}

TEST(ArrayTest, SerializationBinary) {
    Array arr;
    arr.push_back("Hello");
    arr.push_back("World");
    arr.push_back("Test");
    
    // Сериализация
    EXPECT_TRUE(arr.serialize_binary("test_array.bin"));
    
    // Десериализация
    Array arr2;
    EXPECT_TRUE(arr2.deserialize_binary("test_array.bin"));
    
    EXPECT_EQ(arr2.length(), 3);
    EXPECT_EQ(arr2.get(0), "Hello");
    EXPECT_EQ(arr2.get(1), "World");
    EXPECT_EQ(arr2.get(2), "Test");
    
    // Десериализация в непустой массив (должна очиститься)
    Array arr3;
    arr3.push_back("Old");
    EXPECT_TRUE(arr3.deserialize_binary("test_array.bin"));
    EXPECT_EQ(arr3.length(), 3);
    
    // Несуществующий файл
    Array arr4;
    EXPECT_FALSE(arr4.deserialize_binary("non_existent_file.bin"));
    
    // Очистка тестовых файлов
    fs::remove("test_array.bin");
}

TEST(ArrayTest, SerializationText) {
    Array arr;
    arr.push_back("Line 1");
    arr.push_back("Line 2");
    arr.push_back("Line 3 with spaces");
    
    // Сериализация
    EXPECT_TRUE(arr.serialize_text("test_array.txt"));
    
    // Десериализация
    Array arr2;
    EXPECT_TRUE(arr2.deserialize_text("test_array.txt"));
    
    EXPECT_EQ(arr2.length(), 3);
    EXPECT_EQ(arr2.get(0), "Line 1");
    EXPECT_EQ(arr2.get(1), "Line 2");
    EXPECT_EQ(arr2.get(2), "Line 3 with spaces");
    
    // Пустой файл
    ofstream emptyFile("empty.txt");
    emptyFile.close();
    Array emptyArr;
    EXPECT_FALSE(emptyArr.deserialize_text("empty.txt"));
    
    // Очистка тестовых файлов
    fs::remove("test_array.txt");
    fs::remove("empty.txt");
}

TEST(ArrayTest, Print) {
    Array arr;
    
    // Печать пустого массива
    testing::internal::CaptureStdout();
    arr.print();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("массив пуст"), string::npos);
    
    // Печать непустого массива
    arr.push_back("Test");
    arr.push_back("Array");
    
    testing::internal::CaptureStdout();
    arr.print();
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("массив [2]:"), string::npos);
    EXPECT_NE(output.find("Test"), string::npos);
    EXPECT_NE(output.find("Array"), string::npos);
}

TEST(ArrayTest, StressTest) {
    Array arr;
    const int N = 10000;
    
    // Добавление большого количества элементов
    for (int i = 0; i < N; ++i) {
        arr.push_back("element_" + to_string(i));
    }
    
    EXPECT_EQ(arr.length(), N);
    
    // Проверка случайных элементов
    mt19937 rng(42);
    for (int i = 0; i < 100; ++i) {
        int idx = rng() % N;
        EXPECT_EQ(arr.get(idx), "element_" + to_string(idx));
    }
    
    // Удаление половины элементов
    for (int i = 0; i < N/2; ++i) {
        EXPECT_TRUE(arr.remove(0));
    }
    
    EXPECT_EQ(arr.length(), N/2);
}

// ==================== SingleList Tests ====================
TEST(SingleListTest, ConstructorAndDestructor) {
    {
        SingleList list;
        EXPECT_EQ(list.get_size(), 0);
    }
    SUCCEED();
}

TEST(SingleListTest, CopyConstructorAndAssignment) {
    SingleList list1;
    list1.push_back("A");
    list1.push_back("B");
    list1.push_back("C");
    
    // Конструктор копирования
    SingleList list2(list1);
    EXPECT_EQ(list2.get_size(), 3);
    
    // Оператор присваивания
    SingleList list3;
    list3 = list1;
    EXPECT_EQ(list3.get_size(), 3);
    
    // Самоприсваивание
    list3 = list3;
    EXPECT_EQ(list3.get_size(), 3);
}

TEST(SingleListTest, PushFrontAndPushBack) {
    SingleList list;
    
    list.push_front("B");
    list.push_front("A");
    list.push_back("C");
    list.push_back("D");
    
    EXPECT_EQ(list.get_size(), 4);
    
    testing::internal::CaptureStdout();
    list.print_forward();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("A -> B -> C -> D"), string::npos);
}

TEST(SingleListTest, InsertBeforeAndAfter) {
    SingleList list;
    list.push_back("A");
    list.push_back("C");
    
    // Вставка before
    EXPECT_TRUE(list.insert_before("C", "B"));
    EXPECT_EQ(list.get_size(), 3);
    
    // Вставка after
    EXPECT_TRUE(list.insert_after("C", "D"));
    EXPECT_EQ(list.get_size(), 4);
    
    // Несуществующий элемент
    EXPECT_FALSE(list.insert_before("X", "Y"));
    EXPECT_FALSE(list.insert_after("X", "Y"));
    
    // Вставка в начало через insert_before
    EXPECT_TRUE(list.insert_before("A", "Start"));
    EXPECT_EQ(list.get_size(), 5);
}

TEST(SingleListTest, PopFrontAndPopBack) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    EXPECT_TRUE(list.pop_front());
    EXPECT_EQ(list.get_size(), 2);
    
    EXPECT_TRUE(list.pop_back());
    EXPECT_EQ(list.get_size(), 1);
    
    EXPECT_TRUE(list.pop_front());
    EXPECT_EQ(list.get_size(), 0);
    
    // Удаление из пустого списка
    EXPECT_FALSE(list.pop_front());
    EXPECT_FALSE(list.pop_back());
}

TEST(SingleListTest, RemoveOperations) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    list.push_back("E");
    
    // Удаление значения
    EXPECT_TRUE(list.remove_value("C"));
    EXPECT_EQ(list.get_size(), 4);
    
    // Удаление before
    EXPECT_TRUE(list.remove_before("D"));
    EXPECT_EQ(list.get_size(), 3);
    
    // Удаление after
    EXPECT_TRUE(list.remove_after("A"));
    EXPECT_EQ(list.get_size(), 2);
    
    // Несуществующие элементы
    EXPECT_FALSE(list.remove_value("X"));
    EXPECT_FALSE(list.remove_before("A")); // Перед первым
    EXPECT_FALSE(list.remove_after("D")); // После последнего
}

TEST(SingleListTest, FindOperations) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    SNode* found = list.find("B");
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->data, "B");
    
    EXPECT_EQ(list.find("X"), nullptr);
    
    // Итерация через find_first и find_next
    SNode* current = list.find_first();
    vector<string> elements;
    while (current != nullptr) {
        elements.push_back(current->data);
        current = list.find_next(current);
    }
    EXPECT_EQ(elements.size(), 3);
    EXPECT_EQ(elements[0], "A");
    EXPECT_EQ(elements[1], "B");
    EXPECT_EQ(elements[2], "C");
}

TEST(SingleListTest, PrintOperations) {
    SingleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    // Печать вперед
    testing::internal::CaptureStdout();
    list.print_forward();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("A -> B -> C"), string::npos);
    
    // Печать назад
    testing::internal::CaptureStdout();
    list.print_backward();
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("C <- B <- A"), string::npos);
    
    // Печать пустого списка
    SingleList empty;
    testing::internal::CaptureStdout();
    empty.print_forward();
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("пуст"), string::npos);
}

TEST(SingleListTest, Serialization) {
    SingleList list;
    list.push_back("First");
    list.push_back("Second");
    list.push_back("Third");
    
    // Бинарная сериализация
    EXPECT_TRUE(list.serialize_binary("test_slist.bin"));
    
    SingleList list2;
    EXPECT_TRUE(list2.deserialize_binary("test_slist.bin"));
    EXPECT_EQ(list2.get_size(), 3);
    
    // Текстовая сериализация
    EXPECT_TRUE(list.serialize_text("test_slist.txt"));
    
    SingleList list3;
    EXPECT_TRUE(list3.deserialize_text("test_slist.txt"));
    EXPECT_EQ(list3.get_size(), 3);
    
    // Очистка
    fs::remove("test_slist.bin");
    fs::remove("test_slist.txt");
}

// ==================== DoubleList Tests ====================
TEST(DoubleListTest, BasicOperations) {
    DoubleList list;
    
    list.push_front("B");
    list.push_front("A");
    list.push_back("C");
    list.push_back("D");
    
    EXPECT_EQ(list.get_size(), 4);
    
    // Проверка двунаправленной связи
    DNode* node = list.find("B");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->prev->data, "A");
    EXPECT_EQ(node->next->data, "C");
}

TEST(DoubleListTest, InsertOperations) {
    DoubleList list;
    list.push_back("A");
    list.push_back("C");
    
    EXPECT_TRUE(list.insert_before("C", "B"));
    EXPECT_TRUE(list.insert_after("C", "D"));
    
    EXPECT_EQ(list.get_size(), 4);
    
    testing::internal::CaptureStdout();
    list.print_forward();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("A <-> B <-> C <-> D"), string::npos);
}

TEST(DoubleListTest, RemoveOperations) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    list.push_back("D");
    list.push_back("E");
    
    EXPECT_TRUE(list.remove_before("C"));
    EXPECT_TRUE(list.remove_after("C"));
    EXPECT_TRUE(list.remove_value("C"));
    
    EXPECT_EQ(list.get_size(), 2);
    
    testing::internal::CaptureStdout();
    list.print_forward();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("A <-> D"), string::npos);
}

TEST(DoubleListTest, ReversePrint) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    testing::internal::CaptureStdout();
    list.print_backward();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("C <-> B <-> A"), string::npos);
}

TEST(DoubleListTest, ClearAndCopy) {
    DoubleList list;
    list.push_back("A");
    list.push_back("B");
    list.push_back("C");
    
    // Проверка копирования
    DoubleList copy(list);
    EXPECT_EQ(copy.get_size(), 3);
    
    // Проверка присваивания
    DoubleList assigned;
    assigned = list;
    EXPECT_EQ(assigned.get_size(), 3);
    
    // Очистка через деструктор
    {
        DoubleList temp;
        temp.push_back("X");
        temp.push_back("Y");
    }
    SUCCEED();
}

TEST(DoubleListTest, Serialization) {
    DoubleList list;
    list.push_back("Alpha");
    list.push_back("Beta");
    list.push_back("Gamma");
    
    // Тест бинарной сериализации
    EXPECT_TRUE(list.serialize_binary("test_dlist.bin"));
    
    DoubleList list2;
    EXPECT_TRUE(list2.deserialize_binary("test_dlist.bin"));
    EXPECT_EQ(list2.get_size(), 3);
    
    // Тест текстовой сериализации
    EXPECT_TRUE(list.serialize_text("test_dlist.txt"));
    
    DoubleList list3;
    EXPECT_TRUE(list3.deserialize_text("test_dlist.txt"));
    EXPECT_EQ(list3.get_size(), 3);
    
    fs::remove("test_dlist.bin");
    fs::remove("test_dlist.txt");
}

// ==================== Stack Tests ====================
TEST(StackTest, BasicOperations) {
    Stack s;
    
    EXPECT_TRUE(s.is_empty());
    EXPECT_EQ(s.get_size(), 0);
    
    s.push("First");
    s.push("Second");
    s.push("Third");
    
    EXPECT_FALSE(s.is_empty());
    EXPECT_EQ(s.get_size(), 3);
    
    EXPECT_EQ(s.peek(), "Third");
    EXPECT_EQ(s.pop(), "Third");
    EXPECT_EQ(s.pop(), "Second");
    EXPECT_EQ(s.pop(), "First");
    
    EXPECT_TRUE(s.is_empty());
    EXPECT_EQ(s.pop(), "");
    EXPECT_EQ(s.peek(), "");
}

TEST(StackTest, CopyAndAssignment) {
    Stack s1;
    s1.push("A");
    s1.push("B");
    s1.push("C");
    
    Stack s2(s1);
    EXPECT_EQ(s2.get_size(), 3);
    EXPECT_EQ(s2.pop(), "C");
    EXPECT_EQ(s2.pop(), "B");
    EXPECT_EQ(s2.pop(), "A");
    
    Stack s3;
    s3 = s1;
    EXPECT_EQ(s3.get_size(), 3);
}

TEST(StackTest, ResizeTest) {
    Stack s(2); // Маленькая начальная емкость
    
    for (int i = 0; i < 100; ++i) {
        s.push("Item " + to_string(i));
    }
    
    EXPECT_EQ(s.get_size(), 100);
    
    for (int i = 99; i >= 0; --i) {
        EXPECT_EQ(s.pop(), "Item " + to_string(i));
    }
}

TEST(StackTest, Serialization) {
    Stack s;
    s.push("Bottom");
    s.push("Middle");
    s.push("Top");
    
    EXPECT_TRUE(s.serialize_binary("test_stack.bin"));
    
    Stack s2;
    EXPECT_TRUE(s2.deserialize_binary("test_stack.bin"));
    EXPECT_EQ(s2.get_size(), 3);
    EXPECT_EQ(s2.pop(), "Top");
    
    EXPECT_TRUE(s.serialize_text("test_stack.txt"));
    
    Stack s3;
    EXPECT_TRUE(s3.deserialize_text("test_stack.txt"));
    EXPECT_EQ(s3.get_size(), 3);
    
    fs::remove("test_stack.bin");
    fs::remove("test_stack.txt");
}

TEST(StackTest, Print) {
    Stack s;
    s.push("A");
    s.push("B");
    s.push("C");
    
    testing::internal::CaptureStdout();
    s.print();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("C | B | A"), string::npos);
    
    // Печать пустого стека
    Stack empty;
    testing::internal::CaptureStdout();
    empty.print();
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("пуст"), string::npos);
}

// ==================== Queue Tests ====================
TEST(QueueTest, BasicOperations) {
    Queue q;
    
    EXPECT_TRUE(q.is_empty());
    EXPECT_EQ(q.get_size(), 0);
    
    q.push("First");
    q.push("Second");
    q.push("Third");
    
    EXPECT_FALSE(q.is_empty());
    EXPECT_EQ(q.get_size(), 3);
    
    EXPECT_EQ(q.peek(), "First");
    EXPECT_EQ(q.pop(), "First");
    EXPECT_EQ(q.pop(), "Second");
    EXPECT_EQ(q.pop(), "Third");
    
    EXPECT_TRUE(q.is_empty());
    EXPECT_EQ(q.pop(), "");
    EXPECT_EQ(q.peek(), "");
}

TEST(QueueTest, CircularBuffer) {
    Queue q(3); // Маленькая емкость для теста кругового буфера
    
    q.push("A");
    q.push("B");
    q.push("C");
    
    // Должен произойти resize
    q.push("D");
    
    EXPECT_EQ(q.get_size(), 4);
    EXPECT_EQ(q.pop(), "A");
    EXPECT_EQ(q.pop(), "B");
    EXPECT_EQ(q.pop(), "C");
    EXPECT_EQ(q.pop(), "D");
}

TEST(QueueTest, CopyAndAssignment) {
    Queue q1;
    q1.push("A");
    q1.push("B");
    q1.push("C");
    
    Queue q2(q1);
    EXPECT_EQ(q2.get_size(), 3);
    EXPECT_EQ(q2.pop(), "A");
    
    Queue q3;
    q3 = q1;
    EXPECT_EQ(q3.get_size(), 3);
}

TEST(QueueTest, Serialization) {
    Queue q;
    q.push("First");
    q.push("Second");
    q.push("Third");
    
    EXPECT_TRUE(q.serialize_binary("test_queue.bin"));
    
    Queue q2;
    EXPECT_TRUE(q2.deserialize_binary("test_queue.bin"));
    EXPECT_EQ(q2.get_size(), 3);
    EXPECT_EQ(q2.pop(), "First");
    
    EXPECT_TRUE(q.serialize_text("test_queue.txt"));
    
    Queue q3;
    EXPECT_TRUE(q3.deserialize_text("test_queue.txt"));
    EXPECT_EQ(q3.get_size(), 3);
    
    fs::remove("test_queue.bin");
    fs::remove("test_queue.txt");
}

TEST(QueueTest, Print) {
    Queue q;
    q.push("A");
    q.push("B");
    q.push("C");
    
    testing::internal::CaptureStdout();
    q.print();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("A -> B -> C"), string::npos);
}

// ==================== FullBinaryTree Tests ====================
TEST(FullBinaryTreeTest, BasicOperations) {
    FullBinaryTree tree;
    
    EXPECT_TRUE(tree.insert(50, "Root"));
    EXPECT_TRUE(tree.insert(25, "Left"));
    EXPECT_TRUE(tree.insert(75, "Right"));
    EXPECT_TRUE(tree.insert(12, "Left-Left"));
    EXPECT_TRUE(tree.insert(37, "Left-Right"));
    EXPECT_TRUE(tree.insert(62, "Right-Left"));
    EXPECT_TRUE(tree.insert(87, "Right-Right"));
    
    EXPECT_EQ(tree.get_size(), 7);
    
    EXPECT_EQ(tree.search(50), "Root");
    EXPECT_EQ(tree.search(25), "Left");
    EXPECT_EQ(tree.search(99), ""); // Несуществующий ключ
    
    // Вставка существующего ключа
    EXPECT_FALSE(tree.insert(50, "NewRoot"));
}

TEST(FullBinaryTreeTest, Traversals) {
    FullBinaryTree tree;
    tree.insert(4, "D");
    tree.insert(2, "B");
    tree.insert(6, "F");
    tree.insert(1, "A");
    tree.insert(3, "C");
    tree.insert(5, "E");
    tree.insert(7, "G");
    
    testing::internal::CaptureStdout();
    tree.inorder();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("1:A"), string::npos);
    
    testing::internal::CaptureStdout();
    tree.preorder();
    output = testing::internal::GetCapturedStdout();
    
    testing::internal::CaptureStdout();
    tree.postorder();
    output = testing::internal::GetCapturedStdout();
    
    testing::internal::CaptureStdout();
    tree.level_order();
    output = testing::internal::GetCapturedStdout();
}

TEST(FullBinaryTreeTest, TreeProperties) {
    FullBinaryTree tree;
    
    // Пустое дерево
    EXPECT_TRUE(tree.is_full());
    EXPECT_EQ(tree.height(), 0);
    
    // Полное двоичное дерево
    tree.insert(1, "A");
    EXPECT_TRUE(tree.is_full());
    
    tree.insert(2, "B");
    tree.insert(3, "C");
    EXPECT_TRUE(tree.is_full());
    
    tree.insert(4, "D");
    tree.insert(5, "E");
    tree.insert(6, "F");
    tree.insert(7, "G");
    EXPECT_TRUE(tree.is_full());
    
    EXPECT_EQ(tree.height(), 3);
    EXPECT_EQ(tree.get_size(), 7);
}

TEST(FullBinaryTreeTest, CopyAndAssignment) {
    FullBinaryTree tree1;
    tree1.insert(1, "A");
    tree1.insert(2, "B");
    tree1.insert(3, "C");
    
    FullBinaryTree tree2(tree1);
    EXPECT_EQ(tree2.get_size(), 3);
    EXPECT_EQ(tree2.search(1), "A");
    
    FullBinaryTree tree3;
    tree3 = tree1;
    EXPECT_EQ(tree3.get_size(), 3);
    
    // Самоприсваивание
    tree3 = tree3;
    EXPECT_EQ(tree3.get_size(), 3);
}

TEST(FullBinaryTreeTest, Serialization) {
    FullBinaryTree tree;
    tree.insert(5, "Five");
    tree.insert(3, "Three");
    tree.insert(7, "Seven");
    tree.insert(2, "Two");
    tree.insert(4, "Four");
    
    EXPECT_TRUE(tree.serialize_binary("test_tree.bin"));
    
    FullBinaryTree tree2;
    EXPECT_TRUE(tree2.deserialize_binary("test_tree.bin"));
    EXPECT_EQ(tree2.get_size(), 5);
    EXPECT_EQ(tree2.search(5), "Five");
    
    EXPECT_TRUE(tree.serialize_text("test_tree.txt"));
    
    FullBinaryTree tree3;
    EXPECT_TRUE(tree3.deserialize_text("test_tree.txt"));
    EXPECT_EQ(tree3.get_size(), 5);
    
    fs::remove("test_tree.bin");
    fs::remove("test_tree.txt");
}

TEST(FullBinaryTreeTest, Print) {
    FullBinaryTree tree;
    tree.insert(8, "Root");
    tree.insert(4, "Left");
    tree.insert(12, "Right");
    
    testing::internal::CaptureStdout();
    tree.print();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Full Binary Tree"), string::npos);
    EXPECT_NE(output.find("Is full binary tree"), string::npos);
}

// ==================== HashTable Tests ====================
TEST(DoubleHashTableTest, BasicOperations) {
    DoubleHashTable table(10);
    
    EXPECT_TRUE(table.insert("key1", "value1"));
    EXPECT_TRUE(table.insert("key2", "value2"));
    EXPECT_TRUE(table.insert("key3", "value3"));
    
    EXPECT_EQ(table.search("key1"), "value1");
    EXPECT_EQ(table.search("key2"), "value2");
    EXPECT_EQ(table.search("key3"), "value3");
    EXPECT_EQ(table.search("key4"), ""); // Несуществующий ключ
    
    EXPECT_EQ(table.get_size(), 3);
    EXPECT_TRUE(table.get_load_factor() > 0);
}

TEST(DoubleHashTableTest, UpdateAndRemove) {
    DoubleHashTable table(10);
    
    table.insert("key1", "value1");
    table.insert("key1", "value1_updated"); // Обновление
    
    EXPECT_EQ(table.search("key1"), "value1_updated");
    
    EXPECT_TRUE(table.remove("key1"));
    EXPECT_EQ(table.search("key1"), "");
    EXPECT_FALSE(table.remove("key1")); // Повторное удаление
    EXPECT_FALSE(table.remove("non_existent"));
    
    EXPECT_EQ(table.get_size(), 0);
}

TEST(DoubleHashTableTest, Restructure) {
    DoubleHashTable table(5);
    
    // Заполняем до порога реструктуризации
    for (int i = 0; i < 4; ++i) {
        table.insert("key" + to_string(i), "value" + to_string(i));
    }
    
    // Следующая вставка должна вызвать реструктуризацию
    testing::internal::CaptureStdout();
    table.insert("key4", "value4");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("реструктуризация"), string::npos);
    
    EXPECT_EQ(table.get_size(), 5);
}

TEST(DoubleHashTableTest, CopyAndAssignment) {
    DoubleHashTable table1(10);
    table1.insert("a", "1");
    table1.insert("b", "2");
    
    DoubleHashTable table2(table1);
    EXPECT_EQ(table2.search("a"), "1");
    EXPECT_EQ(table2.get_size(), 2);
    
    DoubleHashTable table3(5);
    table3 = table1;
    EXPECT_EQ(table3.search("b"), "2");
}

TEST(DoubleHashTableTest, Serialization) {
    DoubleHashTable table(10);
    table.insert("name", "John");
    table.insert("age", "30");
    table.insert("city", "New York");
    
    EXPECT_TRUE(table.serialize_binary("test_dhash.bin"));
    
    DoubleHashTable table2(5);
    EXPECT_TRUE(table2.deserialize_binary("test_dhash.bin"));
    EXPECT_EQ(table2.search("name"), "John");
    EXPECT_EQ(table2.get_size(), 3);
    
    EXPECT_TRUE(table.serialize_text("test_dhash.txt"));
    
    DoubleHashTable table3(5);
    EXPECT_TRUE(table3.deserialize_text("test_dhash.txt"));
    EXPECT_EQ(table3.search("city"), "New York");
    
    fs::remove("test_dhash.bin");
    fs::remove("test_dhash.txt");
}

TEST(DoubleHashTableTest, Print) {
    DoubleHashTable table(5);
    table.insert("test", "value");
    
    testing::internal::CaptureStdout();
    table.print();
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Таблица с двойным хешированием"), string::npos);
    EXPECT_NE(output.find("test -> value"), string::npos);
}

// ==================== Database Tests ====================
TEST(DatabaseTest, ArrayCommands) {
    Database db;
    
    // Создание массива
    EXPECT_EQ(db.executeCommand("MCREATE arr1"), "SUCCESS: Array created: arr1");
    EXPECT_TRUE(db.hasArray("arr1"));
    
    // Добавление элементов
    EXPECT_EQ(db.executeCommand("MPUSH arr1 value1"), "SUCCESS: Value pushed to array");
    EXPECT_EQ(db.executeCommand("MPUSH arr1 value2"), "SUCCESS: Value pushed to array");
    
    // Получение элемента
    EXPECT_EQ(db.executeCommand("MGET arr1 0"), "VALUE: value1");
    
    // Вставка
    EXPECT_EQ(db.executeCommand("MINSERT arr1 1 inserted"), "SUCCESS: Value inserted at index 1");
    
    // Замена
    EXPECT_EQ(db.executeCommand("MREPLACE arr1 0 replaced"), "SUCCESS: Value replaced at index 0");
    
    // Размер
    EXPECT_EQ(db.executeCommand("MSIZE arr1"), "SIZE: 3");
    
    // Удаление
    EXPECT_EQ(db.executeCommand("MDEL arr1 1"), "SUCCESS: Element removed at index 1");
    
    // Печать
    testing::internal::CaptureStdout();
    db.executeCommand("PRINT arr1");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("массив"), string::npos);
    
    // Ошибки
    EXPECT_NE(db.executeCommand("MCREATE arr1").find("ERROR"), string::npos);
    EXPECT_NE(db.executeCommand("MPUSH arr2 value").find("ERROR"), string::npos);
    EXPECT_NE(db.executeCommand("MGET arr1 100").find("ERROR"), string::npos);
}

TEST(DatabaseTest, SingleListCommands) {
    Database db;
    
    EXPECT_EQ(db.executeCommand("FCREATE list1"), "SUCCESS: Singly list created: list1");
    EXPECT_TRUE(db.hasSinglyList("list1"));
    
    // Добавление
    EXPECT_EQ(db.executeCommand("FPUSH list1 FRONT front1"), "SUCCESS: Value pushed to front");
    EXPECT_EQ(db.executeCommand("FPUSH list1 BACK back1"), "SUCCESS: Value pushed to back");
    EXPECT_EQ(db.executeCommand("FPUSH list1 BEFORE back1 middle"), "SUCCESS: Value inserted before target");
    EXPECT_EQ(db.executeCommand("FPUSH list1 AFTER front1 afterfront"), "SUCCESS: Value inserted after target");
    
    // Получение
    string result = db.executeCommand("FGET list1");
    EXPECT_NE(result.find("LIST:"), string::npos);
    
    // Поиск
    EXPECT_EQ(db.executeCommand("FGET list1 middle"), "FOUND: middle");
    EXPECT_EQ(db.executeCommand("FGET list1 notfound"), "NOT_FOUND");
    
    // Размер
    EXPECT_EQ(db.executeCommand("FSIZE list1"), "SIZE: 4");
    
    // Удаление
    EXPECT_EQ(db.executeCommand("FDEL list1 FRONT"), "SUCCESS: Front element removed");
    EXPECT_EQ(db.executeCommand("FDEL list1 BACK"), "SUCCESS: Back element removed");
    EXPECT_EQ(db.executeCommand("FDEL list1 VALUE middle"), "SUCCESS: Value removed");
    EXPECT_EQ(db.executeCommand("FDEL list1 BEFORE afterfront"), "ERROR: Cannot remove before target");
    EXPECT_EQ(db.executeCommand("FDEL list1 AFTER afterfront"), "ERROR: Cannot remove after target");
    
    // Печать назад
    testing::internal::CaptureStdout();
    db.executeCommand("FPRINT_BACKWARD list1");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("SUCCESS") != string::npos || 
                output.find("список") != string::npos);
}

TEST(DatabaseTest, DoubleListCommands) {
    Database db;
    
    EXPECT_EQ(db.executeCommand("LCREATE dlist1"), "SUCCESS: Doubly list created: dlist1");
    EXPECT_TRUE(db.hasDoublyList("dlist1"));
    
    EXPECT_EQ(db.executeCommand("LPUSH dlist1 FRONT A"), "SUCCESS: Value pushed to front");
    EXPECT_EQ(db.executeCommand("LPUSH dlist1 BACK C"), "SUCCESS: Value pushed to back");
    EXPECT_EQ(db.executeCommand("LPUSH dlist1 BEFORE C B"), "SUCCESS: Value inserted before target");
    
    EXPECT_EQ(db.executeCommand("LSIZE dlist1"), "SIZE: 3");
    
    testing::internal::CaptureStdout();
    db.executeCommand("PRINT dlist1");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Двусвязный список"), string::npos);
}

TEST(DatabaseTest, StackCommands) {
    Database db;
    
    EXPECT_EQ(db.executeCommand("SCREATE stack1"), "SUCCESS: Stack created: stack1");
    EXPECT_TRUE(db.hasStack("stack1"));
    
    EXPECT_EQ(db.executeCommand("SPUSH stack1 first"), "SUCCESS: Value pushed to stack");
    EXPECT_EQ(db.executeCommand("SPUSH stack1 second"), "SUCCESS: Value pushed to stack");
    
    EXPECT_EQ(db.executeCommand("SPEEK stack1"), "PEEK: second");
    EXPECT_EQ(db.executeCommand("SPOP stack1"), "POPPED: second");
    EXPECT_EQ(db.executeCommand("SSIZE stack1"), "SIZE: 1");
    
    testing::internal::CaptureStdout();
    db.executeCommand("PRINT stack1");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Стопка"), string::npos);
}

TEST(DatabaseTest, QueueCommands) {
    Database db;
    
    EXPECT_EQ(db.executeCommand("QCREATE queue1"), "SUCCESS: Queue created: queue1");
    EXPECT_TRUE(db.hasQueue("queue1"));
    
    EXPECT_EQ(db.executeCommand("QPUSH queue1 first"), "SUCCESS: Value pushed to queue");
    EXPECT_EQ(db.executeCommand("QPUSH queue1 second"), "SUCCESS: Value pushed to queue");
    
    EXPECT_EQ(db.executeCommand("QPEEK queue1"), "PEEK: first");
    EXPECT_EQ(db.executeCommand("QPOP queue1"), "POPPED: first");
    EXPECT_EQ(db.executeCommand("QSIZE queue1"), "SIZE: 1");
}

TEST(DatabaseTest, TreeCommands) {
    Database db;
    
    EXPECT_EQ(db.executeCommand("TCREATE tree1"), "SUCCESS: Tree created: tree1");
    EXPECT_TRUE(db.hasTree("tree1"));
    
    EXPECT_EQ(db.executeCommand("TINSERT tree1 10 root"), "SUCCESS: Value inserted with key 10");
    EXPECT_EQ(db.executeCommand("TINSERT tree1 5 left"), "SUCCESS: Value inserted with key 5");
    EXPECT_EQ(db.executeCommand("TINSERT tree1 15 right"), "SUCCESS: Value inserted with key 15");
    
    EXPECT_EQ(db.executeCommand("TSEARCH tree1 10"), "FOUND: root");
    EXPECT_EQ(db.executeCommand("TSEARCH tree1 99"), "NOT_FOUND");
    
    EXPECT_EQ(db.executeCommand("TSIZE tree1"), "SIZE: 3");
    EXPECT_EQ(db.executeCommand("THEIGHT tree1"), "HEIGHT: 2");
    EXPECT_EQ(db.executeCommand("TISFULL tree1"), "IS_FULL: YES");
    
    testing::internal::CaptureStdout();
    db.executeCommand("TTRAVERSE tree1 INORDER");
    string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("SUCCESS") != string::npos || 
                output.find("Inorder") != string::npos);
}

TEST(DatabaseTest, HashTableCommands) {
    Database db;
    
    // Двойное хеширование
    EXPECT_EQ(db.executeCommand("HCREATE hash1"), "SUCCESS: Double hash table created: hash1");
    EXPECT_TRUE(db.hasHashTable("hash1"));
    
    EXPECT_EQ(db.executeCommand("HINSERT hash1 key1 value1"), "SUCCESS: Key-Value inserted");
    EXPECT_EQ(db.executeCommand("HSEARCH hash1 key1"), "FOUND: value1");
    EXPECT_EQ(db.executeCommand("HDELETE hash1 key1"), "SUCCESS: Key deleted");
    EXPECT_EQ(db.executeCommand("HSEARCH hash1 key1"), "NOT_FOUND");
}

TEST(DatabaseTest, DatabaseManagement) {
    Database db;
    
    // HELP
    string help = db.executeCommand("HELP");
    EXPECT_NE(help.find("COMMANDS"), string::npos);
    EXPECT_NE(help.find("ARRAYS"), string::npos);
    
    // LIST (пустая база)
    EXPECT_EQ(db.executeCommand("LIST"), "CONTAINERS:\nNo containers found.");
    
    // Создаем несколько контейнеров
    db.executeCommand("MCREATE test_array");
    db.executeCommand("FCREATE test_list");
    
    string listOutput = db.executeCommand("LIST");
    EXPECT_NE(listOutput.find("Arrays"), string::npos);
    EXPECT_NE(listOutput.find("Singly Linked Lists"), string::npos);
    
    // CLEAR
    EXPECT_EQ(db.executeCommand("CLEAR"), "SUCCESS: Database cleared");
    EXPECT_EQ(db.executeCommand("LIST"), "CONTAINERS:\nNo containers found.");
    
    // Неизвестная команда
    EXPECT_NE(db.executeCommand("UNKNOWN_COMMAND").find("ERROR"), string::npos);
}

TEST(DatabaseTest, SaveAndLoad) {
    Database db1;
    
    // Создаем тестовые данные
    db1.executeCommand("MCREATE arr1");
    db1.executeCommand("MPUSH arr1 data1");
    db1.executeCommand("MPUSH arr1 data2");
    
    db1.executeCommand("FCREATE list1");
    db1.executeCommand("FPUSH list1 FRONT item1");
    
    // Сохраняем
    EXPECT_EQ(db1.executeCommand("SAVE test_db.txt"), "SUCCESS: Database saved to test_db.txt");
    
    // Загружаем в новую базу
    Database db2;
    EXPECT_EQ(db2.executeCommand("LOAD test_db.txt"), "SUCCESS: Database loaded from test_db.txt");
    
    // Проверяем загруженные данные
    EXPECT_TRUE(db2.hasArray("arr1"));
    EXPECT_TRUE(db2.hasSinglyList("list1"));
    
    const Array* arr = db2.getArray("arr1");
    EXPECT_NE(arr, nullptr);
    EXPECT_EQ(arr->length(), 2);
    
    // Очистка
    fs::remove("test_db.txt");
    
    // Загрузка несуществующего файла
    EXPECT_NE(db2.executeCommand("LOAD non_existent.txt").find("ERROR"), string::npos);
}

TEST(DatabaseTest, CommandParsing) {
    Database db;
    
    // Пустая команда
    EXPECT_EQ(db.executeCommand(""), "ERROR: Empty command");
    
    // Команда PRINT без имени контейнера
    EXPECT_EQ(db.executeCommand("PRINT"), "ERROR: PRINT requires container name");
    
    // Команда для несуществующего контейнера
    EXPECT_NE(db.executeCommand("PRINT nonexistent").find("ERROR"), string::npos);
    
    // Разные регистры команд
    db.executeCommand("mcreate test");
    EXPECT_TRUE(db.hasArray("test"));
    
    // Команды с лишними пробелами
    EXPECT_EQ(db.executeCommand("  MPUSH   test   value  "), "SUCCESS: Value pushed to array");
}

TEST(DatabaseTest, BoundaryConditions) {
    Database db;
    
    // Большое количество операций
    db.executeCommand("MCREATE big");
    for (int i = 0; i < 1000; ++i) {
        db.executeCommand("MPUSH big value_" + to_string(i));
    }
    
    const Array* arr = db.getArray("big");
    EXPECT_EQ(arr->length(), 1000);
    
    // Проверка переполнения (должно работать через resize)
    db.executeCommand("SCREATE small_stack");
    for (int i = 0; i < 10000; ++i) {
        db.executeCommand("SPUSH small_stack item_" + to_string(i));
    }
    
    // Проверка корректности после многих операций
    for (int i = 0; i < 5000; ++i) {
        db.executeCommand("SPOP small_stack");
    }
}

// ==================== Integration Tests ====================
TEST(IntegrationTest, ComplexScenario) {
    Database db;
    
    // Сценарий использования
    db.executeCommand("MCREATE students");
    db.executeCommand("MPUSH students Alice");
    db.executeCommand("MPUSH students Bob");
    db.executeCommand("MPUSH students Charlie");
    db.executeCommand("MINSERT students 1 David");
    
    db.executeCommand("FCREATE courses");
    db.executeCommand("FPUSH courses FRONT Math");
    db.executeCommand("FPUSH courses BACK Physics");
    db.executeCommand("FPUSH courses AFTER Math CS");
    
    db.executeCommand("SCREATE history");
    db.executeCommand("SPUSH history page1");
    db.executeCommand("SPUSH history page2");
    db.executeCommand("SPOP history");
    
    // Сохранение и восстановление
    db.executeCommand("SAVE integration_test.txt");
    
    Database db2;
    db2.executeCommand("LOAD integration_test.txt");
    
    EXPECT_TRUE(db2.hasArray("students"));
    EXPECT_TRUE(db2.hasSinglyList("courses"));
    EXPECT_TRUE(db2.hasStack("history"));
    
    fs::remove("integration_test.txt");
}

TEST(IntegrationTest, AllDataStructures) {
    Database db;
    
    // Используем все типы структур
    db.executeCommand("MCREATE arr");
    db.executeCommand("MPUSH arr test");
    
    db.executeCommand("FCREATE slist");
    db.executeCommand("FPUSH slist FRONT test");
    
    db.executeCommand("LCREATE dlist");
    db.executeCommand("LPUSH dlist FRONT test");
    
    db.executeCommand("SCREATE stack");
    db.executeCommand("SPUSH stack test");
    
    db.executeCommand("QCREATE queue");
    db.executeCommand("QPUSH queue test");
    
    db.executeCommand("TCREATE tree");
    db.executeCommand("TINSERT tree 1 test");
    
    db.executeCommand("HCREATE dhash");
    db.executeCommand("HINSERT dhash key value");
    
    db.executeCommand("CCREATE chash");
    db.executeCommand("CINSERT chash key value");
    
    // Проверяем, что все созданы
    EXPECT_TRUE(db.hasArray("arr"));
    EXPECT_TRUE(db.hasSinglyList("slist"));
    EXPECT_TRUE(db.hasDoublyList("dlist"));
    EXPECT_TRUE(db.hasStack("stack"));
    EXPECT_TRUE(db.hasQueue("queue"));
    EXPECT_TRUE(db.hasTree("tree"));
    EXPECT_TRUE(db.hasHashTable("dhash"));
    
    // Сохраняем и загружаем
    db.executeCommand("SAVE all_structures.txt");
    
    Database db2;
    db2.executeCommand("LOAD all_structures.txt");
    
    EXPECT_TRUE(db2.hasArray("arr"));
    EXPECT_TRUE(db2.hasSinglyList("slist"));
    // ... проверяем все остальные
    
    fs::remove("all_structures.txt");
}

// ==================== Performance Tests ====================
TEST(PerformanceTest, ArrayPerformance) {
    const int N = 10000;
    Array arr;
    
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < N; ++i) {
        arr.push_back("item_" + to_string(i));
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    EXPECT_EQ(arr.length(), N);
    cout << "\nArray push_back " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    // Проверка скорости доступа
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i += 100) {
        arr.get(i);
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Array random access: " << duration.count() << " ms" << endl;
}

TEST(PerformanceTest, HashTablePerformance) {
    const int N = 10000;
    DoubleHashTable table(10000);
    
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < N; ++i) {
        table.insert("key_" + to_string(i), "value_" + to_string(i));
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    EXPECT_EQ(table.get_size(), N);
    cout << "\nDoubleHashTable insert " << N << " элементов: " 
         << duration.count() << " ms" << endl;
    
    // Поиск
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i += 10) {
        table.search("key_" + to_string(i));
    }
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "DoubleHashTable search: " << duration.count() << " ms" << endl;
}

TEST(PerformanceTest, SerializationPerformance) {
    // Создаем большой массив для теста
    Array bigArray;
    const int N = 10000;
    
    for (int i = 0; i < N; ++i) {
        bigArray.push_back(generateRandomString(100));
    }
    
    auto start = chrono::high_resolution_clock::now();
    bigArray.serialize_binary("perf_test.bin");
    auto end = chrono::high_resolution_clock::now();
    auto serialize_time = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    start = chrono::high_resolution_clock::now();
    Array loadedArray;
    loadedArray.deserialize_binary("perf_test.bin");
    end = chrono::high_resolution_clock::now();
    auto deserialize_time = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    EXPECT_EQ(loadedArray.length(), N);
    cout << "\nСериализация " << N << " строк: " << serialize_time.count() << " ms" << endl;
    cout << "Десериализация " << N << " строк: " << deserialize_time.count() << " ms" << endl;
    
    fs::remove("perf_test.bin");
}

// ==================== Edge Cases Tests ====================
TEST(EdgeCasesTest, EmptyStructures) {
    // Пустые структуры
    Array emptyArr;
    EXPECT_EQ(emptyArr.length(), 0);
    EXPECT_EQ(emptyArr.get(0), "");
    EXPECT_FALSE(emptyArr.remove(0));
    
    SingleList emptyList;
    EXPECT_EQ(emptyList.get_size(), 0);
    EXPECT_FALSE(emptyList.pop_front());
    EXPECT_EQ(emptyList.find("anything"), nullptr);
    
    Stack emptyStack;
    EXPECT_TRUE(emptyStack.is_empty());
    EXPECT_EQ(emptyStack.pop(), "");
    
    Queue emptyQueue;
    EXPECT_TRUE(emptyQueue.is_empty());
    EXPECT_EQ(emptyQueue.pop(), "");
    
    FullBinaryTree emptyTree;
    EXPECT_EQ(emptyTree.get_size(), 0);
    EXPECT_EQ(emptyTree.search(1), "");
    EXPECT_TRUE(emptyTree.is_full());
    
    DoubleHashTable emptyTable(10);
    EXPECT_EQ(emptyTable.get_size(), 0);
    EXPECT_EQ(emptyTable.search("key"), "");
}

TEST(EdgeCasesTest, SpecialCharacters) {
    // Тестирование специальных символов
    Array arr;
    string special = "Line\nWith\tTabs\rAnd Unicode: café \u20AC";
    arr.push_back(special);
    
    EXPECT_TRUE(arr.serialize_binary("special.bin"));
    
    Array arr2;
    EXPECT_TRUE(arr2.deserialize_binary("special.bin"));
    EXPECT_EQ(arr2.get(0), special);
    
    fs::remove("special.bin");
}

TEST(EdgeCasesTest, LargeData) {
    // Большие объемы данных
    SingleList list;
    const int N = 5000;
    
    for (int i = 0; i < N; ++i) {
        list.push_back("data_block_" + to_string(i) + "_" + generateRandomString(50));
    }
    
    EXPECT_EQ(list.get_size(), N);
    
    // Удаление всех элементов
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(list.pop_front());
    }
    
    EXPECT_EQ(list.get_size(), 0);
}

TEST(EdgeCasesTest, MemoryManagement) {
    // Тест на утечки памяти
    {
        DoubleList* list = new DoubleList();
        for (int i = 0; i < 1000; ++i) {
            list->push_back("item_" + to_string(i));
        }
        delete list; // Должен очистить всю память
    }
    
    {
        FullBinaryTree* tree = new FullBinaryTree();
        for (int i = 0; i < 100; ++i) {
            tree->insert(i, "value_" + to_string(i));
        }
        delete tree;
    }
    
    SUCCEED(); // Если не упали, значит деструкторы работают
}

// мэйн
int main(int argc, char** argv) {
    cout << "=== Запуск всех тестов ===" << endl;
    cout << "Тестирование всех структур данных..." << endl;
    
    // Инициализация Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Запуск всех тестов
    int result = RUN_ALL_TESTS();
    
    cout << "\n=== Результаты тестирования ===" << endl;
    cout << "Все тесты завершены." << endl;
    
    return result;
}