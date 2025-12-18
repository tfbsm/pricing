import zmq
import json
import time
import enum

class Market(enum.Enum):
    OPTION = 0
    SPOT = 1


ctx = zmq.Context()
s = ctx.socket(zmq.PUB)

SYMBOL = "BTC-26DEC25-86000-C"
SPOT_SYMBOL = SYMBOL.split("-")[0]

LISTEN_ADDR = "tcp://*:5556"

s.bind(LISTEN_ADDR)

print(f"Listening for tick feed consumers on {LISTEN_ADDR}")

while True:
    s.send(bytes(json.dumps({
        "type": "tick",
        "ts": time.time(),
        "market": Market.OPTION.value,
        "symbol": SYMBOL,
        "ask": 1000.1,
        "bid": 1000.2,
    }), 'ascii'))

    s.send(bytes(json.dumps({
        "type": "tick",
        "ts": time.time(),
        "market": Market.SPOT.value, # spot
        "symbol": SPOT_SYMBOL,
        "ask": 85990,
        "bid": 85991.2,
    }), 'ascii'))
    
    time.sleep(1)