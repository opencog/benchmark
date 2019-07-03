
Pattern Matcher benchmark HOWTO
-------------------------------
Do this:
```
$ creatdb en_nano
$ cat en_nano.sql | psql en_nano
```
The above creates a database with 43752 atoms in it.
This database consists of a matrix, of dimension 577 x 7660
with a total of 27463 non-zero matrix entries.

```
$ guile -l nano-en.scm
guile> 
```

(cog-report-counts)
Expect a total of 152904 in about 2 minutes or less.
