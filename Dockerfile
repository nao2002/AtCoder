# - Ubuntu 26.04 LTS
# - C++23: GCC 15
# - CPython 3.13.7
# - PyPy 7.3.20
# - Rust/Cargo: AHC official local tools (gen/tester/vis)

ARG UV_VERSION=0.11.32
FROM ghcr.io/astral-sh/uv:${UV_VERSION} AS uv

FROM ubuntu:26.04

ARG DEBIAN_FRONTEND=noninteractive
ARG PYTHON_VERSION=3.13.7

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        curl \
        git \
        unzip \
        build-essential \
        g++-15 \
        gdb \
        pypy3 \
        pkg-config \
        libfontconfig-dev \
        locales \
    && printf 'en_US.UTF-8 UTF-8\n' > /etc/locale.gen \
    && locale-gen \
    && rm -rf /var/lib/apt/lists/*

ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

COPY --from=uv /uv /uvx /usr/local/bin/

# CPython: match the AtCoder runtime.
RUN uv venv --seed --python "${PYTHON_VERSION}" /home/venv \
    && uv pip install --python /home/venv/bin/python \
        sortedcontainers==2.4.0

# PyPy: keep its venv out of PATH and expose only PyPy-named commands.
RUN uv venv --seed --python /usr/bin/pypy3 /home/pypy-venv \
    && uv pip install --python /home/pypy-venv/bin/python \
        sortedcontainers==2.4.0 \
    && printf '#!/bin/sh\nexec /home/pypy-venv/bin/python "$@"\n' > /usr/local/bin/pypy3 \
    && chmod +x /usr/local/bin/pypy3 \
    && ln -s /usr/local/bin/pypy3 /usr/local/bin/pypy \
    && ln -s /usr/local/bin/pypy3 /usr/local/bin/pypy3.11

# Rust is only for AtCoder AHC local tools.
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs \
    | sh -s -- -y --profile minimal --default-toolchain stable

ENV PATH="/home/venv/bin:/root/.cargo/bin:${PATH}"

# Fail the image build if the expected contest/tool environment is broken.
RUN python --version 2>&1 | grep -Fx "Python 3.13.7" \
    && python -c "import sortedcontainers; assert sortedcontainers.__version__ == '2.4.0'" \
    && pypy3 -c "import sys; assert sys.version_info[:2] == (3, 11); assert tuple(sys.pypy_version_info)[:3] == (7, 3, 20)" \
    && pypy3 -c "import sortedcontainers; assert sortedcontainers.__version__ == '2.4.0'" \
    && test "$(g++-15 -dumpfullversion)" = "15.2.0" \
    && printf 'int main(){return 0;}\n' | g++-15 -std=gnu++23 -O2 -x c++ - -o /tmp/cpp23-check \
    && /tmp/cpp23-check \
    && rm -f /tmp/cpp23-check \
    && pkg-config --exists fontconfig \
    && rustc --version \
    && cargo --version

WORKDIR /workspace
