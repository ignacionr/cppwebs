# a basic C++ development environment
FROM ubuntu:24.04

RUN apt update && \
    apt install -y curl clang-16 gdb lldb-16 lld-16 \
    build-essential cmake ninja-build git

RUN apt install -y nodejs npm && npm install -g wscat
