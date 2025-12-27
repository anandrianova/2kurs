package main

import (
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
	"github.com/stretchr/testify/suite"
)

// TestSuite для Array с использованием testify suite
type ArrayTestSuite struct {
	suite.Suite
	array *Array
}

// Настройка перед каждым тестом
func (suite *ArrayTestSuite) SetupTest() {
	suite.array = NewArray()
}

// TestArrayBasicOperations с использованием assert
func (suite *ArrayTestSuite) TestArrayBasicOperations() {
	// Test empty array
	assert.True(suite.T(), suite.array.IsEmpty())
	assert.Equal(suite.T(), 0, suite.array.Size())

	// Test insert
	suite.array.Insert("one")
	suite.array.Insert("two")
	suite.array.Insert("three")

	assert.False(suite.T(), suite.array.IsEmpty())
	assert.Equal(suite.T(), 3, suite.array.Size())
}

// TestArrayGetSetOperations с использованием require для фатальных ошибок
func (suite *ArrayTestSuite) TestArrayGetSetOperations() {
	suite.array.Insert("one")
	suite.array.Insert("two")
	suite.array.Insert("three")

	// Test get
	val, err := suite.array.Get(0)
	require.NoError(suite.T(), err)
	assert.Equal(suite.T(), "one", val)

	// Test set
	err = suite.array.Set(1, "modified")
	require.NoError(suite.T(), err)
	val, err = suite.array.Get(1)
	require.NoError(suite.T(), err)
	assert.Equal(suite.T(), "modified", val)
}

// TestArrayErrorConditions с проверкой ошибок
func (suite *ArrayTestSuite) TestArrayErrorConditions() {
	suite.array.Insert("one")

	// Test get out of bounds
	_, err := suite.array.Get(1)
	assert.Error(suite.T(), err)
	assert.Equal(suite.T(), ErrIndexOutOfRange, err)

	// Test set out of bounds
	err = suite.array.Set(1, "test")
	assert.Error(suite.T(), err)
	assert.Equal(suite.T(), ErrIndexOutOfRange, err)
}

// Запуск suite
func TestArrayTestSuite(t *testing.T) {
	suite.Run(t, new(ArrayTestSuite))
}

// TestHashTableWithTestify - отдельные тесты с testify
func TestHashTableWithTestify(t *testing.T) {
	t.Run("Basic Operations", func(t *testing.T) {
		ht := NewHashTable()

		assert.True(t, ht.IsEmpty())
		assert.Equal(t, 0, ht.Size())

		ht.Insert(1, "Alice")
		ht.Insert(2, "Bob")

		assert.False(t, ht.IsEmpty())
		assert.Equal(t, 2, ht.Size())

		val, exists := ht.Get(1)
		assert.True(t, exists)
		assert.Equal(t, "Alice", val)
	})

	t.Run("Update Existing Key", func(t *testing.T) {
		ht := NewHashTable()
		ht.Insert(1, "Alice")
		ht.Insert(1, "Alicia") // Update

		val, exists := ht.Get(1)
		assert.True(t, exists)
		assert.Equal(t, "Alicia", val)
		assert.Equal(t, 1, ht.Size())
	})

	t.Run("Non-existent Key", func(t *testing.T) {
		ht := NewHashTable()
		ht.Insert(1, "Alice")

		val, exists := ht.Get(999)
		assert.False(t, exists)
		assert.Empty(t, val)
	})
}

// TestBinaryTreeWithTestify с использованием табличных тестов
func TestBinaryTreeWithTestify(t *testing.T) {
	testCases := []struct {
		name     string
		values   []string
		search   string
		expected bool
	}{
		{"Empty Tree", []string{}, "test", false},
		{"Single Element Found", []string{"root"}, "root", true},
		{"Single Element Not Found", []string{"root"}, "missing", false},
		{"Multiple Elements Found", []string{"dog", "cat", "elephant"}, "cat", true},
		{"Multiple Elements Not Found", []string{"dog", "cat", "elephant"}, "zebra", false},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			tree := NewBinaryTree()
			for _, value := range tc.values {
				tree.Insert(value)
			}

			found := tree.Search(tc.search)
			assert.Equal(t, tc.expected, found, "Search result mismatch for case: %s", tc.name)
		})
	}
}

