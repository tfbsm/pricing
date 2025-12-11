package sink

import (
	"context"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

type DataSink interface {
	Send(ctx context.Context, observation ...domain.Observation) error
}
