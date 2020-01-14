
Pattern Matcher benchmark HOWTO
-------------------------------
This is a "full-stack" pattern-matcher benchmark. It uses an AtomSpace
of a non-trivial size (XXX Atoms total) having a very simple structure:
it consists entirely of vertexes and labelled edges. Each vertex
represents either a gene, or a formal database name for it (the
Entrez ID).  There are two edge labels (two types of edges), either
`(Predicate "interacts_with")` or `(Predicate "has_entrez_id")`.

Edges are represented with `EvaluationLinks` as usual. For example:
```
	EvaluationLink
		PredicateNode "interacts_with"
		ListLink
			GeneNode "GCN1"
			GeneNode "RAD23A"
```

This is a "real world" benchmark, in that it uses a real-world dataset,
containing actual data from genomics datasets.  As a "real-world"
benchmark, it should represent "real-world" results that users can
expect to get.

A diary of results can be found in `diary.txt`.

==Running the benchmark

To run each test:
```
$ guile -l bio-loop.scm
```
The above should take about 2 minutes (or less) to run, and, when done,
should print something similar to this:

```
Elapsed: 124 secs Tot-cnt=152904 Avg=0.2153 secs/word Rate=1233.1 cnts/sec
```

The Tot-cnt should always be the same.

This test shows a large benefit from using Linux Huge Pages.
Not a surprise: it's accessing a lot of memory in a very random
kind of way.  Try it:
```
HUGETLB_MORECORE=yes LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libhugetlbfs.so.0 guile -l nano-en.scm
```
