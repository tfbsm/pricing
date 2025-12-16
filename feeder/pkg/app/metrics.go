package app

import (
	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promauto"
)

var (
	mUpstreamMessagesProcessed = promauto.NewCounter(prometheus.CounterOpts{
		Name: "tfbsm_feeder_upstream_messages_processed",
		Help: "Number of upstream messages processed",
	})

	mUpstreamMessagesDiscarded = promauto.NewCounter(prometheus.CounterOpts{
		Name: "tfbsm_feeder_upstream_messages_discarded_count",
		Help: "Number of messages from upstream discarded by feeder",
	})

	mUpstreamMessageProcessingErrors = promauto.NewCounter(prometheus.CounterOpts{
		Name: "tfbsm_feeder_upstream_message_processing_errors_count",
		Help: "Number of upstream message processing errors (service errors total)",
	})
)
