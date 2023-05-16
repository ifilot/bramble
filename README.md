# Bramble

## Purpose

Atomic pattern recognition utility based on the CNA algorithm. Bramble is the refactored version of [Barnacle](https://gitlab.tue.nl/inorganic-materials-chemistry/barnacle).

## Compilation

Ensure you have the necessary dependencies installed on your system.

```bash
sudo apt install -y build-essential cmake libboost-all-dev pkg-config libeigen3-dev libtclap-dev
```

```bash
mkdir build && cd build
cmake ../src
make -j
```

## Usage

```bash
./bramble -i <geometry_file> -p <pattern_library> -o <output_file>
```
