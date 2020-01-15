
Pattern Matcher benchmark HOWTO
-------------------------------
This is a "full-stack" pattern-matcher benchmark. It uses an AtomSpace
of a non-trivial size, of almost 2 million (1967867) Atoms in total.
The data itself has a very simple structure: it consists entirely of
vertexes and labelled edges. Each vertex represents either a gene,
or a formal database name for it (the Entrez ID).  There are two edge
labels (two types of edges), either `(Predicate "interacts_with")` or
`(Predicate "has_entrez_id")`. A third predicate `(Predicate
"has_biomed_ID")` is used to label edges.

Edges are represented with `EvaluationLinks` as usual. For example:
```
	EvaluationLink
		PredicateNode "interacts_with"
		ListLink
			GeneNode "GCN1"
			GeneNode "RAD23A"
```

The specific pattern search is for triangular loops, of the form
```
   (Evaluation (Predicate "interacts") (List gene (Variable "$a")))
   (Evaluation (Predicate "interacts") (List (Variable "$a") (Variable "$b"))
   (Evaluation (Predicate "interacts") (List gene (Variable "$b")))
```
where `gene` is a given gene, specified by the scientist, and `$a` and
`$b` are two other genes that interact with it.  Given the above
contraints, these interaction make a loop or triangle.

The dataset is further charcachterized
[in the dataset-notes directory.](./dataset-notes)

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
The above should take about 2 minutes (or less) to load the various
datasets, providing a print after each.  It will then run two different
benchmarks three times each, printing something similar to this:

```
Triangle relations for 681 genes in 187.32 seconds
Protein expression for 12 pathways in 120.01 seconds
```
The third benchmark is an expanded version of the second one; it takes
hours to run.

This test shows a large benefit from using Linux Huge Pages.
Not a surprise: it's accessing a lot of memory in a very random
kind of way.  Try it:
```
HUGETLB_MORECORE=yes LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libhugetlbfs.so.0 guile -l bio-loop.scm
```
(Of course, be sure to correctly configure hugepages first).
