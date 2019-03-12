
Cogserver shell performance notes
=================================

11 March 2019
-------------
spured by bug
   https://github.com/singnet/language-learning/issues/179

create very super-simplistic `shell-bench.pl`

Results (on `fanny`):
   $ ./shell-bench.pl

   Cogserver shell performance test, NREP=1000
   Elapsed=6.544488 secs; shell cmds; msecs-each=6.544488
   Elapsed=31.836581 secs; trivial guile cmds; msecs-each=31.836581
   Elapsed=31.883032 secs; non-trivial guile cmds; msecs-each=31.883032
   Elapsed=31.876574 secs; trivial Atoms; msecs-each=31.876574
   Elapsed=31.830150 secs; non-trivial Atoms; msecs-each=31.830150

So entering guile takes about 6x longer than entering the cogserver shell!
(Well, not really, this is a false/misleading result, it's actually netcat
weirdness; see further below.)

according to top:
When doing the shell commands, cogserver runs at about 2% of CPU.
When doing the guile commands, cogserver runs at about 10% of total cpu
meanwhile -- the perl command doing the timing runs at about 2% of CPU time.

See above github issue for wild guesses as to why.

  sudo nice -n -10 ./shell-bench.pl

does not alter the perf profile.

Changing `nc -q 0 localhost 17001` to `nc -q 0 -n 127.0.0.1 17001`
has no effect at all on performance.

Native sockets
--------------
Instead of using netcat, just open a socket (in perl) write to it,
and close. What's the performance then? It depends. If its "open a
socket, write message, wait for full reply, close" then its:

   Elapsed=30.523183 secs; direct logger+response; msecs-each=101.743943

If its "open a socket, write message, don't wait, close", then its

   Elapsed=18.459016 secs; direct logger; msecs-each=6.153005

In the former, cogserver CPU-usage runs at 2% to 4%. In the latter,
cpu usage runs 200% to 300%.

So, the CPU usage is about 80x greater for don't-wait-for-reply case.
The throughput is about 16x faster (elapsed time) so I guess there's
a lot of CPU waste with multi-threading. Each command is super-short,
so, of course the cogserver will waste a lot of time creating and
dstroying threads, entering and exiting guile in each thread.

According to `htop`, the cogserver is running 30-80 threads when its
running in the close-don't-wait mode.

   htop -- S for setup, columns, NLWP column for threads.

Latest run 11 March 2019
------------------------
After cleanup of benchmark script. Pretty-print, etc.

   $ ./shell-bench.pl
   Cogserver shell performance measurements - Tue Mar 12 00:25:54 2019
   nreps=   500 time= 3.141 secs; netcat trivial shell;    call= 6.281 msecs
   nreps=   100 time=10.027 secs; netcat shell help cmd;   call=100.266 msecs
   nreps=   100 time= 3.111 secs; netcat trivial scheme;   call=31.113 msecs
   nreps=   100 time= 3.148 secs; netcat non-trivial scm;  call=31.480 msecs
   nreps=   100 time= 3.238 secs; netcat create same atom; call=32.383 msecs
   nreps=   100 time= 3.200 secs; netcat various atoms;    call=31.995 msecs
   nreps=   500 time= 3.069 secs; no-wait socket atoms ;   call= 6.139 msecs
   nreps=   500 time= 3.086 secs; no-wait socket logger;   call= 6.173 msecs
   nreps=    50 time= 5.005 secs; socket-wait response ;   call=100.102 msecs
