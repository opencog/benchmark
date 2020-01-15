
Pattern Matcher benchmark HOWTO
-------------------------------
This is a "full-stack" pattern-matcher benchmark, using a "real-world"
natural language dataset to perform a "typical" search used in natural
language machine learning.

The dataset consists of 27K Sections (precisely, 27463 Sections which
consist of word-disjunct pairs; there are 576 words and 6982 disjuncts
That is, this is a matrix of dimension 576 x 6982 of which only 27463
matrix entries are non-zero.)

Each Section can be (should be) thought of as a distinct "jigsaw puzzle
piece". Each disjunct can be (should be) thought of as a collection of
one or more tabs/connectors on that jigsaw-puzzle piece. The search to
be performed (to be benchmarked) is to locate pairs of connectable
puzzle pieces (pairs of connectable words), and then to increment counts
whenever a connection (a word-pair) is possible. The search makes very
heavy use of GlobNodes to identify the connector-sets to the left and
right of the connecting-connectors.

The benchmark creates 576 BindLinks, one for each word. Each BindLink
has one Variable (its the word that is being connected-to) and four
GlobNodes. Two GlobNodes specify the connector-sets to the left/right
of the base word, and the other two GlobNodes specify the left/right
connector sets of the other word.

In ASCII-art:
```
         \       /
        --*-----*--
         /       \
```
with the `*` being a word, the long `-----` being the connector-pair
joining the two words, and the diagonal slashes and dashes being the
other unconnected-connectors left dangling.  So this is an edge-mining
task. Easy, in principle; hard only because of the need to partition
the unconnected connectors (aka "half-edges") into sets.

The AtomSpace contains 43752 Atoms in total.

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
