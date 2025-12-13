# Price sink feeder

Price feeder is a component used to feed price predicted by C++ pricing engine to different sinks:
 - Websockets
 - Kafka (TBD)

It receives price via ZMQ and broadcasts it to all consumers.

PS. It was heavilly adopted from our previous project ([okx-exporter](https://github.com/gavt45/okx-exporter))

## Simple monitoring setup for local charting

docker run --rm -p 9090:9090 \
  -v `pwd`/config/prometheus.yml:/etc/prometheus/prometheus.yml \
  prom/prometheus

