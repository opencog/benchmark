## Python Benchmark

This tool was developed by Cosmo Harrigan and updated by Curtis Faith to
test the performance of the Python bindings and to compare new bindings
against the Scheme scheme_eval and scheme_eval_h which were previously the
only way to access certain OpenCog functionality which has been replaced by
new Cython bindings.

Run:

```bash
$ python3 ./benchmark.py
```

The test will run and produce its output automatically if no options are
specified. The options are available with `-h` or `--help`.

```bash
$ python3 benchmark.py -h
usage: benchmark.py [-h] [-v | -c]
                    [-a | -t {spread,node,bindlink,traverse,scheme}] [-i N]

OpenCog Python Benchmark

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose         verbose output
  -c, --columns         columnar output (default)
  -a, --all             run all tests
  -t {spread,node,bindlink,traverse,scheme}, --test {spread,node,bindlink,traverse,scheme}
                        Test to benchmark, where:
                          spread    - a spread of tests across areas (default)
                          node      - atomspace node operations
                          bindlink  - all the bindlink forms
                          traverse  - traversal of atomspace lists
                          scheme    - scheme evaluation functions
  -i N, --iterations N  iterations to average (default=10)
```

A diary of results can be found in `python_diary.txt`.
