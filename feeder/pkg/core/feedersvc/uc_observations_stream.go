package feedersvc

import (
	"context"
	"iter"
	"time"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

func (s *service) ObservationsStream(
	ctx context.Context,
	instrument string,
) (iter.Seq[domain.Observation], error) {
	return func(yield func(domain.Observation) bool) {
		for {
			if !yield(domain.Observation{Instrument: domain.Instrument("stub"), ProducedAt: time.Now()}) {
				return
			}
		}
	}, nil
}
