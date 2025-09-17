FROM ubuntu:latest
LABEL authors="ctapobep"

RUN apt update
RUN apt install -y \
    g++ gdb \
    libboost-all-dev \
    libssl-dev \
    cmake
RUN yes | unminimize
RUN apt install -y liburing-dev vim less netcat

WORKDIR /opt/dev/

ENTRYPOINT ["top", "-b"]