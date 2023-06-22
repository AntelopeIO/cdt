FROM ubuntu:jammy

RUN apt-get update && apt-get upgrade -y && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential \
                                                      cmake \
                                                      wget \
                                                      git \
                                                      ninja-build \
                                                      python3 \
                                                      pkg-config \
                                                      libboost-all-dev \
                                                      libcurl4-gnutls-dev \
                                                      lsb-release \
                                                      software-properties-common \
                                                      gnupg \
                                                      clang-tidy

RUN wget https://apt.llvm.org/llvm.sh
RUN chmod +x llvm.sh
RUN ./llvm.sh 16

ENV CC=clang-16
ENV CXX=clang++-16