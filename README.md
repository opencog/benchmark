# Benchmark

Collection of tools and data to benchmark the AtomSpace, the pattern
matcher and other OpenCog processes.

## Organization
Each distinct benchmark has it's own instructions for building and
running it.  See individual directories for details.

* atomspace -- A synthetic benchmark for measuring core AtomSpace
  functions, such as rates for creating Nodes and Links, rates for
  deleting them, and rates for setting and getting TruthValues and
  IncomingSets.

* query-trite -- A synthetic benchmark containing a trivial pattern
  matcher query.

* query-links -- A real-world benchmark for performing a complex
  pattern match on a real-world natural-langauge dataset.



## Prerequisites
Some, but not all of the benchmarks require the "Google Benchmarks"
micro-benchmarking tool.

### Google Benchmarks

Google Benchmark is a library supporting C++ micro-benchmarking.
Build and install it:

```
    git clone https://github.com/google/benchmark.git google-benchmark
    mkdir google-benchmark/build
    cd google-benchmark/build
    cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_GTEST_TESTS=OFF ..
    make
    sudo make install
```
