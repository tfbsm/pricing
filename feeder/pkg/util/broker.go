package util

import (
	"context"
)

type Broker[T any] struct {
	stopCh    chan struct{}
	publishCh chan T
	unsubCh   chan chan T
	subCh     chan chan T
}

func NewBroker[T any]() Broker[T] {
	return Broker[T]{
		stopCh:    make(chan struct{}, 1),
		publishCh: make(chan T, 1),
		subCh:     make(chan chan T, 1),
		unsubCh:   make(chan chan T, 1),
	}
}

func (b Broker[T]) Start(ctx context.Context) {
	subscriptions := make(map[chan T]struct{})

	for {
		select {
		case <-b.stopCh:
			return
		case <-ctx.Done():
			return
		case sub := <-b.subCh:
			subscriptions[sub] = struct{}{}
		case unsub := <-b.unsubCh:
			delete(subscriptions, unsub)
		case msg := <-b.publishCh:
			for ch := range subscriptions {
				select {
				case ch <- msg:
				default:
					// do not block here
				}
			}
		}
	}
}

func (b Broker[T]) Close() {
	b.stopCh <- struct{}{}
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
