package feedersvc

import (
	"context"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

func (s *service) ProcessObservation(
	ctx context.Context,
	observation *domain.Observation,
) error {
	// log.Infof("Message: %+v", observation)

	s.broker.Publish(observation)

	return nil
}
