package util

import "context"

type Broker[T any] struct {
	publishCh chan T
	unsubCh   chan chan T
	subCh     chan chan T
}

func NewBroker[T any]() Broker[T] {
	return Broker[T]{
		publishCh: make(chan T, 1),
		subCh:     make(chan chan T, 1),
		unsubCh:   make(chan chan T, 1),
	}
}

func (b Broker[T]) Start(ctx context.Context) {
	// TODO..
}

func (b Broker[T]) Publish(msg T) {
	b.publishCh <- msg
}

func (b Broker[T]) Subscribe(ch chan T) {
	b.subCh <- ch
}

func (b Broker[T]) Unsubscribe(ch chan T) {
	b.unsubCh <- ch
}
