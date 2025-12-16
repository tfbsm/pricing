package domain

import (
	"time"

	"github.com/shopspring/decimal"
)

type Observation struct {
	SpotInstrument   Instrument      `json:"spot_instrument"`
	OptionInstrument Instrument      `json:"option_instrument"`
	MarketPrice      decimal.Decimal `json:"market"`
	PredictedPrice   decimal.Decimal `json:"predicted_price"`
	ProducedAt       time.Time       `json:"ts"`
}
