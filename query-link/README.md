
Pattern Matcher benchmark HOWTO
-------------------------------
This is a "full-stack" pattern-matcher benchmark. It uses an AtomSpace
of a non-trivial size (43752 Atoms total) with a non-trivial structure
(27463 Sections which consist of word-disjunct pairs; there are 576
words and 6982 disjuncts.  That is, this is a matrix of dimension
576 x 6982 of which only 27463 matrix entries are non-zero.)

This matrix is loaded up (from an SQL database) outside of the timing
loop. The timing loop measure the performance of computing 152904 links
between 576 word-pairs, using a non-trivial search pattern. The search
pattern has one variable and four GlobNodes in it; the Globs mean that
this whole thing requires significant pattern-matching effort.  To
avoid polluting the AtomSpace with search results, this also uses
temporary AtomSpaces for the search. Linux `perf_events` confirms that
90% of all CPU time is spent in the pattern-matcher.

This is a "real world" benchmark, in that it uses a real-world dataset,
containing actual data from actual NLP experiments.  It's called "nano"
only because its many thousands of times smaller than the real datasets
it was derived from.  As a "real-world" benchmark, it should represent
"real-world" results that users can expect to get.

A diary of results can be found in `diary.txt`.

==Running the benchmark

Do this (once, the first time only):
```
$ createdb en_nano
$ cat en_nano.sql | psql en_nano
```
The above creates a database with 43752 atoms in it.
This database consists of a matrix, of dimension 576 x 7660
with a total of 27463 non-zero matrix entries.

Then, to run each test:
```
$ guile -l nano-en.scm
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
