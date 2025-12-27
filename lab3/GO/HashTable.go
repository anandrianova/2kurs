package main

import (
	"encoding/json"
	"fmt"
	"strconv"
)

type hashNode struct {
	key   int
	value string
	next  *hashNode
}

// HashTable представляет хеш-таблицу с цепочками для разрешения коллизий
type HashTable struct {
	buckets []*hashNode
	size    int
}

const (
	defaultCapacity = 16
	loadFactor      = 0.75
)

// NewHashTable создает новую хеш-таблицу с емкостью по умолчанию
func NewHashTable() *HashTable {
	return &HashTable{
		buckets: make([]*hashNode, defaultCapacity),
		size:    0,
	}
}

// NewHashTableWithCapacity создает хеш-таблицу с заданной емкостью
func NewHashTableWithCapacity(capacity int) *HashTable {
	if capacity <= 0 {
		capacity = defaultCapacity
	}
	return &HashTable{
		buckets: make([]*hashNode, capacity),
		size:    0,
	}
}

func (h *HashTable) hash(key int) int {
	if len(h.buckets) == 0 {
		return 0
	}
	// Обработка отрицательных ключей
	hash := key % len(h.buckets)
	if hash < 0 {
		hash += len(h.buckets)
	}
	return hash
}

func (h *HashTable) rehash() {
	if len(h.buckets) == 0 {
		h.buckets = make([]*hashNode, defaultCapacity)
		return
	}

	oldBuckets := h.buckets
	newCapacity := len(oldBuckets) * 2
	if newCapacity < defaultCapacity {
		newCapacity = defaultCapacity
	}

	h.buckets = make([]*hashNode, newCapacity)
	h.size = 0

	for _, head := range oldBuckets {
		current := head
		for current != nil {
			h.insertInternal(current.key, current.value)
			current = current.next
		}
	}
}

func (h *HashTable) insertInternal(key int, value string) {
	index := h.hash(key)
	current := h.buckets[index]

	// Проверка существующего ключа
	for current != nil {
		if current.key == key {
			current.value = value
			return
		}
		current = current.next
	}

	// Вставка нового узла в начало цепочки
	newNode := &hashNode{key: key, value: value, next: h.buckets[index]}
	h.buckets[index] = newNode
	h.size++
}

// Insert вставляет или обновляет ключ-значение
func (h *HashTable) Insert(key int, value string) {
	if float64(h.size)/float64(len(h.buckets)) > loadFactor {
		h.rehash()
	}
	h.insertInternal(key, value)
}

// Get возвращает значение по ключу
func (h *HashTable) Get(key int) (string, bool) {
	if h.size == 0 || len(h.buckets) == 0 {
		return "", false
	}

	index := h.hash(key)
	current := h.buckets[index]

	for current != nil {
		if current.key == key {
			return current.value, true
		}
		current = current.next
	}
	return "", false
}

// Remove удаляет ключ-значение
func (h *HashTable) Remove(key int) bool {
	if h.size == 0 || len(h.buckets) == 0 {
		return false
	}

	index := h.hash(key)
	current := h.buckets[index]
	var prev *hashNode

	for current != nil {
		if current.key == key {
			if prev == nil {
				h.buckets[index] = current.next
			} else {
				prev.next = current.next
			}
			h.size--
			return true
		}
		prev = current
		current = current.next
	}
	return false
}

// Contains проверяет наличие ключа
func (h *HashTable) Contains(key int) bool {
	_, exists := h.Get(key)
	return exists
}

// Keys возвращает все ключи
func (h *HashTable) Keys() []int {
	keys := make([]int, 0, h.size)
	for _, head := range h.buckets {
		current := head
		for current != nil {
			keys = append(keys, current.key)
			current = current.next
		}
	}
	return keys
}

// Values возвращает все значения
func (h *HashTable) Values() []string {
	values := make([]string, 0, h.size)
	for _, head := range h.buckets {
		current := head
		for current != nil {
			values = append(values, current.value)
			current = current.next
		}
	}
	return values
}

