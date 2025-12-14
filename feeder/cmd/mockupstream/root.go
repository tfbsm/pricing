package main

import (
	"encoding/binary"
	"flag"
	"log"
	"math/rand"
	"time"

	zmq "github.com/pebbe/zmq4"
	"github.com/tfbsm/pricing/feeder/pkg/core/domain"
)

func main() {
	var (
		listenHost   string
		sendInterval time.Duration
		sendJitter   time.Duration
		jitter       int64
	)

	flag.DurationVar(&sendInterval, "interval", time.Second, "Interval between message sends")
	flag.DurationVar(&sendJitter, "jitter", 500*time.Millisecond, "Jitter between message sends")
	flag.StringVar(&listenHost, "host", "tcp://*:5555", "ZeroMQ bind host")

	flag.Parse()

	srv, err := zmq.NewSocket(zmq.PUB)
	if err != nil {
		log.Fatalln("Make socket: ", err.Error())
	}

	if err := srv.Bind(listenHost); err != nil {
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

		if sendJitter != 0 {
			// -jitter/2 to jitter/2
			jitter = rand.Int63n(int64(sendJitter)) - int64(sendJitter)/2
		}

		time.Sleep(sendInterval + time.Duration(jitter))
	}
}
