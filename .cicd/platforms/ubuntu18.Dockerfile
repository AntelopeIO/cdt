FROM ubuntu:bionic

RUN apt-get update && apt-get upgrade -y && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential \
                                                      cmake \
                                                      curl \
                                                      ninja-build \
                                                      software-properties-common \
                                                      zlib1g-dev

RUN add-apt-repository ppa:git-core/ppa && apt update && apt install -y git

RUN curl -L  https://www.python.org/ftp/python/3.10.6/Python-3.10.6.tgz | tar zx && \
    cd Python* && \
    ./configure --enable-optimizations --prefix=/usr && \
    make -j$(nproc) install && \
    cd .. && \
    rm -rf Python*