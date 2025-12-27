package main

type Queue struct {
	list *SinglyLinkedList
}

func NewQueue() *Queue {
	return &Queue{
		list: NewSinglyLinkedList(),
	}
}

func (q *Queue) Enqueue(value string) {
	q.list.PushBack(value)
}

func (q *Queue) Dequeue() (string, error) {
	return q.list.PopFront()
}

func (q *Queue) Front() (string, error) {
	return q.list.Get(0)
}

func (q *Queue) Back() (string, error) {
	return q.list.Get(q.list.Size() - 1)
}

func (q *Queue) Size() int {
	return q.list.Size()
}

func (q *Queue) IsEmpty() bool {
	return q.list.IsEmpty()
}

func (q *Queue) Clear() {
	q.list.Clear()
}

func (q *Queue) Print() {
	q.list.Print()
}

func (q *Queue) Serialize() ([]byte, error) {
	return q.list.Serialize()
}

func (q *Queue) Deserialize(data []byte) error {
	return q.list.Deserialize(data)
}

// SerializeBinary сериализует очередь в бинарный формат
func (q *Queue) SerializeBinary() ([]byte, error) {
	return q.list.SerializeBinary()
}

// DeserializeBinary десериализует очередь из бинарного формата
func (q *Queue) DeserializeBinary(data []byte) error {
	return q.list.DeserializeBinary(data)
}
