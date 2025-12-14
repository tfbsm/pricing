package main

import (
	"context"
	"errors"
	"flag"
	"fmt"
	"net/http"
	"os"
	"os/signal"
	"sync"
	"time"

	"github.com/gorilla/websocket"
	"github.com/tfbsm/pricing/feeder/pkg/log"
)

const (
	pingWriteDeadline = 1 * time.Second
	pingReplyDeadline = 2 * time.Second
)

func ping(ctx context.Context, url string) error {
	conn, _, err := websocket.DefaultDialer.DialContext(ctx, url, http.Header{
		"User-Agent": []string{"Mock-Client/0"},
	})
	if err != nil {
		return fmt.Errorf("websocket dial context: %w", err)
	}
	defer conn.Close()

	pong := make(chan struct{}, 1)

	conn.SetPongHandler(func(appData string) error {
		log.Info("Pong")

		pong <- struct{}{}

		return nil
	})

	readerrs := make(chan error, 1)

	go func() {
		for {
			select {
			case <-ctx.Done():
				return
			default:
			}

			if _, _, err := conn.ReadMessage(); err != nil {
				readerrs <- err
			}
		}
	}()

	if err := conn.WriteControl(
		websocket.PingMessage,
		[]byte{},
		time.Now().Add(pingWriteDeadline),
	); err != nil {
		return fmt.Errorf("write ping control message: %w", err)
	}

	select {
	case <-pong:
		return nil
	case <-time.After(pingReplyDeadline):
		return errors.New("ping reply deadline exceeded")
	case <-readerrs:
		return fmt.Errorf("read message: %w", err)
	}
}

func reader(ctx context.Context, url string) (int64, error) {
	conn, _, err := websocket.DefaultDialer.DialContext(ctx, url, http.Header{
		"User-Agent": []string{"Mock-Client/0"},
	})
	if err != nil {
		return 0, fmt.Errorf("websocket dial context: %w", err)
	}
	defer conn.Close()

	var msgsRead int64

	readerrs := make(chan error, 1)

	go func() {
		for {
			select {
			case <-ctx.Done():
				return
			default:
			}

			if _, _, err := conn.ReadMessage(); err != nil {
				readerrs <- err
			}

			msgsRead++
		}
	}()

	select {
	case <-ctx.Done():
		return msgsRead, nil
	case err := <-readerrs:
		return msgsRead, fmt.Errorf("read message: %w", err)
	}
}

func main() {
	var (
		nclients int
		url      string
	)

	flag.StringVar(&url, "url", "ws://127.0.0.1:8080/instruments/any/ws", "WS host")
	flag.IntVar(&nclients, "n", 1, "Number of concurrent clients")

	flag.Parse()

	ctx, cancel := signal.NotifyContext(context.Background(), os.Interrupt)
	defer cancel()

	// Check
	if err := ping(ctx, url); err != nil {
		log.Error("Ping url: ", err.Error())

		return
	}

	errors := make([]error, nclients)
	nread := make([]int64, nclients)

	var wg sync.WaitGroup

	for i := range nclients {
		wg.Go(func() {
			nread[i], errors[i] = reader(ctx, url)
		})
	}

	wg.Wait()

	var (
		total int64
		errs  int
	)

	for _, nr := range nread {
		total += nr
	}

	for i, err := range errors {
		if err != nil {
			log.Debugf("Error on worker %d: %s", i, err.Error())

			errs += 1
		}
	}

	log.Infof("Number messages read total=%d; errors=%d", total, errs)
	log.Info("done")
}
