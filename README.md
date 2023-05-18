# Bramble

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/bramble?label=version)
[![Build](https://github.com/ifilot/bramble/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/bramble/actions/workflows/build.yml)
[![Documentation](https://github.com/ifilot/bramble/actions/workflows/docs.yml/badge.svg)](https://bramble.imc-tue.nl)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Purpose

Atomic pattern recognition utility based on the CNA algorithm. For detailed explanation
of Bramble, [see the documentation](https://bramble.imc-tue.nl).

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

## Usage

```bash
./bramble -i <geometry_file> -p <pattern_library> -o <output_file>
```
