package feedersvc

import (
	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promauto"
)

var (
	mObservationsDispatched = promauto.NewCounter(prometheus.CounterOpts{
		Name: "tfbsm_feeder_observations_dispatched",
		Help: "Total observations dispatched to clients",
	})

	mDownstreamClients = promauto.NewGauge(prometheus.GaugeOpts{
		Name: "tfbsm_feeder_downstream_clients",
		Help: "Number of active downstream clients consuming feeds",
	})
)
