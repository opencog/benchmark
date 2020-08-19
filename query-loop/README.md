
Pattern Matcher (Genomic) Benchmark HOWTO
-----------------------------------------
This is a "full-stack" pattern-matcher benchmark on a "real-world"
genome/proteome/reactome dataset.

It uses `EvaluationLink`'s to represent the edges and vertexes of
a graph, and then searches that graph for triangles and pentagons.
The (labeled-) vertex and (labeled-) edge representation is
straight-forward, just a classic use of `EvaluationLink`. The
graph contains 250K vertexes and 1.2M edges, with most of the
edges representing gene-gene or gene-protein interactions, although
some of them are used to attach tags for PubMed/Entrez/UniProt id's.

Edges are represented with `EvaluationLinks` as usual. For example:
```
	EvaluationLink
		PredicateNode "interacts_with"
		ListLink
			GeneNode "GCN1"
			GeneNode "RAD23A"
```

One of the two benchmarks searches for triangular loops in the
graph, specifically, those of the form
```
   (Evaluation (Predicate "interacts") (List gene (Variable "$a")))
   (Evaluation (Predicate "interacts") (List (Variable "$a") (Variable "$b"))
   (Evaluation (Predicate "interacts") (List gene (Variable "$b")))
```
where `gene` is a given gene, specified by the scientist, and `$a` and
`$b` are two other genes that interact with it.

The other benchmark looks for pentagons, and so is similar, except that
one vertex is a reactome pathway label, two edges connect that pathway
to proteins that participate in that pathway, two edges are gene-protein
expression relations, and the fifth edge indicates that the two genes
interact with each-other. (Thus, one pathway vertex, two protein vertexes,
and two gene vertexes).

The structure of the dataset is explored
[in the dataset-notes directory.](./dataset-notes)

A historical diary of performance measurement results can be found in
`diary.txt`.

## Running the benchmark

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

## Benchmark variants

There are several variants here, for exploration.
* `bio-loop2.scm` -- same as `bio-loop.scm` above, except it uses
  `MeetLink`s instead of `GetLink`s, thus avoiding the creation of
  SetLink, and also it's deletion.  That is, `MeetLink`s return
  their results in a `QueueValue` instead of a `SetLink`.
  At this time, there is no particularly noticeable difference in
  performance as a result of this change. Almost all CPU time is spent
  in the pattern matcher.

* `bio-loop3.scm` -- same as `bio-loop.scm` above, except it runs
  a threaded version of the query, in varying numbers of threads.
  Useful for quantifying the parallelizability of the pattern matcher.
  See https://github.com/opencog/atomspace/issues/2758 for current
  problem with parallelizability.
