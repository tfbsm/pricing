package main

import (
	"encoding/binary"
	"log"
	"time"

	zmq "github.com/pebbe/zmq4"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

func main() {
	srv, err := zmq.NewSocket(zmq.PUB)
	if err != nil {
		log.Fatalln("Make socket: ", err.Error())
	}

	if err := srv.Bind("tcp://*:5555"); err != nil {
		log.Fatalln("Bind server: ", err.Error())
	}

	defer srv.Close()

	sendbuffer := make([]byte, 32)

	for {
		observation := domain.ObservationDTO{
			InstrumentCode: 1234,
			PredictedPrice: 1337,
			PriceExponent:  2,
			ProducedAt:     uint64(time.Now().UnixMilli()),
			CRC32:          0,
		}

		_, err := binary.Encode(sendbuffer, binary.BigEndian, observation)
		if err != nil {
			log.Fatalln("Can't encode observation: ", err.Error())
		}

		_, err = srv.SendBytes(sendbuffer, 0)
		if err != nil {
			log.Fatalln("Can't send bytes: ", err.Error())
		}

		time.Sleep(time.Millisecond)
	}
}
