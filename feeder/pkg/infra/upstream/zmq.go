package upstream

import (
	"context"
	"errors"
	"fmt"
	"sync"
	"sync/atomic"
	"syscall"

	zmq "github.com/pebbe/zmq4"
	"github.com/tfbsm/pricing/feeder/pkg/config"
	"github.com/tfbsm/pricing/feeder/pkg/log"
)

var _ Client = (*zmqClient)(nil)

type zmqClient struct {
	cfg *config.PriceUpstreamConfig

	socketConnected atomic.Bool

	socket   *zmq.Socket
	socketMu sync.Mutex

	data chan []byte

	handlers []func(context.Context, []byte)

	closeOnce sync.Once
}

func NewZMQClient(cfg *config.PriceUpstreamConfig) (Client, error) {
	socket, err := zmq.NewSocket(zmq.SUB)
	if err != nil {
		return nil, fmt.Errorf("init zmq socket: %w", err)
	}

	return &zmqClient{
		cfg:    cfg,
		socket: socket,
		data:   make(chan []byte, 1),
	}, nil
}

// Close implements Client.
func (z *zmqClient) Close() (err error) {
	z.closeOnce.Do(func() {
		z.socketMu.Lock()
		defer z.socketMu.Unlock()

		err = z.socket.Close()

		z.socketConnected.Store(false)
	})

	return
}

// Connect implements Client.
func (z *zmqClient) Connect(ctx context.Context) error {
	if z.socketConnected.Load() {
		return errors.New("socket already connected")
	}

	z.socketMu.Lock()
	defer z.socketMu.Unlock()

	if err := z.socket.SetSubscribe(""); err != nil {
		return fmt.Errorf("zmq socket subscribe: %w", err)
	}

	log.Debug("Connecting to upstream ", z.cfg.Endpoint)

	err := z.socket.Connect(z.cfg.Endpoint)
	if err != nil {
		return fmt.Errorf("zmq socket connect: %w", err)
	}

	z.socketConnected.Store(true)

	return nil
}

func (z *zmqClient) receive() error {
	for z.socketConnected.Load() {
		data, err := z.socket.RecvBytes(zmq.DONTWAIT)
		if errors.Is(zmq.AsErrno(err), zmq.Errno(syscall.EAGAIN)) {
			continue
		} else if err != nil {
			return fmt.Errorf("recv bytes: %w", err)
		}

		z.data <- data
	}

	return nil
}

// Start implements Client.
func (z *zmqClient) Start(ctx context.Context) error {
	recverrs := make(chan error, 1)

	go func() {
		recverrs <- z.receive()
	}()

	ctx, cancel := context.WithCancel(ctx)
	defer cancel()

	for {
		select {
		case receiveErr := <-recverrs:
			if err := z.Close(); err != nil {
				return fmt.Errorf("receive: %w; close: %w", receiveErr, err)
			}

			return fmt.Errorf("receive: %w", receiveErr)
		case <-ctx.Done():
			return z.Close()
		case data := <-z.data:
			for _, h := range z.handlers {
				h(ctx, data)
			}
		}
	}
}

// Subscribe implements Client.
func (z *zmqClient) Subscribe(handler func(context.Context, []byte)) {
	z.handlers = append(z.handlers, handler)
}
