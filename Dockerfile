# gcc 15のインストール簡易化のためubuntuを25.10へ
# 26以降が出次第latestへ移行予定
FROM ubuntu:25.10

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y git curl wget vim

RUN apt-get install -y python3 python3-pip python3.13-venv
RUN apt-get install -y build-essential gcc-15 g++-15

RUN apt-get autoremove -y &&\
    apt-get clean &&\
    rm -rf /var/lib/apt/lists/*

WORKDIR /home

# python setup
RUN python3 -m venv /home/venv
ENV PATH="/home/venv/bin:${PATH}"
RUN pip install --no-cache-dir --upgrade pip &&\
    pip install --no-cache-dir sortedcontainers
