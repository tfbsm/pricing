package feedersvc

import "github.com/tfbsm/pricing/feeder/pkg/core"

var _ core.FeederService = (*service)(nil)

type service struct {
}

func New() core.FeederService {
	return &service{}
}
