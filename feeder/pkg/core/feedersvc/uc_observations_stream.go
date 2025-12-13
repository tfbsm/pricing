package feedersvc

import (
	"context"
	"iter"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

func (s *service) ObservationsStream(
	ctx context.Context,
	instrument string,
) (iter.Seq[*domain.Observation], error) {
	return func(yield func(*domain.Observation) bool) {
		mDownstreamClients.Inc()
		defer mDownstreamClients.Dec()

		obss := make(chan *domain.Observation, 1)

		s.broker.Subscribe(obss)
		defer s.broker.Unsubscribe(obss)

		for {
			select {
			case <-ctx.Done():
				return
			case obs := <-obss:
				if !yield(obs) {
					return
				}

				mObservationsDispatched.Inc()
			}
		}
	}, nil
}