// Size возвращает количество элементов
func (h *HashTable) Size() int {
	return h.size
}

// IsEmpty проверяет пуста ли таблица
func (h *HashTable) IsEmpty() bool {
	return h.size == 0
}

// Clear очищает таблицу
func (h *HashTable) Clear() {
	h.buckets = make([]*hashNode, defaultCapacity)
	h.size = 0
}

// Capacity возвращает емкость таблицы
func (h *HashTable) Capacity() int {
	return len(h.buckets)
}

// Print выводит таблицу в stdout
func (h *HashTable) Print() {
	fmt.Printf("HashTable (size: %d, capacity: %d):\n", h.size, len(h.buckets))
	for i, head := range h.buckets {
		if head != nil {
			fmt.Printf("Bucket %d: ", i)
			current := head
			for current != nil {
				fmt.Printf("(%d:%s) ", current.key, current.value)
				current = current.next
			}
			fmt.Println()
		}
	}
}

// GetLongestChain возвращает длину самой длинной цепочки
func (h *HashTable) GetLongestChain() int {
	maxLength := 0
	for _, head := range h.buckets {
		length := 0
		current := head
		for current != nil {
			length++
			current = current.next
		}
		if length > maxLength {
			maxLength = length
		}
	}
	return maxLength
}

// GetShortestChain возвращает длину самой короткой непустой цепочки
func (h *HashTable) GetShortestChain() int {
	if len(h.buckets) == 0 || h.size == 0 {
		return 0
	}

	minLength := -1
	for _, head := range h.buckets {
		length := 0
		current := head
		for current != nil {
			length++
			current = current.next
		}
		if length > 0 {
			if minLength == -1 || length < minLength {
				minLength = length
			}
		}
	}

	if minLength == -1 {
		return 0
	}
	return minLength
}

// GetLoadFactor возвращает коэффициент загрузки
func (h *HashTable) GetLoadFactor() float64 {
	if len(h.buckets) == 0 {
		return 0
	}
	return float64(h.size) / float64(len(h.buckets))
}

// Serialize сериализует таблицу в JSON
func (h *HashTable) Serialize() ([]byte, error) {
	data := make(map[string]string)
	for _, head := range h.buckets {
		current := head
		for current != nil {
			keyStr := strconv.Itoa(current.key)
			data[keyStr] = current.value
			current = current.next
		}
	}
	return json.Marshal(data)
}

// Deserialize десериализует таблицу из JSON
func (h *HashTable) Deserialize(data []byte) error {
	var jsonData map[string]string
	if err := json.Unmarshal(data, &jsonData); err != nil {
		return err
	}

	h.Clear()
	for keyStr, value := range jsonData {
		key, err := strconv.Atoi(keyStr)
		if err != nil {
			return err
		}
		h.Insert(key, value)
	}
	return nil
}

// SerializeBinary сериализует таблицу в бинарный формат
func (h *HashTable) SerializeBinary() ([]byte, error) {
	type KeyValue struct {
		Key   int
		Value string
	}

	type HashTableData struct {
		Size      int
		KeyValues []KeyValue
	}

	var keyValues []KeyValue
	for _, head := range h.buckets {
		current := head
		for current != nil {
			keyValues = append(keyValues, KeyValue{
				Key:   current.key,
				Value: current.value,
			})
			current = current.next
		}
	}

	hashTableData := HashTableData{
		Size:      h.size,
		KeyValues: keyValues,
	}

	return serializeToBinary(hashTableData)
}

// DeserializeBinary десериализует таблицу из бинарного формата
func (h *HashTable) DeserializeBinary(data []byte) error {
	type KeyValue struct {
		Key   int
		Value string
	}

	type HashTableData struct {
		Size      int
		KeyValues []KeyValue
	}

	var hashTableData HashTableData
	if err := deserializeFromBinary(data, &hashTableData); err != nil {
		return err
	}

	h.Clear()
	for _, kv := range hashTableData.KeyValues {
		h.Insert(kv.Key, kv.Value)
	}
	return nil
}
