package app

import (
	"net/http"

	"github.com/go-chi/chi/v5/middleware"
	"github.com/prometheus/client_golang/prometheus/promhttp"
	"github.com/tfbsm/pricing/feeder/pkg/app/handlers"
)

func (a *sinkFeederApp) initMiddleware() {
	a.router.Use(middleware.Logger)
	a.router.Use(middleware.Recoverer)
}

func (a *sinkFeederApp) initRoutes() {
	a.router.Get("/metrics", promhttp.Handler().ServeHTTP)

	a.router.Get("/healthz", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("{\"message\": \"ok\"}"))
	})

	wsrg := handlers.NewWSHandlers(a.svc)

	a.router.HandleFunc("/instruments/{instrument}/ws", wsrg.Entrypoint)
}
