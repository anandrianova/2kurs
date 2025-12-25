#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <sstream>
#include "Array.h"
#include "SingleList.h"
#include "DoubleList.h"
#include "Stack.h"
#include "Queue.h"
#include "FullBinaryTree.h"
#include "HashTable.h"
#include "DB.h"

using namespace std;

// ========== ARRAY TESTS ==========
TEST_CASE("Array - Basic operations", "[array]") {
    Array arr;
    
    SECTION("Empty array") {
        REQUIRE(arr.length() == 0);
    }
    
    SECTION("Push back and get") {
        arr.push_back("A");
        arr.push_back("B");
        arr.push_back("C");
        
        REQUIRE(arr.length() == 3);
        REQUIRE(arr.get(0) == "A");
        REQUIRE(arr.get(1) == "B");
        REQUIRE(arr.get(2) == "C");
    }
    
    SECTION("Insert at position") {
        arr.push_back("A");
        arr.push_back("C");
        
        REQUIRE(arr.insert(1, "B"));
        REQUIRE(arr.length() == 3);
        REQUIRE(arr.get(0) == "A");
        REQUIRE(arr.get(1) == "B");
        REQUIRE(arr.get(2) == "C");
        
        // Invalid insert
        REQUIRE_FALSE(arr.insert(10, "D"));
        REQUIRE_FALSE(arr.insert(-1, "D"));
    }
    
    SECTION("Remove element") {
        arr.push_back("A");
        arr.push_back("B");
        arr.push_back("C");
        
        REQUIRE(arr.remove(1));
        REQUIRE(arr.length() == 2);
        REQUIRE(arr.get(0) == "A");
        REQUIRE(arr.get(1) == "C");
        
        // Invalid remove
        REQUIRE_FALSE(arr.remove(10));
        REQUIRE_FALSE(arr.remove(-1));
    }
    
    SECTION("Replace element") {
        arr.push_back("A");
        arr.push_back("B");
        arr.push_back("C");
        
        REQUIRE(arr.replace(1, "X"));
        REQUIRE(arr.get(1) == "X");
        
        // Invalid replace
        REQUIRE_FALSE(arr.replace(10, "Y"));
        REQUIRE_FALSE(arr.replace(-1, "Y"));
    }
    
    SECTION("Print array") {
        arr.push_back("Hello");
        arr.push_back("World");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        arr.print();
        std::cout.rdbuf(old_buf);
        
        REQUIRE(ss.str().find("Hello") != string::npos);
        REQUIRE(ss.str().find("World") != string::npos);
    }
    
    SECTION("Serialization") {
        arr.push_back("Test1");
        arr.push_back("Test2");
        arr.push_back("Test3");
        
        REQUIRE(arr.serialize_binary("test_array.bin"));
        REQUIRE(arr.serialize_text("test_array.txt"));
        
        Array arr2;
        REQUIRE(arr2.deserialize_binary("test_array.bin"));
        REQUIRE(arr2.length() == 3);
        REQUIRE(arr2.get(1) == "Test2");
        
        Array arr3;
        REQUIRE(arr3.deserialize_text("test_array.txt"));
        REQUIRE(arr3.length() == 3);
        REQUIRE(arr3.get(2) == "Test3");
        
        // Cleanup
        remove("test_array.bin");
        remove("test_array.txt");
    }
}

