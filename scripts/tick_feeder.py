import zmq
import json
import time
import enum

from pybit.unified_trading import WebSocket

ws = WebSocket(
    testnet=False,
    channel_type="option",
)

ws_spot = WebSocket(
    testnet=False,
    channel_type="spot",
)

class Market(enum.Enum):
    OPTION = 0
    SPOT = 1


ctx = zmq.Context()
s = ctx.socket(zmq.PUB)

SYMBOL = "BTC-19DEC25-88500-C-USDT"
SPOT_SYMBOL = "BTCUSDT"

LISTEN_ADDR = "tcp://*:5556"

s.bind(LISTEN_ADDR)

print(f"Listening for tick feed consumers on {LISTEN_ADDR}")

def handle_option_tick(message):
    print(f"Option tick: {message}")

    s.send(bytes(json.dumps({
        "type": "tick",
        "ts": message["ts"],
        "market": Market.OPTION.value,
        "symbol": SYMBOL,
        "ask": float(message["data"]["askPrice"]),
        "bid": float(message["data"]["bidPrice"]),
    }), 'ascii'))

def handle_spot_tick(message):
    s.send(bytes(json.dumps({
        "type": "tick",
        "ts": message["ts"],
        "market": Market.SPOT.value, # spot
        "symbol": SPOT_SYMBOL,
        "ask": float(message["data"]["lastPrice"]),
        "bid": float(message["data"]["lastPrice"]),
    }), 'ascii'))

ws_spot.ticker_stream(
    symbol=f"{SPOT_SYMBOL}",
    callback=handle_spot_tick
)

ws.ticker_stream(
    symbol=f"{SYMBOL}",
    callback=handle_option_tick
)

while True:
    time.sleep(1)