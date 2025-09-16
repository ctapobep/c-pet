FROM ubuntu:latest
LABEL authors="ctapobep"

RUN apt update
RUN apt install -y \
    g++ gdb \
    libboost-all-dev \
    libssl-dev \
    cmake

ENTRYPOINT ["top", "-b"]