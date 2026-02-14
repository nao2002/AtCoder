FROM python:3.12-slim

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y git
RUN apt-get autoremove -y &&\
    apt-get clean &&\
    rm -rf /usr/local/src/*

RUN pip install --no-cache-dir --upgrade pip &&\
    pip install --no-cache-dir sortedcontainers

WORKDIR /home