# syntax=docker/dockerfile:1

FROM ghcr.io/siemens/kas/kas:3.3
RUN sudo wget https://github.com/GitTools/GitVersion/releases/download/5.9.0/gitversion-linux-x64-5.9.0.tar.gz && \
    sudo tar -xf gitversion-linux-x64-5.9.0.tar.gz -C /usr/bin/ && \
    sudo chmod 755 /usr/bin/gitversion && \
    sudo rm -rf gitversion-linux-x64-5.9.0.tar.gz
RUN sudo apt-get update && \
    sudo apt-get install --no-install-recommends -y \
        cifs-utils nfs-common jq && \
    sudo apt-get clean && \
    sudo rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
RUN sudo pip install --upgrade python-gitlab
