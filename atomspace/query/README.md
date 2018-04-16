# Pattern matcher benchmarking #

## Overview ##

The benchmarking tool allows running different queries on specific atomspace in
the loop and measures overall execution time. Each pair of atomspace
and query forms benchmark. Atomspace and query are described using Scheme
language. All benchmarks are described using configuration file.

Tool command line options:

```
Query benchmark tool
Usage: query_benchmark [-d <working_dir>] [-c <config>] [-t <benchmark_id>]
Options:
  -d <working_dir> - working dir, default: current dir

  -c <config> - configuration file to load, default: query_benchmark.conf

    Configuration file properties:
      - guile_auto_compile=(true|false) # whether guile autocompilation should be enabled
      - print_results=(true|false) # print query results after last execution
      - benchmarks_to_run=benchmark1,benchmark2 # comma separated list of benchmarks to run
      - <benchmark>_atomspace_file=<filename.scm> # scheme file describing atomspace to load
      - <benchmark>_query_file=<filename.scm> # scheme file describing query to execute
      - <benchmark>_iterations_count=<number> # number of times to execute query

  -t <benchmark_id>,... - comma separated list of benchmarks to run,
                          default: run all benchmarks from config
```

Example of configuration file:
```
guile_auto_compile=false
print_results=true

benchmarks_to_run=animals_bindlink,animals_getlink

animals_bindlink_atomspace_file=animals.atomspace.scm
animals_bindlink_query_file=animals_bindlink.query.scm
animals_bindlink_iterations_count=100000

animals_getlink_atomspace_file=animals.atomspace.scm
animals_getlink_query_file=animals_getlink.query.scm
animals_getlink_iterations_count=100000
```

To run all benchmarks use specific make target:
```
make run_query_benchmark
```

## Profiling ##

### Using perf ###
Install:
```
apt-get install linux-tools
```

When you are in <benchmark_source_root>/build folder run:
```
QUERY_BENCHMARK="./atomspace/query/query_benchmark -d ../atomspace/query"
perf record $QUERY_BENCHMARK
perf report
```
Please see Brendan Gregg's page for for more usage examples:
http://www.brendangregg.com/perf.html

You can use flame graphs to visualize it
(https://github.com/brendangregg/FlameGraph). In example below $FLAMEGRAPH_DIR
points to FlameGraph git repository root folder.
```
perf record --call-graph dwarf,16384 $QUERY_BENCHMARK
perf script | $FLAMEGRAPH_DIR/stackcollapse-perf.pl >out
$FLAMEGRAPH_DIR/flamegraph.pl out >perf.svg
```
Open perf.svg in browser to look at resulting flame graph
```
firefox perf.svg
```

### Using valgrind ###

When you are in <benchmark_source_root>/build folder run:
```
valgrind --tool=callgrind $QUERY_BENCHMARK
```
after this completes:
```
kcachegrind callgrind.out.<pid>
```
gpof2dot and graphviz tools can be used to convert valgrind call graph to
picture (see https://github.com/jrfonseca/gprof2dot).
