FROM debian:unstable-slim
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    cmake \
    dpkg-dev \
    file \
    g++ \
    gcovr \
    git \
    lcov \
    make \
    ninja-build

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
    clang-13 \
    clang-format-13 \
    clang-tidy-13 \
    libc++-13-dev \
    libc++abi-13-dev \
    lld-13 \
    llvm-13 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/jdbg
COPY . /usr/src/jdbg
CMD ["/bin/bash"]
