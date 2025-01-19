# AtomSpace Base-Function Benchmarking

This tool was developed by Joel Pitt, in order to assess the impact
of design changes to the AtomSpace API.  The goal is to report the
performance of specific AtomSpace operations, such as the creation
and deletion of Nodes and Links, as well as the setting and getting
of TruthValues, as well as other operations (fetch of Incoming Set,
etc.)

This tool measures C++, guile and python performance.

A simple example:

```bash
$ cd <BUILD_DIR>/atomspace
$ ./atomspace_bm -m "addLink" -k -S 100 -n 10000 -f
```

This will run the addLink benchmark (`-m "addLink"`). It will also build
a test AtomSpace before running the benchmark which consists of random
graph whose size and link density can be changed on the command line.

It will run and measure the speed of the addLink method 10,000 times
(`-n 10000`), but between each it will add 100 more random atoms to the
AtomSpace (`-S 100`) so that you can assess how performance scales with
AtomSpace size. `-k` calculates some statistics on the run times (min,
max, mean, etc) of the specified method, and `-f` dumps the records to a
file called addLink_benchmark.csv (the filename used is always the
method name appended by `_benchmark.csv`).

You should see output like:

```
OpenCog Atomspace Benchmark - Version 1.0.1

Random generator: MT19937
Random seed: 1454704931

Ignore this: 32619
Benchmarking AtomSpace's addLink method 10000 times .....
0.226272 seconds elapsed (110486.44 per second)
Sum clock() time for all requests: 184614 (0.184614 seconds, 135418 requests per second)
Per operation stats, in CPU clock ticks:
  N: 5
  mean: 36922
  min: 34017
  max: 42678
  std: 3570
------------------------------
```

Use the `-l` option to list methods that are implemented for testing,
or `-A` to run ALL methods sequentially. You can also specify multiple
methods by using `-m` repeatedly.

The option `-?` will print more detail.

## A note about memory measurement ##

We just measure changes in the max RSS (resident stack size). This
means that when memory is freed the benchmarker doesn't detect it.
For this reason, if you want (semi)meaningful memory measurements,
you should:

- only benchmark a single method at a time
- not enable statistic calculation (`-k`), as this will be storing all
  the time records (you can always output to file with `-f` and do stats
  calculations later)

## Graphs ##

There is a script `atomspace/make_benchmark_graphs.py` which will
create graphs from the csv files. You must have matplotlib (Python graphing
library) version 2.0 or greater installed for this to work. If you run the
script from a directory with files ending in `_benchmark.csv` in it, it will
create a `.png` file for each.

## TODO ##

The memory estimation should be made more informative and reliable. One way to
do this may be to use dmalloc or wrap the process in valgrind and create
a script to process memory calls. Python also has the subprocess module which
could spawn instances of the benchmarker for each method (since the current
memory measurement via max RSS isn't useful when running multiple methods).

A better way might be to record the number of mallocs:
http://www.gnu.org/s/libc/manual/html_node/Hooks-for-Malloc.html
