package config

import "github.com/tfbsm/pricing/feeder/pkg/core/domain"

type PriceUpstreamConfig struct {
	Endpoint string `json:"endpoint" config:"upstream_endpoint" validate:"required,gt=0"`
}

var _ domain.InstrumentCodeDecoder = (*InstrumentCodesConfig)(nil)

type InstrumentCodesConfig struct {
	Codes map[int]domain.Instrument `json:"codes"`
}

// GetInstrumentByCode implements [domain.InstrumentCodeDecoder].
func (i *InstrumentCodesConfig) GetInstrumentByCode(code uint32) (domain.Instrument, bool) {
	return i.Codes[int(code)], true
}

type ServiceConfig struct {
	Host string `json:"host" yaml:"host" config:"host" validate:"required"`
	Port int    `json:"port" yaml:"port" config:"port" validate:"required"`

	Upstream PriceUpstreamConfig `json:"upstream" config:"upstream"`

	InstrumentCodes InstrumentCodesConfig `json:"instrument_codes"`
	// OKX  OKXConfig      `json:"okx" yaml:"okx" config:"okx"`
	// Sink ClickhouseSink `json:"clickhouse_sink" yaml:"clickhouse_sink" config:"ch_sink"`
}
