FROM debian:bookworm AS build

RUN apt update && apt install -y build-essential clang cmake git \
    python3 clang-format cmake-format


RUN git clone https://github.com/zeromq/libzmq.git && \
    cd libzmq && mkdir build && cd build \
    cmake .. && \
    make -j4

WORKDIR /build

COPY ./.clang-format /build/
COPY ./.cmake-format /build/

COPY ./cmake/ /build/cmake/
COPY ./.git/ /build/.git/
COPY ./include/ /build/include/
COPY ./src/ /build/src/
COPY ./tests/ /build/tests/

COPY ./CMakeLists.txt /build/
COPY ./Makefile /build/

RUN make build

RUN chmod +x build/test_app

FROM gcr.io/distroless/static-debian12

COPY --from=build /build/build/test_app /test_app

CMD [ "/test_app" ]
