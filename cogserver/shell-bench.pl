#! /usr/bin/env perl
#
# shell-bench.pl -- quick-n-dirty hack to measure cogserver perf.
#

my $nrep=10;

my $loop = 0;
while ($loop < $nrep) {
	print "its $loop\n";
	$loop += 1;
}
