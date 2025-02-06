# Bramble

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/bramble?label=version)
[![Build](https://github.com/ifilot/bramble/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/bramble/actions/workflows/build.yml)
[![codecov](https://codecov.io/gh/ifilot/bramble/graph/badge.svg?token=25GK2ME5ZV)](https://codecov.io/gh/ifilot/bramble)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![status](https://joss.theoj.org/papers/f6493d619d92bb6d993713b9d1abb38c/status.svg)](https://joss.theoj.org/papers/f6493d619d92bb6d993713b9d1abb38c)
[![DOI](https://zenodo.org/badge/641289155.svg)](https://zenodo.org/badge/latestdoi/641289155)
[![Docker pulls](https://img.shields.io/docker/pulls/ivofilot/bramble)](https://hub.docker.com/r/ivofilot/bramble)

## Purpose

> [!TIP]
> Detailed documentation can be found [here](https://ifilot.github.io/bramble/).

Bramble is a single atom pattern recognition algorithm based on the Common
Neighbor Analysis method. It can efficiently construct CNA fingerprints per
atom and connect these fingerprints to a (customizable) pattern library to add
labels to the fingerprints.

![Nanoparticle](./docs/_static/img/nanoparticle_identified_atoms.png)

For fingerprints that are (still) unknown or for atoms that have a more
amorphous chemical environment, Bramble comes bundled with a similarity
analysis tool. Although relatively computationally expensive to execute, it
yields a powerful similarity metric by which the extent that two chemical
environments are the same can be probed.

![Nanoparticle](./docs/_static/img/similarity_analysis_co1121.png)

## Compilation

Ensure you have the necessary dependencies installed on your system.

```bash
sudo apt install -y build-essential cmake libboost-all-dev pkg-config libeigen3-dev libtclap-dev
```

Next, clone the repository and combile Bramble using

```bash
mkdir build && cd build
cmake ../src
make -j
```

To test that Bramble is working, run the test suite

```bash
make test
```

For debugging purposes, one can run

```bash
CTEST_OUTPUT_ON_FAILURE=TRUE make test
```

### CUDA support

To compile `bramble` with CUDA support, run

```bash
cmake ../src -DMOD_CUDA=1 -DCUDA_ARCH=sm_89
```

where the value for `DCUDA_ARCH` should match the architecture of your graphical
card. A nice overview is given [here](https://arnon.dk/matching-sm-architectures-arch-and-gencode-for-various-nvidia-cards/).
For example, for a `RTX 4090`,  `-DCUDA_ARCH=sm_89`.

## Docker container

Rather than compiling Bramble yourself, you can also make direct use of the
[Bramble docker container](https://hub.docker.com/r/ivofilot/bramble). This
container is built on top of the [NVidia CUDA container](https://hub.docker.com/r/nvidia/cuda/)
allowing you to use the CUDA-enabled version of Bramble.

To use the Docker container, first download it from Docker Hub

```bash
docker pull ivofilot/bramble
```

Next, launch the container and link a volume on your hard drive to interact
with. This folder should contain the files you wish to work with.

```bash
docker run --name bramble -v "D:/bramble-data":/home/bramble/data -d ivofilot/bramble:latest
```

This will launch the `bramble` image, which you check using

```bash
docker ps
```

To execute Bramble, simply log into the container and use the same kind of commands
you would normally use for running Bramble.

```bash
docker exec -it bramble /bin/bash
```

Go to the `/home/bramble/data` folder and make sure you have copied 
`patterns.json` and `pa_fcc111.txt` to this folder. 
Since the `bramble` executable is available from the path, you
can directly invoke it. To perform a pattern identification, run

```bash
cd /home/bramble/data
bramble -p patterns.json -i POSCAR_Rh111 -o pa_fcc111.txt
```