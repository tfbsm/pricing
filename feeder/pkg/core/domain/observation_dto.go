package domain

import (
	"fmt"
	"time"

	"github.com/shopspring/decimal"
)

var ten = decimal.NewFromInt(10)

type ObservationDTO struct {
	InstrumentCode uint32
	PriceExponent  uint32
	PredictedPrice uint64
	// ProducedAt unix UTC timestamp
	ProducedAt uint64
	CRC32      uint32
}

func (o ObservationDTO) Observation() Observation {
	price := decimal.NewFromUint64(o.PredictedPrice).
		Div(ten.Pow(decimal.NewFromUint64(uint64(o.PriceExponent))))

	return Observation{
		Instrument:     Instrument(fmt.Sprintf("%d", o.InstrumentCode)),
		PredictedPrice: price,
		ProducedAt:     time.UnixMilli(int64(o.ProducedAt)),
	}
}
