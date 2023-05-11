# Prerequisites

## The `kas` Tool

Before using the Yocto/OE build system (`bitbake`) the metadata needs to be
downloaded from different sources and the build needs to be configured. This is
where the `kas` tool maintained by Siemens comes in handy.

!!! info "Official `kas` docs"

    This guide will only cover the basic aspects of `kas` as needed. Please
    consult the [official `kas` documentation](https://kas.readthedocs.io) for
    further information.

## Choose a Build Host

Select a machine that has enough free disk space (> 50 GiB), a high amount of
memory (>= 32 GiB) and a fast internet connection.

Install an up-to-date Linux distribution of your choice.

!!! info "Yocto Supported Distros"

    If you want to do native builds (see below), make sure the distribution is
    supported by Yocto. See the
    ["System Requirements" chapter](https://docs.yoctoproject.org/dev/ref-manual/system-requirements.html#system-requirements)
    in the official Yocto manual for a list of supported distributions.

!!! info "Virtual Machines or Windows Subsystem for Linux (WSL)"

    You can use a virtual machine or WSL as your build host, but please note
    that this might impact your build performance and might bring further
    pitfalls that are not covered by this guide.

## Install Git

Install Git using your distribution's package manager. For Debian/Ubuntu the
following should do:

```
sudo apt install git
```

## Native or Containerized Builds

There are two ways to do builds using `kas` on your machine. Either natively,
which means you need to have all the dependencies used by Yocto/OE installed in
your OS. Or you can use an already available container image with all the
dependencies included.

### Setup for Containerized Builds (Recommended)

#### Install the Container Engine

We will use `podman` as a container engine. Install it using your distribution's
package manager. For Debian/Ubuntu the following should do:

```
sudo apt install podman
```

!!! info "Using `docker`"

    You can use `docker` instead of `podman`, but please note that there might
    be differences that are not covered by this guide. If you have both `docker`
    and `podman` installed `kas-container` will currently prefer `docker`. You
    can override this by setting the environment variable
    `KAS_CONTAINER_ENGINE=podman`.

#### Download the `kas-container` Script

Download the `kas-container` script from the `kas` repository and install it to
your system so it can be accessed easily.

```
wget https://raw.githubusercontent.com/siemens/kas/master/kas-container
sudo mv kas-container /usr/bin/
sudo chmod a+x /usr/bin/kas-container
```

### Setup for Native Builds

#### Install `kas`

Install `kas` using the Python package manager `pip`:

```
pip install kas
```

#### Install Yocto Dependencies

This step depends on the Yocto Release you are targeting and your host
distribution. Please follow the ["System Requirements"
chapter](https://docs.yoctoproject.org/dev/ref-manual/system-requirements.html#system-requirements)
in the official Yocto manual. Make sure that you select the correct Yocto
release when you read the manual.
