package handlers

import (
	"context"
	"errors"
	"net/http"
	"time"

	"github.com/go-chi/chi/v5"
	"github.com/gorilla/websocket"
	"github.com/tfbsm/pricing/feeder/pkg/core"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain/api"
)

type WSHandlerGroup struct {
	upgrader websocket.Upgrader
	svc      core.FeederService
}

func NewWSHandlers(
	svc core.FeederService,
) *WSHandlerGroup {
	hg := &WSHandlerGroup{
		svc: svc,
	}

	hg.upgrader = websocket.Upgrader{
		HandshakeTimeout: 3 * time.Second,
		Error:            hg.WSError,
	}

	return hg
}

func (hg *WSHandlerGroup) WSError(w http.ResponseWriter, r *http.Request, status int, reason error) {
	api.NewError(status, reason).Write(w) //nolint:errcheck
}

func (hg *WSHandlerGroup) Entrypoint(w http.ResponseWriter, r *http.Request) {
	instrument := chi.URLParam(r, "instrument")

	// TODO: check instrument exists
	if instrument != "any" {
		api.NewError(http.StatusNotFound, errors.New("instrument not found")).Write(w)

		return
	}

	conn, err := hg.upgrader.Upgrade(w, r, nil)
	if err != nil {
		return // Error is already handled by WSError
	}
	defer conn.Close()

	conn.WriteJSON(api.WSMessageServerInfo{Time: time.Now()}.WSMessage())

	ctx, cancel := context.WithCancel(r.Context())
	defer cancel()

	obsIter, err := hg.svc.ObservationsStream(ctx, instrument)
	if err != nil {
		conn.WriteJSON(api.WSMessageError{Error: err}.WSMessage())

		return
	}

	obsIter(func(observation *domain.Observation) bool {
		err := conn.WriteJSON(api.WSMessageObservation{Observation: observation}.WSMessage())
		if err != nil {
			conn.WriteJSON(api.WSMessageError{Error: err}.WSMessage())

			return false
		}

		return true
	})
}
