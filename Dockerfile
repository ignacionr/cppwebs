# a basic C++ development environment
FROM ubuntu:24.04

RUN apt update && \
    apt install -y curl clang-16 gdb lldb-16 lld-16 \
    build-essential cmake ninja-build git

COPY . /app

WORKDIR /app

RUN mkdir -p build && \
    cd build && \
    cmake .. -G Ninja && \
    cmake --build .

ENTRYPOINT ["/app/build/simple_example"]
