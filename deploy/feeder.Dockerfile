FROM golang:1.25 AS build

RUN apt update && apt install -y libzmq5-dev

WORKDIR /build

COPY ./.git/ /build/.git/
COPY ./feeder/Makefile .
COPY ./feeder/vendor/ /build/vendor/
COPY ./feeder/pkg/ /build/pkg/
COPY ./feeder/go.mod .
COPY ./feeder/cmd/ /build/cmd/

RUN make prebuild build-linux

FROM alpine:3.21

# See: https://stackoverflow.com/questions/66963068/docker-alpine-executable-binary-not-found-even-if-in-path
RUN apk add gcompat libzmq

WORKDIR /

COPY --from=build /build/dist/linux/feeder .
COPY --from=build /build/dist/linux/mockclient .
COPY --from=build /build/dist/linux/mockupstream .

COPY ./feeder/config /config/

CMD ["/feeder"]