// ========== SINGLE LINKED LIST TESTS ==========
TEST_CASE("SingleList - Basic operations", "[singlelist]") {
    SingleList list;
    
    SECTION("Empty list") {
        REQUIRE(list.get_size() == 0);
        REQUIRE(list.find_first() == nullptr);
    }
    
    SECTION("Push front and back") {
        list.push_front("B");
        list.push_front("A");
        list.push_back("C");
        
        REQUIRE(list.get_size() == 3);
        
        auto node = list.find_first();
        REQUIRE(node->data == "A");
        node = list.find_next(node);
        REQUIRE(node->data == "B");
        node = list.find_next(node);
        REQUIRE(node->data == "C");
    }
    
    SECTION("Insert before and after") {
        list.push_back("A");
        list.push_back("C");
        
        REQUIRE(list.insert_before("C", "B"));
        REQUIRE(list.insert_after("B", "D"));
        
        REQUIRE(list.get_size() == 4);
        
        auto node = list.find_first();
        REQUIRE(node->data == "A");
        node = list.find_next(node);
        REQUIRE(node->data == "B");
        node = list.find_next(node);
        REQUIRE(node->data == "D");
        node = list.find_next(node);
        REQUIRE(node->data == "C");
    }
    
    SECTION("Find and remove") {
        list.push_back("A");
        list.push_back("B");
        list.push_back("C");
        
        auto found = list.find("B");
        REQUIRE(found != nullptr);
        REQUIRE(found->data == "B");
        
        REQUIRE(list.remove_value("B"));
        REQUIRE(list.get_size() == 2);
        REQUIRE(list.find("B") == nullptr);
        
        REQUIRE(list.remove_before("C"));
        REQUIRE(list.remove_after("A"));
        REQUIRE(list.get_size() == 1);
    }
    
    SECTION("Pop operations") {
        list.push_back("A");
        list.push_back("B");
        list.push_back("C");
        
        REQUIRE(list.pop_front());
        REQUIRE(list.get_size() == 2);
        REQUIRE(list.find_first()->data == "B");
        
        REQUIRE(list.pop_back());
        REQUIRE(list.get_size() == 1);
        REQUIRE(list.find_first()->data == "B");
    }
    
    SECTION("Serialization") {
        list.push_back("Node1");
        list.push_back("Node2");
        list.push_back("Node3");
        
        REQUIRE(list.serialize_text("test_slist.txt"));
        REQUIRE(list.serialize_binary("test_slist.bin"));
        
        SingleList list2;
        REQUIRE(list2.deserialize_text("test_slist.txt"));
        REQUIRE(list2.get_size() == 3);
        
        SingleList list3;
        REQUIRE(list3.deserialize_binary("test_slist.bin"));
        REQUIRE(list3.get_size() == 3);
        
        // Cleanup
        remove("test_slist.txt");
        remove("test_slist.bin");
    }
}

// ========== DOUBLE LINKED LIST TESTS ==========
TEST_CASE("DoubleList - Basic operations", "[doublelist]") {
    DoubleList list;
    
    SECTION("Empty list") {
        REQUIRE(list.get_size() == 0);
        REQUIRE(list.find_first() == nullptr);
        // Убрали find_last(), если его нет в вашем классе
    }
    
    SECTION("Push operations") {
        list.push_front("B");
        list.push_front("A");
        list.push_back("C");
        
        REQUIRE(list.get_size() == 3);
        REQUIRE(list.find_first()->data == "A");
        // Проверяем последний элемент через обход списка
        auto node = list.find_first();
        auto last = node;
        while (node) {
            last = node;
            node = list.find_next(node);
        }
        REQUIRE(last->data == "C");
    }
    
    SECTION("Insert operations") {
        list.push_back("A");
        list.push_back("C");
        
        REQUIRE(list.insert_before("C", "B"));
        REQUIRE(list.insert_after("B", "D"));
        
        REQUIRE(list.get_size() == 4);
        
        auto node = list.find_first();
        REQUIRE(node->data == "A");
        node = list.find_next(node);
        REQUIRE(node->data == "B");
        node = list.find_next(node);
        REQUIRE(node->data == "D");
        node = list.find_next(node);
        REQUIRE(node->data == "C");
    }
    
    SECTION("Remove operations") {
        list.push_back("A");
        list.push_back("B");
        list.push_back("C");
        list.push_back("D");
        
        REQUIRE(list.remove_value("B"));
        REQUIRE(list.remove_before("D"));
        REQUIRE(list.remove_after("A"));
        
        REQUIRE(list.get_size() == 2);
        REQUIRE(list.find_first()->data == "A");
        
        // Находим последний элемент
        auto node = list.find_first();
        auto last = node;
        while (node) {
            last = node;
            node = list.find_next(node);
        }
        REQUIRE(last->data == "D");
    }
    
    SECTION("Pop operations") {
        list.push_back("A");
        list.push_back("B");
        list.push_back("C");
        
        REQUIRE(list.pop_front());
        REQUIRE(list.pop_back());
        
        REQUIRE(list.get_size() == 1);
        REQUIRE(list.find_first()->data == "B");
    }
    
    SECTION("Print both directions") {
        list.push_back("A");
        list.push_back("B");
        list.push_back("C");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        list.print_forward();
        std::cout.rdbuf(old_buf);
        REQUIRE(ss.str().find("A") != string::npos);
        
        ss.str("");
        old_buf = std::cout.rdbuf(ss.rdbuf());
        list.print_backward();
        std::cout.rdbuf(old_buf);
        REQUIRE(ss.str().find("C") != string::npos);
    }
}

