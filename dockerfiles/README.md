# Andúril 2 Dockerfiles

## Table of Contents
- [Andúril 2 Dockerfiles](#Andúril2Dockerfiles)
  - [Table of Contents](#tableofcontents)
  - [Installing Docker](#installingdocker)
    - [Install Docker on Linux](#installdockeronlinux)
    - [Install Docker on MacOS](#installdockeronmacos)
  - [Build a Docker Image](#buildadockerimage)
    - [List of Docker Images](#listofdockerimages)
      - [Ubuntu 24.04](#ubuntu24.04)
  - [Running a Container](#runningacontainer)
  - [Building Andúril 2](#building-andúril-2)
  - [Cleaning up containers and images](#cleaningupcontainersandimages)
  - [FAQ](#faq)

## Installing Docker

https://www.docker.com/products/docker-desktop/

### Install Docker on Linux

```bash
sudo apt install docker.io
```
So that you don't need to `sudo docker` every command please read and follow the instructions from the official docker page:
[post-installation steps for Linux](https://docs.docker.com/engine/install/linux-postinstall/)

Update docker to prevent privilege escalation to root outside the docker container.
[user namespace remap](https://docs.docker.com/engine/security/userns-remap/)

### Install Docker on MacOS

https://www.docker.com/products/docker-desktop/

Install the .dmg file for your MacOS Apple Silicon or Intel version

```bash
brew install docker docker-buildx
mkdir ~/.docker
cat "{"cliPluginsExtraDirs": [
      "/opt/homebrew/lib/docker/cli-plugins"
  ]} >> ~/.docker/config.json
```

---

## Build a Docker Image

**Note**

The dev directory assumes it is in your home directory and migrated to /root home directory.

Build the base container

docker build -t base .
docker build -t anduril .

### List of Docker Images

The following is an overview of some of the docker images that can be built.

#### Ubuntu 24.04
For Ubuntu 24.04, the `base build`, and `andúril` container may  be consolidated into a single Docker container.
* [ubuntu-24.04-base](ubuntu/24.04/base/Dockerfile): Common build layer that includes base 24.04 image
* [ubuntu-24.04-andúril](ubuntu/24.04/andúril/Dockerfile): Includes all dependencies needed to build Andúril 2


## Running a Container

Before running a container make sure its corresponding docker image is either pulled or built first as described above

Use `docker image ls` to check the name of the images you have

For example from the top level directory you could run:

docker run --rm --env="HOME=/root" --name="anduril" -v <source_dir_to_anduril_code>:/root/anduril -it anduril


## Building Andúril 2

When you run the container as described above, you will start in the root directory.

Now to build Andúril 2, enter the following commands

```bash
./make
```
For help:

```bash
./make --help
```

Note: The compiled firmware goes into the hex/ directory, ready to be used by any firmware flashing program.

Once you are finished stop the container by entering `exit`

## Cleaning up containers and images

Docker containers and images can be pretty large, so they should be removed when no longer needed. You can recapture
some disk space by calling `docker system prune`

`docker system prune` will remove all:
 * stopped containers
 * dangling images

To remove only containers and their volumes:

```bash
docker rm -vf $(docker ps -aq)
```

If you want to remove a docker image you can use

```bash
# shows the images you have
docker image ls

# remove an image by name
docker rmi <image_name>
```

For removing all images

```bash
docker rmi -f $(docker images -aq)
