package main

type Stack struct {
	list *SinglyLinkedList
}

func NewStack() *Stack {
	return &Stack{
		list: NewSinglyLinkedList(),
	}
}

func (s *Stack) Push(value string) {
	s.list.PushBack(value)
}

func (s *Stack) Pop() (string, error) {
	return s.list.PopBack()
}

func (s *Stack) Top() (string, error) {
	return s.list.Get(s.list.Size() - 1)
}

func (s *Stack) Size() int {
	return s.list.Size()
}

func (s *Stack) IsEmpty() bool {
	return s.list.IsEmpty()
}

func (s *Stack) Clear() {
	s.list.Clear()
}

func (s *Stack) Print() {
	s.list.Print()
}

func (s *Stack) Serialize() ([]byte, error) {
	return s.list.Serialize()
}

func (s *Stack) Deserialize(data []byte) error {
	return s.list.Deserialize(data)
}

// SerializeBinary сериализует стек в бинарный формат
func (s *Stack) SerializeBinary() ([]byte, error) {
	return s.list.SerializeBinary()
}

// DeserializeBinary десериализует стек из бинарного формата
func (s *Stack) DeserializeBinary(data []byte) error {
	return s.list.DeserializeBinary(data)
}
