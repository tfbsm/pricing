package api

import (
	"time"

	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

type WSMessageType string

var (
	WSMessageTypeServerInfo  WSMessageType = "sinfo"
	WSMessageTypeObservation WSMessageType = "obs"
	WSMessageTypeError       WSMessageType = "error"
)

type WSMessage[T any] struct {
	Type    WSMessageType `json:"type"`
	Message string        `json:"message,omitempty"`
	Data    *T            `json:"data,omitempty"`
}

type WSMessageObservation struct {
	domain.Observation
}

func (m WSMessageObservation) WSMessage() WSMessage[WSMessageObservation] {
	return WSMessage[WSMessageObservation]{
		Type: WSMessageTypeObservation,
		Data: &m,
	}
}

type WSMessageServerInfo struct {
	Time time.Time `json:"time"`
}

func (m WSMessageServerInfo) WSMessage() WSMessage[WSMessageServerInfo] {
	return WSMessage[WSMessageServerInfo]{
		Type: WSMessageTypeServerInfo,
		Data: &m,
	}
}

type WSMessageError struct {
	Error error
}

func (m WSMessageError) WSMessage() WSMessage[WSMessageError] {
	return WSMessage[WSMessageError]{
		Type:    WSMessageTypeError,
		Message: m.Error.Error(),
	}
}
