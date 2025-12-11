package domain

import (
	"time"

	"github.com/shopspring/decimal"
)

type Observation struct {
	Instrument     Instrument
	PredictedPrice decimal.Decimal
	ProducedAt     time.Time
}
