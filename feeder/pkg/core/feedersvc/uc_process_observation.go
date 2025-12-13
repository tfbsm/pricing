package feedersvc

import (
	"context"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
	"github.com/tfbsm/pricing/feeder/pkg/log"
)

func (s *service) ProcessObservation(
	ctx context.Context,
	observation domain.Observation,
) error {
	log.Infof("Message: %+v", observation)

	return nil
}
