# Price sink feeder

Price feeder is a component used to feed price predicted by C++ pricing engine to different sinks:
 - Websockets
 - Kafka (TBD)

It receives price via ZMQ and broadcasts it to all consumers.

PS. It was heavilly adopted from our previous project ([okx-exporter](https://github.com/gavt45/okx-exporter))

## Build and run

Build container:
```console
$ make docker
```

Run upstream (pricing engine) or mock upstream:
```console
$ docker run --rm -p 5555:5555 ghcr.io/tfbsm/pricing/feeder:latest /mockupstream -interval 1ms -jitter 0 -host tcp://\*:5555
2025-12-14T06:23:27.427Z	INFO	mockupstream/root.go:43	Upstream running on tcp://*:5555
```

Run feeder:
```console
$ docker run --rm -p 8080:8080 -e upstream_endpoint=tcp://host.docker.internal:5555 ghcr.io/tfbsm/pricing/feeder:latest
2025-12-14T06:24:18.986Z        INFO    feeder/root.go:22       Starting sink feeder ver:  build date: 2025-12-14T06:16:59 commit: 1beec4c6a6da28cb9809550dedda5b5eddc8a116
2025-12-14T06:24:18.987Z        DEBUG   upstream/zmq.go:73      Connecting to upstream tcp://host.docker.internal:5555
2025-12-14T06:24:18.987Z        INFO    app/root.go:125 Server is listening on 0.0.0.0:8080
2025/12/14 06:24:23 "GET http://host.docker.internal:8080/metrics HTTP/1.1" from 151.101.194.132:58765 - 200 2139B in 536.916µs
2025/12/14 06:24:28 "GET http://host.docker.internal:8080/instruments/any/ws HTTP/1.1" from 151.101.194.132:49576 - 000 0B in 2.252666ms
2025/12/14 06:24:28 "GET http://host.docker.internal:8080/metrics HTTP/1.1" from 151.101.194.132:58765 - 200 2157B in 1.343ms
2025/12/14 06:24:32 "GET http://host.docker.internal:8080/instruments/any/ws HTTP/1.1" from 151.101.194.132:22819 - 000 0B in 3.948907544s
```

Run mock client:
```console
$ docker run --rm ghcr.io/tfbsm/pricing/feeder:latest /mockclient -n 1 -url ws://host.docker.internal:8080/instruments/any/ws
2025-12-14T06:24:28.350Z	INFO	mockclient/root.go:35	Pong
^C2025-12-14T06:24:32.301Z	INFO	mockclient/root.go:164	Number messages read total=3409; errors=0
2025-12-14T06:24:32.301Z	INFO	mockclient/root.go:165	done
```

## Simple monitoring setup for local charting

docker run -d --name tfbsm-prom -p 9090:9090 \
  -v `pwd`/config/prometheus.yml:/etc/prometheus/prometheus.yml \
  prom/prometheus

