# Benchmark

Collection of tools and data to benchmark the AtomSpace, the pattern
matcher and other OpenCog processes.

## Organization
Each distinct benchmark has it's own instructions for building and
running it.  See individual directories for details.

The most interesting benchmarks are probably the **query-links**
benchmark (because it is a real-world benchmark) and the **atomspace**
benchmark (because it has a very long measurement history).

* __atomspace__ -- A synthetic benchmark for measuring core AtomSpace
  functions, such as rates for creating Nodes and Links, rates for
  deleting them, and rates for setting and getting TruthValues and
  IncomingSets.

* __micro__ -- Micro-benchmarks for misc items.

* __python__ -- Benchmarks for the python bindings to various AtomSpace
  subsystems.

* __query-trite__ -- A synthetic benchmark containing some trivial
  pattern matcher queries.

* __query-links__ -- A real-world benchmark for performing a complex
  pattern match on a real-world natural-language dataset.

* __query-loop__ -- A real-world benchmark for performing a complex
  pattern match on a real-world genome dataset.

* __cogserver__ -- Benchmark of network I/O to the cogserver.
  Highlights issues created by using `netcat` instead of using native
  socket I/O.