// ========== STACK TESTS ==========
TEST_CASE("Stack - Basic operations", "[stack]") {
    Stack s;
    
    SECTION("Empty stack") {
        REQUIRE(s.is_empty());
        REQUIRE(s.get_size() == 0);
    }
    
    SECTION("Push and pop") {
        s.push("A");
        s.push("B");
        s.push("C");
        
        REQUIRE_FALSE(s.is_empty());
        REQUIRE(s.get_size() == 3);
        
        REQUIRE(s.peek() == "C");
        REQUIRE(s.pop() == "C");
        REQUIRE(s.pop() == "B");
        REQUIRE(s.pop() == "A");
        REQUIRE(s.is_empty());
    }
    
    SECTION("Underflow protection") {
        REQUIRE_THROWS_AS(s.pop(), std::runtime_error);
        REQUIRE_THROWS_AS(s.peek(), std::runtime_error);
    }
    
    SECTION("Print stack") {
        s.push("First");
        s.push("Second");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        s.print();
        std::cout.rdbuf(old_buf);
        
        REQUIRE(ss.str().find("First") != string::npos);
        REQUIRE(ss.str().find("Second") != string::npos);
    }
}

// ========== QUEUE TESTS ==========
TEST_CASE("Queue - Basic operations", "[queue]") {
    Queue q;
    
    SECTION("Empty queue") {
        REQUIRE(q.is_empty());
        REQUIRE(q.get_size() == 0);
    }
    
    SECTION("Enqueue and dequeue") {
        q.push("A");
        q.push("B");
        q.push("C");
        
        REQUIRE_FALSE(q.is_empty());
        REQUIRE(q.get_size() == 3);
        
        REQUIRE(q.peek() == "A");
        REQUIRE(q.pop() == "A");
        REQUIRE(q.pop() == "B");
        REQUIRE(q.pop() == "C");
        REQUIRE(q.is_empty());
    }
    
    SECTION("Underflow protection") {
        REQUIRE_THROWS_AS(q.pop(), std::runtime_error);
        REQUIRE_THROWS_AS(q.peek(), std::runtime_error);
    }
    
    SECTION("Print queue") {
        q.push("First");
        q.push("Second");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        q.print();
        std::cout.rdbuf(old_buf);
        
        REQUIRE(ss.str().find("First") != string::npos);
        REQUIRE(ss.str().find("Second") != string::npos);
    }
}

