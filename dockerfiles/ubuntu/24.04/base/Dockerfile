FROM ubuntu:24.04

ARG DOCKERFILEPATH=""
ARG TIMESTAMP=""
ARG COMMIT=""

LABEL maintainer="https://gitlab.com/cjones"
LABEL docker-dir=${DOCKERFILEPATH}
LABEL update-date=${TIMESTAMP}
LABEL commit=${COMMIT}
LABEL description="Ubuntu 24.04 base container for Andúril 2"

# Use these for possible debugging
ENV DOCKER_FOLDER_PATH="${DOCKERFILEPATH}"
ENV DOCKER_UPDATE_DATE="${TIMESTAMP}"
ENV DOCKER_COMMIT_SHA="${COMMIT}"


# Use this to update the 24.04 image and clean up.
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get upgrade -y \
    && apt-get clean \
    && apt-get autoremove

WORKDIR /root
