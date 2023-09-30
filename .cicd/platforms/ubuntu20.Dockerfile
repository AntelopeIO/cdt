FROM ubuntu:focal

RUN apt-get update && apt-get upgrade -y && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential \
                                                      cmake \
                                                      git \
                                                      ninja-build