// ========== FULL BINARY TREE TESTS ==========
TEST_CASE("FullBinaryTree - Basic operations", "[tree]") {
    FullBinaryTree tree;
    
    SECTION("Empty tree") {
        REQUIRE(tree.get_size() == 0);
        REQUIRE(tree.height() == 0);
        // Заменили is_empty() на проверку размера
        REQUIRE(tree.get_size() == 0); // Это эквивалент is_empty()
    }
    
    SECTION("Insert and search") {
        REQUIRE(tree.insert(5, "Five"));
        REQUIRE(tree.insert(3, "Three"));
        REQUIRE(tree.insert(7, "Seven"));
        REQUIRE(tree.insert(1, "One"));
        REQUIRE(tree.insert(4, "Four"));
        
        REQUIRE(tree.get_size() == 5);
        REQUIRE(tree.search(5) == "Five");
        REQUIRE(tree.search(3) == "Three");
        REQUIRE(tree.search(99) == ""); // Not found
    }
    
    SECTION("Tree properties") {
        tree.insert(5, "Five");
        tree.insert(3, "Three");
        tree.insert(7, "Seven");
        tree.insert(1, "One");
        tree.insert(4, "Four");
        
        REQUIRE(tree.height() > 0);
        REQUIRE_FALSE(tree.is_full()); // Not a full binary tree in this case
        
        // Full tree test
        FullBinaryTree fullTree;
        fullTree.insert(2, "Two");
        fullTree.insert(1, "One");
        fullTree.insert(3, "Three");
        REQUIRE(fullTree.is_full());
    }
    
    SECTION("Traversals") {
        tree.insert(4, "Four");
        tree.insert(2, "Two");
        tree.insert(6, "Six");
        tree.insert(1, "One");
        tree.insert(3, "Three");
        tree.insert(5, "Five");
        tree.insert(7, "Seven");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        
        tree.inorder();
        REQUIRE(ss.str().find("One") != string::npos);
        ss.str("");
        
        tree.preorder();
        REQUIRE(ss.str().find("Four") != string::npos);
        ss.str("");
        
        tree.postorder();
        REQUIRE(ss.str().find("Seven") != string::npos);
        ss.str("");
        
        tree.level_order();
        REQUIRE(ss.str().find("Two") != string::npos);
        
        std::cout.rdbuf(old_buf);
    }
    
    SECTION("Serialization") {
        tree.insert(5, "Five");
        tree.insert(3, "Three");
        tree.insert(7, "Seven");
        
        REQUIRE(tree.serialize_text("test_tree.txt"));
        REQUIRE(tree.serialize_binary("test_tree.bin"));
        
        FullBinaryTree tree2;
        REQUIRE(tree2.deserialize_text("test_tree.txt"));
        REQUIRE(tree2.get_size() == 3);
        REQUIRE(tree2.search(5) == "Five");
        
        FullBinaryTree tree3;
        REQUIRE(tree3.deserialize_binary("test_tree.bin"));
        REQUIRE(tree3.get_size() == 3);
        REQUIRE(tree3.search(7) == "Seven");
        
        // Cleanup
        remove("test_tree.txt");
        remove("test_tree.bin");
    }
}

