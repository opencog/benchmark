# Benchmark

Collection of tools and data to benchmark the AtomSpace, the pattern
matcher and other OpenCog processes.

## Organization

For now all benchmarks across the various OpenCog repositories will be
centralized here, with the folder names (under that root folder) equal
to names of the repositories being benchmarked. As well as likely some
common tools placed under some common folder.

## Building Benchmarks

Perform the following steps at the shell prompt:
```
    cd to project root dir
    mkdir build
    cd build
    cmake ..
    make
```