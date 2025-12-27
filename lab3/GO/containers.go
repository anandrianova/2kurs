package main

import (
	"bytes"
	"encoding/gob"
	"errors"
	"fmt"
)

// Container базовый интерфейс для всех контейнеров
type Container interface {
	Size() int
	IsEmpty() bool
	Clear()
	Print()
}

// Serializer интерфейс для текстовой сериализации (JSON)
type Serializer interface {
	Serialize() ([]byte, error)
	Deserialize(data []byte) error
}

// BinarySerializer интерфейс для бинарной сериализации
type BinarySerializer interface {
	SerializeBinary() ([]byte, error)
	DeserializeBinary(data []byte) error
}

// UniversalContainer комбинированный интерфейс
type UniversalContainer interface {
	Container
	Serializer
	BinarySerializer
}

// Common errors
var (
	ErrIndexOutOfRange = errors.New("index out of range")
	ErrEmptyContainer  = errors.New("container is empty")
	ErrKeyNotFound     = errors.New("key not found")
)

// Вспомогательная функция для проверки индекса
func checkIndex(index, size int) error {
	if index < 0 || index >= size {
		return ErrIndexOutOfRange
	}
	return nil
}

// PrintContainer универсальная функция для печати контейнеров
func PrintContainer(name string, container Container) {
	fmt.Printf("%s: ", name)
	container.Print()
}

// Вспомогательные функции для бинарной сериализации
func serializeToBinary(data interface{}) ([]byte, error) {
	var buf bytes.Buffer
	encoder := gob.NewEncoder(&buf)
	if err := encoder.Encode(data); err != nil {
		return nil, err
	}
	return buf.Bytes(), nil
}

func deserializeFromBinary(data []byte, target interface{}) error {
	buf := bytes.NewBuffer(data)
	decoder := gob.NewDecoder(buf)
	return decoder.Decode(target)
}
