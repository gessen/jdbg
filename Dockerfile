FROM debian:unstable-slim
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    catch2 \
    cmake \
    dpkg-dev \
    file \
    g++ \
    gcovr \
    git \
    lcov \
    make \
    meson \
    ninja-build \
    pkg-config

# Clang stuff
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    ca-certificates \
    gnupg \
    wget \
 && wget -q -O - http://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
 && echo "deb http://apt.llvm.org/unstable/ llvm-toolchain main" >> /etc/apt/sources.list \
 && apt-get update \
 && apt-get install -y --no-install-recommends \
    clang \
    clang-format \
    clang-tidy \
    libc++-dev \
    libc++abi-dev \
    lld \
    llvm \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/jdbg
COPY . /usr/src/jdbg
CMD ["/bin/bash"]
