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

* micro -- Micro-benchmarks for misc items.

* python -- Benchmarks for the python bindinggs to various AtomSpace
  subsystems.

* query-trite -- A synthetic benchmark containing a trivial pattern
  matcher query.

* query-links -- A real-world benchmark for performing a complex
  pattern match on a real-world natural-langauge dataset.
