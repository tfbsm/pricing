package core

import (
	"context"
	"iter"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

type FeederService interface {
	ProcessObservation(
		ctx context.Context,
		observation domain.Observation,
	) error

	ObservationsStream(
		ctx context.Context,
		instrument string,
	) (iter.Seq[domain.Observation], error)
}
