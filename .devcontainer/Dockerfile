# a basic C++ development environment
FROM ubuntu:24.04

RUN apt update && \
    apt install -y curl gcc-14 g++-14 gdb lldb-16 lld-16 \
    build-essential cmake ninja-build git

RUN rm -f /usr/bin/c++ /usr/bin/g++ /usr/bin/gcc /usr/bin/cc && \
    ln /usr/bin/g++-14 /usr/bin/g++ && \
    ln /usr/bin/g++-14 /usr/bin/c++ && \
    ln /usr/bin/gcc-14 /usr/bin/gcc && \
    ln /usr/bin/gcc-14 /usr/bin/cc

RUN apt install -y nodejs npm && npm install -g wscat
