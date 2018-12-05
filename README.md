# Benchmark

Collection of tools and data to benchmark the AtomSpace, the pattern
matcher and other OpenCog processes.

## Organization

For now all benchmarks across the various OpenCog repositories will be
centralized here, with the folder names (under that root folder) equal
to names of the repositories being benchmarked. As well as likely some
common tools placed under some common folder.

## Prerequisites

### Google Benchmarks

Google Benchmark is a library supporting C++ benchmarks writing.
Use following commands to build and install it:

```
    git clone https://github.com/google/benchmark.git google-benchmark
    mkdir google-benchmark/build
    cd google-benchmark/build
    cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_GTEST_TESTS=OFF ..
    make
    sudo make install
```

## Building Benchmarks

Perform the following steps at the shell prompt:
```
    cd to project root dir
    mkdir build
    cd build
    cmake ..
    make
```
