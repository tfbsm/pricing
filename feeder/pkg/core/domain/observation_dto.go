package domain

import (
	"errors"
	"time"

	"github.com/shopspring/decimal"
)

var ten = decimal.NewFromInt(10)

type InstrumentCodeDecoder interface {
	GetInstrumentByCode(code uint32) (Instrument, bool)
}

type ObservationDTO struct {
	OptionInstrumentCode uint32
	SpotInstrumentCode   uint32
	PriceExponent        uint32
	MarketPrice          uint64
	PredictedPrice       uint64
	// ProducedAt unix UTC timestamp
	ProducedAt uint64
	CRC32      uint32
}

func (o ObservationDTO) Observation(
	instrumentDecoder InstrumentCodeDecoder,
) (Observation, error) {

	spotInstrument, ok := instrumentDecoder.GetInstrumentByCode(o.SpotInstrumentCode)
	if !ok {
		return Observation{}, errors.New("spot instrument not found")
	}

	optionInstrument, ok := instrumentDecoder.GetInstrumentByCode(o.OptionInstrumentCode)
	if !ok {
		return Observation{}, errors.New("option instrument not found")
	}

	exp := ten.Pow(decimal.NewFromUint64(uint64(o.PriceExponent)))

	price := decimal.NewFromUint64(o.PredictedPrice).Div(exp)

	marketPrice := decimal.NewFromUint64(o.MarketPrice).Div(exp)

	return Observation{
		SpotInstrument:   spotInstrument,
		OptionInstrument: optionInstrument,
		MarketPrice:      marketPrice,
		PredictedPrice:   price,
		ProducedAt:       time.UnixMilli(int64(o.ProducedAt)),
	}, nil
}
