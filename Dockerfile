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
    curl \
    gnupg \
 && gpg --list-keys \
 && curl -L "http://apt.llvm.org/llvm-snapshot.gpg.key" \
  | gpg --dearmor > /etc/apt/keyrings/llvm.gpg \
 && echo "deb [signed-by=/etc/apt/keyrings/llvm.gpg] http://apt.llvm.org/unstable/ llvm-toolchain main" \
  | tee /etc/apt/sources.list.d/llvm.list \
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
