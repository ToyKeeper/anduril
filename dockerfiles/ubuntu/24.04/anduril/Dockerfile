FROM base

ARG DOCKERFILEPATH=""
ARG TIMESTAMP=""
ARG COMMIT=""

LABEL maintainer="https://gitlab.com/cjones"
LABEL docker-dir=${DOCKERFILEPATH}
LABEL update-date=${TIMESTAMP}
LABEL commit=${COMMIT}
LABEL description="Ubuntu 24.04 compile container for Andúril 2"

# Use these for possible debugging
ENV DOCKER_FOLDER_PATH="${DOCKERFILEPATH}"
ENV DOCKER_UPDATE_DATE="${TIMESTAMP}"
ENV DOCKER_COMMIT_SHA="${COMMIT}"


# Use this to update the 24.04 base image,
# Install ONLY the necessary tools to build Andúril 2 that aren't already provided in 24.04 base
# and then clean up.
RUN DEBIAN_FRONTEND=noninteractive apt-get update \
    && apt-get install -y --no-install-suggests --no-install-recommends \
    avr-libc \
    binutils-avr \
    cmark-gfm \
    gcc-avr \
    git \
    make \
    python-is-python3 \
    unzip \
    wget \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root
