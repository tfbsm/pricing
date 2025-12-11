package core

import (
	"context"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
	"github.com/tfbsm/pricing/feeder/pkg/log"
)

type Service struct{}

func NewService() *Service {
	return &Service{}
}

func (s *Service) ProcessObservation(
	ctx context.Context,
	observation domain.Observation,
) error {
	log.Infof("Message: %+v", observation)

	return nil
}
