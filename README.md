# Benchmark

Collection of tools and data to benchmark the AtomSpace, the pattern
matcher and other OpenCog processes.

## Organization
Each distinct benchmark has it's own instructions for building and
running it.  See individual directories for details.

The most interesting benchmarks are probably the **query-links** and
**query-loop** benchmarks (because they are "real-world" benchmarks)
and the **atomspace** benchmark (because it has a very long measurement
history, despite being deeply flawed).

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
  pattern match on a real-world natural-language dataset. Creates 576
  BindLink's with four GlobNodes and a Variable, each. The Globs are
  used to search for connectable connectors, similar to parsing tasks.
  That is, a search over "jigsaw puzzle pieces", looking for pairs
  of jigsaw pieces that can connect.

* __query-loop__ -- A real-world benchmark for performing a complex
  pattern match on a real-world genome dataset. Uses EvaluationLink
  in a simple/straight-forward way to represent the edges and vertexes
  of a large graph (for gene/protein interactions), and then hunts
  for triangles and pentagons in that graph.

* __cogserver__ -- Benchmark of network I/O to the cogserver.
  Highlights issues created by using `netcat` instead of using native
  socket I/O.
