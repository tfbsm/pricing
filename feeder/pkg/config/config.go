package config

type PriceUpstreamConfig struct {
	Endpoint string `json:"endpoint" config:"upstream_endpoint" validate:"required,gt=0"`
}

type ServiceConfig struct {
	Host string `json:"host" yaml:"host" config:"host" validate:"required"`
	Port int    `json:"port" yaml:"port" config:"port" validate:"required"`

	Upstream PriceUpstreamConfig `json:"upstream" config:"upstream"`
	// OKX  OKXConfig      `json:"okx" yaml:"okx" config:"okx"`
	// Sink ClickhouseSink `json:"clickhouse_sink" yaml:"clickhouse_sink" config:"ch_sink"`
}
