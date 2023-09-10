FROM ubuntu:latest

WORKDIR /app
COPY . /app

RUN apt update && \
    apt install -y gcc git make curl tar zip cmake pkg-config build-essential

RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    /vcpkg/bootstrap-vcpkg.sh && \
    /vcpkg/vcpkg integrate install && \
    /vcpkg/vcpkg install json-c

RUN make CFLAGS="-I/vcpkg/installed/x64-linux/include" LDFLAGS="-L/vcpkg/installed/x64-linux/lib"

CMD [ "/bin/bash", "-c", "build/interpreter", "/var/rinha/source.rinha.json" ]