// ========== DOUBLE HASH TABLE TESTS ==========
TEST_CASE("DoubleHashTable - Basic operations", "[hashtable]") {
    DoubleHashTable ht(5);
    
    SECTION("Empty table") {
        REQUIRE(ht.get_size() == 0);
        REQUIRE(ht.get_capacity() == 5);
        REQUIRE(ht.get_load_factor() == 0.0);
    }
    
    SECTION("Insert and search") {
        REQUIRE(ht.insert("key1", "value1"));
        REQUIRE(ht.insert("key2", "value2"));
        REQUIRE(ht.insert("key3", "value3"));
        
        REQUIRE(ht.get_size() == 3);
        REQUIRE(ht.search("key1") == "value1");
        REQUIRE(ht.search("key2") == "value2");
        REQUIRE(ht.search("key3") == "value3");
        REQUIRE(ht.search("nonexistent") == "");
    }
    
    SECTION("Update existing key") {
        ht.insert("key1", "value1");
        REQUIRE(ht.insert("key1", "updated"));
        REQUIRE(ht.search("key1") == "updated");
    }
    
    SECTION("Remove operations") {
        ht.insert("key1", "value1");
        ht.insert("key2", "value2");
        
        REQUIRE(ht.remove("key1"));
        REQUIRE(ht.get_size() == 1);
        REQUIRE(ht.search("key1") == "");
        REQUIRE(ht.search("key2") == "value2");
        
        REQUIRE_FALSE(ht.remove("nonexistent"));
    }
    
    SECTION("Load factor and restructuring") {
        for (int i = 0; i < 4; i++) {
            ht.insert("key" + to_string(i), "value" + to_string(i));
        }
        
        REQUIRE(ht.get_load_factor() < 1.0);
        
        // Should trigger restructuring
        ht.insert("key4", "value4");
        REQUIRE(ht.get_capacity() > 5); // Capacity increased
    }
    
    SECTION("Print table") {
        ht.insert("name", "John");
        ht.insert("age", "30");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        ht.print();
        std::cout.rdbuf(old_buf);
        
        REQUIRE(ss.str().find("John") != string::npos);
        REQUIRE(ss.str().find("30") != string::npos);
    }
    
    SECTION("Serialization") {
        ht.insert("user1", "Alice");
        ht.insert("user2", "Bob");
        ht.insert("user3", "Charlie");
        
        REQUIRE(ht.serialize_binary("test_hash.bin"));
        REQUIRE(ht.serialize_text("test_hash.txt"));
        
        DoubleHashTable ht2;
        REQUIRE(ht2.deserialize_binary("test_hash.bin"));
        REQUIRE(ht2.get_size() == 3);
        REQUIRE(ht2.search("user2") == "Bob");
        
        DoubleHashTable ht3;
        REQUIRE(ht3.deserialize_text("test_hash.txt"));
        REQUIRE(ht3.get_size() == 3);
        REQUIRE(ht3.search("user3") == "Charlie");
        
        // Cleanup
        remove("test_hash.bin");
        remove("test_hash.txt");
    }
}

