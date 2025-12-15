package upstream

import (
	"context"
)

type Client interface {
	Connect(ctx context.Context) error
	Subscribe(handler func(context.Context, []byte))
	Start(ctx context.Context) error
	Close() error
}
