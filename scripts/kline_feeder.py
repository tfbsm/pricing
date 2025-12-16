import zmq
import json
import time

ctx = zmq.Context()
s = ctx.socket(zmq.PUB)

LISTEN_ADDR = "tcp://*:5556"

s.bind(LISTEN_ADDR)

print(f"Listening for kline feed consumers on {LISTEN_ADDR}")

while True:
    s.send(bytes(json.dumps({
        "type": "kline",
        "ts": time.time(),
        "symbol": "BTC-1337-C",
        "open": 1337.0,
        "high": 1337.5,
        "low": 1336.9,
        "close": 1337.2,
        "volume": 12345.1,
        "timescale_s": 1,
    }), 'ascii'))
    
    time.sleep(1)