package main

import (
	"context"

	"github.com/go-playground/validator/v10"
	"github.com/heetch/confita"
	"github.com/heetch/confita/backend/env"
	"github.com/heetch/confita/backend/file"
	"github.com/heetch/confita/backend/flags"
	"github.com/tfbsm/pricing/feeder/pkg/app"
	"github.com/tfbsm/pricing/feeder/pkg/config"
	"github.com/tfbsm/pricing/feeder/pkg/log"
)

var (
	Version   = "unknown"
	BuildDate = "unknown"
	GitCommit = "unknown"
)

func main() {
	log.Info("Starting sink feeder ver: ", Version, " build date: ", BuildDate, " commit: ", GitCommit)

	loader := confita.NewLoader(
		env.NewBackend(),
		file.NewOptionalBackend("config.yaml"),
		file.NewOptionalBackend("config/config.yaml"),
		flags.NewBackend(),
	)

	cfg := config.ServiceConfig{}

	err := loader.Load(context.Background(), &cfg)
	if err != nil {
		log.Fatal("Can't load config: ", err.Error())

		return
	}

	v := validator.New()

	if err = v.Struct(cfg); err != nil {
		log.Fatal("Can't validate config: ", err.Error())

		return
	}

	app, err := app.New(&cfg)
	if err != nil {
		log.Fatal("Can't create app: ", err.Error())

		return
	}

	err = app.Start(context.Background())
	if err != nil {
		log.Fatal("App error: ", err.Error())

		return
	}

	log.Info("Done!")
}
