# syntax=docker/dockerfile:1

FROM ghcr.io/siemens/kas/kas:3.1 AS kas
RUN wget https://github.com/GitTools/GitVersion/releases/download/5.9.0/gitversion-linux-x64-5.9.0.tar.gz && \
    tar -xf gitversion-linux-x64-5.9.0.tar.gz -C /usr/bin/ && \
    chmod 755 /usr/bin/gitversion && \
    rm -rf gitversion-linux-x64-5.9.0.tar.gz
