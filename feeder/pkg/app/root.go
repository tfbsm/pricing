package app

import (
	"context"
	"encoding/binary"
	"fmt"
	"net"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"syscall"
	"time"

	"github.com/go-chi/chi/v5"
	"github.com/tfbsm/pricing/feeder/pkg/config"
	"github.com/tfbsm/pricing/feeder/pkg/core"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
	"github.com/tfbsm/pricing/feeder/pkg/core/feedersvc"
	"github.com/tfbsm/pricing/feeder/pkg/infra/upstream"
	"github.com/tfbsm/pricing/feeder/pkg/log"
	"golang.org/x/sync/errgroup"
)

type App interface {
	Start(ctx context.Context) error
}

var _ App = (*sinkFeederApp)(nil)

type sinkFeederApp struct {
	cfg    *config.ServiceConfig
	client upstream.Client
	svc    core.FeederService

	router *chi.Mux
}

func New(cfg *config.ServiceConfig) (App, error) {
	var err error

	app := &sinkFeederApp{
		cfg:    cfg,
		router: chi.NewRouter(),
	}

	app.svc = feedersvc.New()

	app.client, err = upstream.NewZMQClient(&cfg.Upstream)
	if err != nil {
		return nil, fmt.Errorf("new zmq client: %w", err)
	}

	app.addZMQHandlers()
	app.initMiddleware()
	app.initRoutes()

	return app, nil
}

func (s *sinkFeederApp) addZMQHandlers() {
	s.client.Subscribe(func(ctx context.Context, b []byte) {
		var dto domain.ObservationDTO

		_, err := binary.Decode(b, binary.BigEndian, &dto)
		if err != nil {
			log.Info("Can't decode observation: ", err.Error())

			return
		}

		obs := dto.Observation()

		// log.Infof("Message: %+v", dto.Observation())

		if err := s.svc.ProcessObservation(ctx, &obs); err != nil {
			log.Info("Can't process observation: ", err.Error())

			return
		}
	})
}

// Start implements App.
func (s *sinkFeederApp) Start(ctx context.Context) error {
	if err := s.client.Connect(ctx); err != nil {
		return fmt.Errorf("zmq connect: %w", err)
	}

	defer s.client.Close()

	grp, ctx := errgroup.WithContext(ctx)

	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)

	grp.Go(func() error {
		select {
		case sig := <-sigs:
			return fmt.Errorf("stopping due to signal %d", sig)
		case <-ctx.Done():
			return nil
		}
	})

	grp.Go(func() error {
		return s.client.Start(ctx)
	})

	grp.Go(func() error {
		return s.svc.Start(ctx)
	})

	grp.Go(func() error {
		// Capacity 1, so read is not blocked when channel is empty
		errs := make(chan error, 1)
		srv := &http.Server{
			Addr:              net.JoinHostPort(s.cfg.Host, strconv.Itoa(s.cfg.Port)),
			ReadTimeout:       10 * time.Second,
			ReadHeaderTimeout: 10 * time.Second,
			Handler:           s.router,
		}

		go func() {
			log.Info("Server is listening on ", srv.Addr)

			if err := srv.ListenAndServe(); err != http.ErrServerClosed {
				errs <- err
			}
		}()

		select {
		case <-ctx.Done():
			// parent context is closed, so we create a new one here
			ctxTimeout, cancel := context.WithTimeout(context.Background(), hardShutdownTimeout)
			defer cancel()

			return srv.Shutdown(ctxTimeout)
		case err := <-errs:
			return err
		}
	})

	return grp.Wait()
}
