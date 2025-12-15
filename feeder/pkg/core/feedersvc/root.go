package feedersvc

import (
	"context"

	"github.com/tfbsm/pricing/feeder/pkg/core"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
	"github.com/tfbsm/pricing/feeder/pkg/util"
)

var _ core.FeederService = (*service)(nil)

type service struct {
	broker util.Broker[*domain.Observation]
}

func New() core.FeederService {
	return &service{
		broker: util.NewBroker[*domain.Observation](),
	}
}

func (s *service) Start(ctx context.Context) error {
	s.broker.Start(ctx)

	return nil
}
