# APImageFiltersKruskal
Advanced Programming Image Filters Project

## Installation instructions

First, clone the repository:

```bash
git clone XXX
cd APImageFiltersKruskal
```

Install cmake and a C++ compiler if you don't already have them. On MacOS, you can install them with Homebrew. This should get you the `gcc v15` compilers on MacOS (March 2026):

```bash
brew install cmake
brew install gcc
```

On Windows/WSL, you can install with `apt`. This should get you the `gcc v13` compilers on Ubuntu 24.04 (March 2026)

```bash
sudo apt install build-essential cmake gdb
```

Then, create a build directory cd into it:

```bash
mkdir build
cd build
```

On MacOS, we need to define the correct `gcc` compiler:
```bash
CC=gcc-15 CXX=gcc-15 cmake ..
```

On Windows/WSL, we can just run:
```bash
cmake ..
```

Finally, from inside the build directory, compile the project:

```bash
cmake
```

## Usage instructions

The program should be called with command line options. These are explained in the [Command line options](command_line_options.md) file.
