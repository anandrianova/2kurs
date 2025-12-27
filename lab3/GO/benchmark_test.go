package main

import (
	"fmt"
	"testing"
)

func BenchmarkArrayInsert(b *testing.B) {
	arr := NewArray()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		arr.Insert("value")
	}
}

func BenchmarkArrayGet(b *testing.B) {
	arr := NewArray()
	for i := 0; i < 1000; i++ {
		arr.Insert("value")
	}
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		_, _ = arr.Get(i % 1000) // Игнорируем ошибку в бенчмарке
	}
}

func BenchmarkSinglyLinkedListPushBack(b *testing.B) {
	list := NewSinglyLinkedList()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		list.PushBack("value")
	}
}

func BenchmarkDoublyLinkedListPushBack(b *testing.B) {
	list := NewDoublyLinkedList()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		list.PushBack("value")
	}
}

func BenchmarkQueueEnqueueDequeue(b *testing.B) {
	queue := NewQueue()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		queue.Enqueue("value")
		if i%2 == 0 {
			_, _ = queue.Dequeue() // Игнорируем ошибку в бенчмарке
		}
	}
}

func BenchmarkStackPushPop(b *testing.B) {
	stack := NewStack()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		stack.Push("value")
		if i%2 == 0 {
			_, _ = stack.Pop() // Игнорируем ошибку в бенчмарке
		}
	}
}

func BenchmarkHashTableInsert(b *testing.B) {
	ht := NewHashTable()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		ht.Insert(i, "value")
	}
}

func BenchmarkHashTableGet(b *testing.B) {
	ht := NewHashTable()
	for i := 0; i < 1000; i++ {
		ht.Insert(i, "value")
	}
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		_, _ = ht.Get(i % 1000) // Игнорируем ошибку в бенчмарке
	}
}

func BenchmarkBinaryTreeInsert(b *testing.B) {
	tree := NewBinaryTree()
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		tree.Insert("value")
	}
}

func BenchmarkBinaryTreeSearch(b *testing.B) {
	tree := NewBinaryTree()
	for i := 0; i < 1000; i++ {
		tree.Insert(string(rune('a' + i%26)))
	}
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		tree.Search("z")
	}
}

// Бенчмарки сериализации

func BenchmarkArrayJSONSerialize(b *testing.B) {
	arr := NewArray()
	for i := 0; i < 1000; i++ {
		arr.Insert(fmt.Sprintf("value%d", i))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := arr.Serialize()
		if err != nil {
			b.Fatalf("Serialize failed: %v", err)
		}
	}
}

func BenchmarkArrayBinarySerialize(b *testing.B) {
	arr := NewArray()
	for i := 0; i < 1000; i++ {
		arr.Insert(fmt.Sprintf("value%d", i))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := arr.SerializeBinary()
		if err != nil {
			b.Fatalf("SerializeBinary failed: %v", err)
		}
	}
}

func BenchmarkArrayJSONDeserialize(b *testing.B) {
	arr := NewArray()
	for i := 0; i < 1000; i++ {
		arr.Insert(fmt.Sprintf("value%d", i))
	}
	data, err := arr.Serialize()
	if err != nil {
		b.Fatalf("Setup failed: %v", err)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		newArr := NewArray()
		err := newArr.Deserialize(data)
		if err != nil {
			b.Fatalf("Deserialize failed: %v", err)
		}
	}
}

func BenchmarkArrayBinaryDeserialize(b *testing.B) {
	arr := NewArray()
	for i := 0; i < 1000; i++ {
		arr.Insert(fmt.Sprintf("value%d", i))
	}
	data, err := arr.SerializeBinary()
	if err != nil {
		b.Fatalf("Setup failed: %v", err)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		newArr := NewArray()
		err := newArr.DeserializeBinary(data)
		if err != nil {
			b.Fatalf("DeserializeBinary failed: %v", err)
		}
	}
}

func BenchmarkHashTableJSONSerialize(b *testing.B) {
	ht := NewHashTable()
	for i := 0; i < 1000; i++ {
		ht.Insert(i, fmt.Sprintf("value%d", i))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := ht.Serialize()
		if err != nil {
			b.Fatalf("Serialize failed: %v", err)
		}
	}
}

func BenchmarkHashTableBinarySerialize(b *testing.B) {
	ht := NewHashTable()
	for i := 0; i < 1000; i++ {
		ht.Insert(i, fmt.Sprintf("value%d", i))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := ht.SerializeBinary()
		if err != nil {
			b.Fatalf("SerializeBinary failed: %v", err)
		}
	}
}

func BenchmarkBinaryTreeJSONSerialize(b *testing.B) {
	tree := NewBinaryTree()
	for i := 0; i < 1000; i++ {
		tree.Insert(fmt.Sprintf("value%d", i))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := tree.Serialize()
		if err != nil {
			b.Fatalf("Serialize failed: %v", err)
		}
	}
}

func BenchmarkBinaryTreeBinarySerialize(b *testing.B) {
	tree := NewBinaryTree()
	for i := 0; i < 1000; i++ {
		tree.Insert(fmt.Sprintf("value%d", i))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := tree.SerializeBinary()
		if err != nil {
			b.Fatalf("SerializeBinary failed: %v", err)
		}
	}
}