// ========== DATABASE TESTS ==========
TEST_CASE("Database - Basic operations", "[database]") {
    Database db;
    
    SECTION("Array commands") {
        REQUIRE(db.executeCommand("MCREATE myarray").find("SUCCESS") != string::npos);
        REQUIRE(db.hasArray("myarray"));
        
        REQUIRE(db.executeCommand("MPUSH myarray hello").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("MPUSH myarray world").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("MSIZE myarray").find("SIZE: 2") != string::npos);
        
        // Test duplicate creation
        REQUIRE(db.executeCommand("MCREATE myarray").find("ERROR") != string::npos);
    }
    
    SECTION("Single list commands") {
        REQUIRE(db.executeCommand("FCREATE mylist").find("SUCCESS") != string::npos);
        REQUIRE(db.hasSinglyList("mylist"));
        
        REQUIRE(db.executeCommand("FPUSH mylist FRONT A").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("FPUSH mylist BACK B").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("FSIZE mylist").find("SIZE: 2") != string::npos);
    }
    
    SECTION("Double list commands") {
        REQUIRE(db.executeCommand("LCREATE dlist").find("SUCCESS") != string::npos);
        REQUIRE(db.hasDoublyList("dlist"));
        
        REQUIRE(db.executeCommand("LPUSH dlist FRONT X").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("LPUSH dlist BACK Y").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("LSIZE dlist").find("SIZE: 2") != string::npos);
    }
    
    SECTION("Stack commands") {
        REQUIRE(db.executeCommand("SCREATE mystack").find("SUCCESS") != string::npos);
        REQUIRE(db.hasStack("mystack"));
        
        REQUIRE(db.executeCommand("SPUSH mystack item1").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("SPUSH mystack item2").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("SSIZE mystack").find("SIZE: 2") != string::npos);
    }
    
    SECTION("Queue commands") {
        REQUIRE(db.executeCommand("QCREATE myqueue").find("SUCCESS") != string::npos);
        REQUIRE(db.hasQueue("myqueue"));
        
        REQUIRE(db.executeCommand("QPUSH myqueue job1").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("QPUSH myqueue job2").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("QSIZE myqueue").find("SIZE: 2") != string::npos);
    }
    
    SECTION("Tree commands") {
        REQUIRE(db.executeCommand("TCREATE mytree").find("SUCCESS") != string::npos);
        REQUIRE(db.hasTree("mytree"));
        
        REQUIRE(db.executeCommand("TINSERT mytree 10 ten").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("TINSERT mytree 5 five").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("TINSERT mytree 15 fifteen").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("TSIZE mytree").find("SIZE: 3") != string::npos);
    }
    
    SECTION("Hash table commands") {
        REQUIRE(db.executeCommand("HCREATE myhash").find("SUCCESS") != string::npos);
        REQUIRE(db.hasHashTable("myhash"));
        
        REQUIRE(db.executeCommand("HINSERT myhash username admin").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("HINSERT myhash password 1234").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("HSEARCH myhash username").find("admin") != string::npos);
        REQUIRE(db.executeCommand("HSIZE myhash").find("SIZE: 2") != string::npos);
        
        REQUIRE(db.executeCommand("HDELETE myhash password").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("HSIZE myhash").find("SIZE: 1") != string::npos);
    }
    
    SECTION("Print commands") {
        db.executeCommand("MCREATE testarray");
        db.executeCommand("MPUSH testarray hello");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        db.executeCommand("PRINT testarray");
        std::cout.rdbuf(old_buf);
        
        REQUIRE(ss.str().find("hello") != string::npos);
    }
    
    SECTION("List command") {
        db.executeCommand("MCREATE arr1");
        db.executeCommand("HCREATE hash1");
        db.executeCommand("SCREATE stack1");
        
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        db.executeCommand("LIST");
        std::cout.rdbuf(old_buf);
        
        string output = ss.str();
        REQUIRE(output.find("arr1") != string::npos);
        REQUIRE(output.find("hash1") != string::npos);
        REQUIRE(output.find("stack1") != string::npos);
    }
    
    SECTION("Help command") {
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        db.executeCommand("HELP");
        std::cout.rdbuf(old_buf);
        
        REQUIRE(ss.str().find("COMMANDS") != string::npos);
    }
    
    SECTION("Clear command") {
        db.executeCommand("MCREATE test1");
        db.executeCommand("HCREATE test2");
        
        REQUIRE(db.hasArray("test1"));
        REQUIRE(db.hasHashTable("test2"));
        
        db.executeCommand("CLEAR");
        
        REQUIRE_FALSE(db.hasArray("test1"));
        REQUIRE_FALSE(db.hasHashTable("test2"));
    }
    
    SECTION("Invalid commands") {
        REQUIRE(db.executeCommand("").find("ERROR") != string::npos);
        REQUIRE(db.executeCommand("UNKNOWN").find("ERROR") != string::npos);
        REQUIRE(db.executeCommand("PRINT nonexistent").find("ERROR") != string::npos);
        REQUIRE(db.executeCommand("MPUSH nonexistent value").find("ERROR") != string::npos);
    }
    
    SECTION("File operations") {
        // Create some data
        db.executeCommand("MCREATE save_array");
        db.executeCommand("MPUSH save_array data1");
        db.executeCommand("MPUSH save_array data2");
        
        db.executeCommand("HCREATE save_hash");
        db.executeCommand("HINSERT save_hash key1 val1");
        db.executeCommand("HINSERT save_hash key2 val2");
        
        // Save to file
        REQUIRE(db.executeCommand("SAVE test_db.dat").find("SUCCESS") != string::npos);
        
        // Clear and load
        db.executeCommand("CLEAR");
        REQUIRE(db.executeCommand("LOAD test_db.dat").find("SUCCESS") != string::npos);
        
        // Verify loaded data
        REQUIRE(db.hasArray("save_array"));
        REQUIRE(db.hasHashTable("save_hash"));
        
        // Cleanup
        remove("test_db.dat");
    }
}

