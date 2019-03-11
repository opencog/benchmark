#! /usr/bin/env perl
#
# shell-bench.pl -- quick-n-dirty hack to measure cogserver perf.
#

use DateTime;

my $nrep=10;

my $loop = 0;
my $starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '(+ 2 2)' | nc -q 0 localhost 17001`;
	# print "its $loop $reply\n";
	$loop += 1;
}
my $endtime = time;
my $elapsed = $endtime - $starttime;
my $rate = $elapsed / $nrep;

print "cogserver guile commands per sec=$rate\n";
