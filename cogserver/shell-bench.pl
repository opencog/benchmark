#! /usr/bin/env perl
#
# shell-bench.pl -- quick-n-dirty hack to measure cogserver perf.
#

# Ordinary time returns integer seconds,
# hi-res time returns floating-point (wall-clock) seconds
use Time::HiRes qw( time );

my $nrep=10;

print "Cogserver shell performance test, NREP=$nrep\n";

my $loop = 0;
my $starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '  ' | nc -q 0 localhost 17001`;
	print "its $loop $reply\n";
	$loop += 1;
}
my $endtime = time;
my $elapsed = $endtime - $starttime;
my $rate = $elapsed / $nrep;

print "Elapsed=$elapsed secs; shell cmds/sec=$rate\n";

# ---------------------------------------------------
$loop = 0;
$starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '(+ 2 2)' | nc -q 0 localhost 17001`;
	# print "its $loop $reply\n";
	$loop += 1;
}
$endtime = time;
$elapsed = $endtime - $starttime;
$rate = $elapsed / $nrep;

print "Elapsed=$elapsed secs; guile cmds/sec=$rate\n";

# ---------------------------------------------------
$loop = 0;
$starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '(Concept "foo")' | nc -q 0 localhost 17001`;
	# print "its $loop $reply\n";
	$loop += 1;
}
$endtime = time;
$elapsed = $endtime - $starttime;
$rate = $elapsed / $nrep;

print "Elapsed=$elapsed secs; Atoms/sec=$rate\n";