// TestLinkedListWithTestify с проверкой значений
func TestLinkedListWithTestify(t *testing.T) {
	t.Run("Singly Linked List", func(t *testing.T) {
		list := NewSinglyLinkedList()

		list.PushFront("front")
		list.PushBack("back")

		assert.Equal(t, 2, list.Size())
		assert.False(t, list.IsEmpty())

		front, err := list.PopFront()
		assert.NoError(t, err)
		assert.Equal(t, "front", front)
		assert.Equal(t, 1, list.Size())
	})

	t.Run("Doubly Linked List", func(t *testing.T) {
		list := NewDoublyLinkedList()

		list.PushFront("front")
		list.PushBack("back")
		// Обрабатываем ошибку при Insert
		err := list.Insert(1, "middle")
		assert.NoError(t, err)

		assert.Equal(t, 3, list.Size())

		val, err := list.Get(1)
		assert.NoError(t, err)
		assert.Equal(t, "middle", val)
	})
}

// TestContainerInterfacesWithTestify проверка реализации интерфейсов
func TestContainerInterfacesWithTestify(t *testing.T) {
	t.Run("Array implements Container", func(t *testing.T) {
		var _ Container = NewArray()
	})

	t.Run("SinglyLinkedList implements Container", func(t *testing.T) {
		var _ Container = NewSinglyLinkedList()
	})

	t.Run("HashTable implements Container", func(t *testing.T) {
		var _ Container = NewHashTable()
	})

	t.Run("BinaryTree implements Container", func(t *testing.T) {
		var _ Container = NewBinaryTree()
	})
}

// TestArrayWithTestifyAssertions дополнительные тесты с assert
func TestArrayWithTestifyAssertions(t *testing.T) {
	arr := NewArray()

	// Test initial state
	assert.True(t, arr.IsEmpty(), "New array should be empty")
	assert.Equal(t, 0, arr.Size(), "New array size should be 0")

	// Test insertion
	arr.Insert("test")
	assert.False(t, arr.IsEmpty(), "Array should not be empty after insert")
	assert.Equal(t, 1, arr.Size(), "Array size should be 1 after insert")

	// Test value retrieval
	val, err := arr.Get(0)
	require.NoError(t, err, "Should not get error for valid index")
	assert.Equal(t, "test", val, "Retrieved value should match inserted value")

	// Test error case
	_, err = arr.Get(1)
	assert.Error(t, err, "Should get error for invalid index")
	assert.Equal(t, ErrIndexOutOfRange, err, "Error should be ErrIndexOutOfRange")
}

// TestHashTableStatisticsWithTestify тесты статистики хеш-таблицы
func TestHashTableStatisticsWithTestify(t *testing.T) {
	ht := NewHashTableWithCapacity(4)

	// Insert elements to create chains
	ht.Insert(1, "a")
	ht.Insert(5, "b") // Collision with 1
	ht.Insert(9, "c") // Collision with 1
	ht.Insert(2, "d")

	assert.Equal(t, 4, ht.Size(), "Hash table should have 4 elements")
	assert.Greater(t, ht.GetLongestChain(), 0, "Longest chain should be positive")
	assert.Greater(t, ht.GetShortestChain(), 0, "Shortest chain should be positive")
	assert.Greater(t, ht.GetLoadFactor(), 0.0, "Load factor should be positive")
}

// TestBinaryTreeTraversalsWithTestify тесты обходов дерева
func TestBinaryTreeTraversalsWithTestify(t *testing.T) {
	tree := NewBinaryTree()
	values := []string{"m", "f", "s", "a", "h", "r", "z"}

	for _, v := range values {
		tree.Insert(v)
	}

	assert.Equal(t, len(values), tree.Size(), "Tree size should match number of inserted values")

	// Test that all traversals return correct number of elements
	assert.Len(t, tree.InOrder(), len(values), "In-order traversal should return all elements")
	assert.Len(t, tree.PreOrder(), len(values), "Pre-order traversal should return all elements")
	assert.Len(t, tree.PostOrder(), len(values), "Post-order traversal should return all elements")
	assert.Len(t, tree.LevelOrder(), len(values), "Level-order traversal should return all elements")

	// Test height
	assert.Greater(t, tree.Height(), 0, "Tree height should be positive")
}
