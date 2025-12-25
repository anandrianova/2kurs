#include <benchmark/benchmark.h>
#include <random>
#include "Array.h"
#include "SingleList.h"
#include "DoubleList.h"
#include "Stack.h"
#include "Queue.h"
#include "FullBinaryTree.h"
#include "HashTable.h"

using namespace std;

// Бенчмарк для Array
static void BM_ArrayPushBack(benchmark::State& state) {
    Array arr;
    for (auto _ : state) {
        arr.push_back("element_" + to_string(state.iterations()));
    }
    state.SetComplexityN(state.iterations());
}
BENCHMARK(BM_ArrayPushBack)->Range(8, 8<<10)->Complexity();

static void BM_ArrayInsert(benchmark::State& state) {
    Array arr;
    for (int i = 0; i < state.range(0); i++) {
        arr.push_back("element_" + to_string(i));
    }
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, arr.length() - 1);
    
    for (auto _ : state) {
        arr.insert(dist(gen), "inserted_element");
    }
}
BENCHMARK(BM_ArrayInsert)->Range(8, 1024);

// Бенчмарк для SingleList
static void BM_SingleListPushBack(benchmark::State& state) {
    SingleList list;
    for (auto _ : state) {
        list.push_back("element_" + to_string(state.iterations()));
    }
}
BENCHMARK(BM_SingleListPushBack);

static void BM_SingleListPushFront(benchmark::State& state) {
    SingleList list;
    for (auto _ : state) {
        list.push_front("element_" + to_string(state.iterations()));
    }
}
BENCHMARK(BM_SingleListPushFront);

// Бенчмарк для DoubleList
static void BM_DoubleListPushBack(benchmark::State& state) {
    DoubleList list;
    for (auto _ : state) {
        list.push_back("element_" + to_string(state.iterations()));
    }
}
BENCHMARK(BM_DoubleListPushBack);

// Бенчмарк для Stack
static void BM_StackPushPop(benchmark::State& state) {
    Stack stack;
    for (auto _ : state) {
        for (int i = 0; i < 100; i++) {
            stack.push("element_" + to_string(i));
        }
        for (int i = 0; i < 100; i++) {
            stack.pop();
        }
    }
}
BENCHMARK(BM_StackPushPop);

// Бенчмарк для Queue
static void BM_QueuePushPop(benchmark::State& state) {
    Queue queue;
    for (auto _ : state) {
        for (int i = 0; i < 100; i++) {
            queue.push("element_" + to_string(i));
        }
        for (int i = 0; i < 100; i++) {
            queue.pop();
        }
    }
}
BENCHMARK(BM_QueuePushPop);

// Бенчмарк для FullBinaryTree
static void BM_TreeInsert(benchmark::State& state) {
    FullBinaryTree tree;
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); i++) {
            tree.insert(i, "value_" + to_string(i));
        }
        state.PauseTiming();
        // Очистка дерева
        FullBinaryTree new_tree;
        tree = new_tree;
        state.ResumeTiming();
    }
}
BENCHMARK(BM_TreeInsert)->Range(8, 512);

// Бенчмарк для DoubleHashTable
static void BM_DoubleHashTableInsert(benchmark::State& state) {
    DoubleHashTable table(1000);
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); i++) {
            table.insert("key_" + to_string(i), "value_" + to_string(i));
        }
    }
}
BENCHMARK(BM_DoubleHashTableInsert)->Range(8, 512);

// Бенчмарк сериализации/десериализации
static void BM_ArraySerialization(benchmark::State& state) {
    Array arr;
    for (int i = 0; i < 1000; i++) {
        arr.push_back("element_" + to_string(i));
    }
    
    for (auto _ : state) {
        arr.serialize_binary("benchmark_array.bin");
        Array arr2;
        arr2.deserialize_binary("benchmark_array.bin");
    }
}
BENCHMARK(BM_ArraySerialization);

BENCHMARK_MAIN();