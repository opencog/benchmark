# Micro-Benchmarking

## Prerequisites
This requires the "Google Benchmark" micro-benchmarking tool.

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

## Running
Run benchmarks:

```
cd <BUILD_DIR>/micro
./benchmark
```

Benchmark command line parameters:

```
benchmark [--benchmark_list_tests={true|false}]
          [--benchmark_filter=<regex>]
          [--benchmark_min_time=<min_time>]
          [--benchmark_repetitions=<num_repetitions>]
          [--benchmark_report_aggregates_only={true|false}
          [--benchmark_format=<console|json|csv>]
          [--benchmark_out=<filename>]
          [--benchmark_out_format=<json|console|csv>]
          [--benchmark_color={auto|true|false}]
          [--benchmark_counters_tabular={true|false}]
          [--v=<verbosity>]
```

See the Google Benchmark documentation
https://github.com/google/benchmark/blob/master/README.md and
```DEFINE_*``` definitions in
https://github.com/google/benchmark/blob/master/src/benchmark.cc
for explanation of the command line parameters.
