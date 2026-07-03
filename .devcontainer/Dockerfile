FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Base tools + prerequisites
RUN apt-get update && apt-get install -y \
    ca-certificates \
    gpg \
    lsb-release \
    software-properties-common \
    wget \
    curl \
    git \
    build-essential \
    make \
    && rm -rf /var/lib/apt/lists/*

# Add Kitware repository (latest CMake)
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc \
    | gpg --dearmor - \
    | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' \
    > /etc/apt/sources.list.d/kitware.list

# Install latest stable LLVM/Clang via the official apt.llvm.org script,
# plus latest cmake from Kitware.
RUN wget -qO /tmp/llvm.sh https://apt.llvm.org/llvm.sh && \
    chmod +x /tmp/llvm.sh && \
    /tmp/llvm.sh all && \
    rm /tmp/llvm.sh && \
    apt-get update && apt-get install -y \
        cmake \
        ninja-build && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Resolve the installed clang version and register it as the default.
RUN CLANG_VER=$(ls /usr/bin/clang-[0-9]* 2>/dev/null \
        | sed 's|/usr/bin/clang-||' \
        | sort -n | tail -n1) && \
    test -n "$CLANG_VER" && \
    echo "Configuring clang-${CLANG_VER} as default" && \
    update-alternatives --install /usr/bin/clang           clang           /usr/bin/clang-${CLANG_VER}           100 && \
    update-alternatives --install /usr/bin/clang++         clang++         /usr/bin/clang++-${CLANG_VER}         100 && \
    update-alternatives --install /usr/bin/clangd          clangd          /usr/bin/clangd-${CLANG_VER}          100 && \
    update-alternatives --install /usr/bin/clang-scan-deps clang-scan-deps /usr/bin/clang-scan-deps-${CLANG_VER} 100 && \
    update-alternatives --install /usr/bin/clang-tidy      clang-tidy      /usr/bin/clang-tidy-${CLANG_VER}      100 && \
    update-alternatives --install /usr/bin/clang-format    clang-format    /usr/bin/clang-format-${CLANG_VER}    100 && \
    update-alternatives --install /usr/bin/lldb            lldb            /usr/bin/lldb-${CLANG_VER}            100 && \
    update-alternatives --install /usr/bin/lld             lld             /usr/bin/lld-${CLANG_VER}             100

WORKDIR /workspace