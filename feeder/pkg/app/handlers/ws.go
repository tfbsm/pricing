package handlers

import (
	"context"
	"errors"
	"fmt"
	"net/http"
	"time"

	"github.com/go-chi/chi/v5"
	"github.com/gorilla/websocket"
	"github.com/tfbsm/pricing/feeder/pkg/core"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain/api"
	"github.com/tfbsm/pricing/feeder/pkg/log"
	"golang.org/x/sync/errgroup"
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

func (hg *WSHandlerGroup) connReader(ctx context.Context, conn *websocket.Conn) error {
	for {
		select {
		case <-ctx.Done():
			return nil
		default:
		}

		_, _, err := conn.ReadMessage()
		if err != nil {
			return fmt.Errorf("read message: %w", err)
		}
	}
}

func (hg *WSHandlerGroup) observationWriter(
	ctx context.Context,
	conn *websocket.Conn,
	instrument domain.Instrument,
) error {
	var err error

	obsIter, err := hg.svc.ObservationsStream(ctx, instrument)
	if err != nil {
		return fmt.Errorf("observations stream: %w", err)
	}

	obsIter(func(observation *domain.Observation) bool {
		err = conn.WriteJSON(api.WSMessageObservation{Observation: observation}.WSMessage())

		return err == nil
	})

	if err != nil {
		return fmt.Errorf("write observation json: %w", err)
	}

	return nil
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

	conn.SetPingHandler(func(appData string) error {
		log.Debug("Ping: ", appData)

		if err := conn.WriteControl(websocket.PongMessage, []byte{}, time.Now().Add(time.Second)); err != nil {
			log.Debug("Write control err: ", err.Error())

			return fmt.Errorf("write pong control: %w", err)
		}

		return nil
	})

	conn.WriteJSON(api.WSMessageServerInfo{Time: time.Now()}.WSMessage())

	egrp, ctx := errgroup.WithContext(r.Context())

	egrp.Go(func() error {
		return hg.connReader(ctx, conn)
	})

	egrp.Go(func() error {
		return hg.observationWriter(ctx, conn, domain.Instrument(instrument))
	})

	if err := egrp.Wait(); err != nil {
		var closeErr *websocket.CloseError

		if !errors.As(err, &closeErr) || closeErr.Code != websocket.CloseAbnormalClosure {
			log.Debug("WS conn error: ", err.Error())
		}

		conn.WriteJSON(api.WSMessageError{Error: err}.WSMessage())

		return
	}
}