// ========== INTEGRATION TESTS ==========
TEST_CASE("Integration - All data structures work together", "[integration]") {
    Database db;
    
    SECTION("Multiple container types") {
        // Create containers of all types
        REQUIRE(db.executeCommand("MCREATE arr").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("FCREATE slist").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("LCREATE dlist").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("SCREATE stack").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("QCREATE queue").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("TCREATE tree").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("HCREATE hash").find("SUCCESS") != string::npos);
        
        // Add data to each
        REQUIRE(db.executeCommand("MPUSH arr element").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("FPUSH slist FRONT node").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("LPUSH dlist FRONT node").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("SPUSH stack item").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("QPUSH queue job").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("TINSERT tree 42 answer").find("SUCCESS") != string::npos);
        REQUIRE(db.executeCommand("HINSERT hash key value").find("SUCCESS") != string::npos);
        
        // Check all exist
        REQUIRE(db.hasArray("arr"));
        REQUIRE(db.hasSinglyList("slist"));
        REQUIRE(db.hasDoublyList("dlist"));
        REQUIRE(db.hasStack("stack"));
        REQUIRE(db.hasQueue("queue"));
        REQUIRE(db.hasTree("tree"));
        REQUIRE(db.hasHashTable("hash"));
        
        // Test PRINT command for each
        std::stringstream ss;
        auto old_buf = std::cout.rdbuf(ss.rdbuf());
        
        db.executeCommand("PRINT arr");
        REQUIRE(ss.str().find("element") != string::npos);
        ss.str("");
        
        db.executeCommand("PRINT hash");
        REQUIRE(ss.str().find("value") != string::npos);
        
        std::cout.rdbuf(old_buf);
    }
    
    SECTION("Complex scenario") {
        // Create user database scenario
        db.executeCommand("MCREATE users");
        db.executeCommand("MPUSH users Alice");
        db.executeCommand("MPUSH users Bob");
        db.executeCommand("MPUSH users Charlie");
        
        db.executeCommand("HCREATE user_details");
        db.executeCommand("HINSERT user_details Alice age:25");
        db.executeCommand("HINSERT user_details Bob age:30");
        db.executeCommand("HINSERT user_details Charlie age:35");
        
        db.executeCommand("QCREATE tasks");
        db.executeCommand("QPUSH tasks task1");
        db.executeCommand("QPUSH tasks task2");
        db.executeCommand("QPUSH tasks task3");
        
        // Simulate processing
        db.executeCommand("QCREATE processed");
        db.executeCommand("QPOP tasks"); // Process task1
        db.executeCommand("QPUSH processed task1_done");
        
        // Verify state
        REQUIRE(db.hasArray("users"));
        REQUIRE(db.hasHashTable("user_details"));
        REQUIRE(db.hasQueue("tasks"));
        REQUIRE(db.hasQueue("processed"));
    }
}

TEST_CASE("Performance - Stress tests", "[performance]") {
    SECTION("Array stress test") {
        Array arr;
        for (int i = 0; i < 1000; i++) {
            arr.push_back("item" + to_string(i));
        }
        REQUIRE(arr.length() == 1000);
        REQUIRE(arr.get(999) == "item999");
    }
    
    SECTION("Hash table stress test") {
        DoubleHashTable ht(100);
        for (int i = 0; i < 500; i++) {
            ht.insert("key" + to_string(i), "value" + to_string(i));
        }
        REQUIRE(ht.get_size() == 500);
        REQUIRE(ht.search("key250") == "value250");
    }
    
    SECTION("Database stress test") {
        Database db;
        db.executeCommand("HCREATE large_hash");
        for (int i = 0; i < 100; i++) {
            db.executeCommand("HINSERT large_hash key" + to_string(i) + " value" + to_string(i));
        }
        REQUIRE(db.hasHashTable("large_hash"));
    }
}