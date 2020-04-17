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
    clang-11 \
    clang-format-11 \
    clang-tidy-11 \
    libc++-dev-11 \
    libc++abi-dev-11 \
    lld-11 \
    llvm-11 \
    clangd-11 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/jdbg
COPY . /usr/src/jdbg
CMD ["/bin/bash"]
