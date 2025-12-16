import zmq
import json
import time

ctx = zmq.Context()
s = ctx.socket(zmq.PUB)

LISTEN_ADDR = "tcp://*:5556"

s.bind(LISTEN_ADDR)

print(f"Listening for tick feed consumers on {LISTEN_ADDR}")

while True:
    s.send(bytes(json.dumps({
        "type": "tick",
        "ts": time.time(),
        "market": 0,
        "symbol": "BTC-1337-C",
        "ask": 1000.1,
        "bid": 1000.2,
    }), 'ascii'))
    
    time.sleep(1)