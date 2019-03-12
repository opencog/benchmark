#! /usr/bin/env perl
#
# shell-bench.pl -- quick-n-dirty hack to measure cogserver perf.
#

# Ordinary time returns integer seconds,
# hi-res time returns floating-point (wall-clock) seconds
use Time::HiRes qw( time );

my $nrep=1;

print "Cogserver shell performance test, NREP=$nrep\n";

my $loop = 0;
my $starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '  ' | nc -q 0 localhost 17001`;
	# print "its $loop $reply\n";
	$loop += 1;
}
my $endtime = time;
my $elapsed = $endtime - $starttime;
my $rate = $elapsed / $nrep;

# print "Elapsed=$elapsed secs; shell cmds/sec=$rate\n";
printf "Elapsed=%f secs; shell cmds/sec=%f\n", $elapsed, $rate;

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

printf "Elapsed=%f secs; trivial guile cmds/sec=%f\n", $elapsed, $rate;

# ---------------------------------------------------

$loop = 0;
$starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '(+ 2 $loop)' | nc -q 0 localhost 17001`;
	# print "its $loop $reply\n";
	$loop += 1;
}
$endtime = time;
$elapsed = $endtime - $starttime;
$rate = $elapsed / $nrep;

printf "Elapsed=%f secs; non-trivial guile cmds/sec=%f\n", $elapsed, $rate;

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

printf "Elapsed=%f secs; trivial Atoms/sec=%f\n", $elapsed, $rate;

# ---------------------------------------------------

$loop = 0;
$starttime = time;
while ($loop < $nrep) {
	my $reply = `echo '(List (Number $loop) (Concept "foo"))' | nc -q 0 localhost 17001`;
	# print "its $loop $reply\n";
	$loop += 1;
}
$endtime = time;
$elapsed = $endtime - $starttime;
$rate = $elapsed / $nrep;

printf "Elapsed=%f secs; non-trivial Atoms/sec=%f\n", $elapsed, $rate;

# ---------------------------------------------------
# Avoid using netcat.

use Socket;
my $port = 17001;
my $server = "127.0.0.1";

# Don't use netcat; just send directly
# argument 1: what to send (ascii string)
# argument 2: bool flag, wait for response, or not.
sub send_stuff {
	socket(SOCKET, PF_INET, SOCK_STREAM, (getprotobyname('tcp'))[2])
		or die "Can't create a socket $!\n";

	connect(SOCKET, pack_sockaddr_in($port, inet_aton($server)))
		or die "Can't connect to port $port! \n";

	my $text = $_[0];

	# if subroutine has second argument, use it.
	my $wait_for_response = 0;
	if (1 < scalar @_) {
		$wait_for_response = $_[1];
	}
	# print "Will wait for response: $wait_for_response\n";

	if ($wait_for_response) {
		SOCKET->autoflush(1);
		print SOCKET "$text\n.\n.\n";
		my $line;
		while ($line = <SOCKET>) {
			# print "Drained: $line";
		}
		close SOCKET;
	} else {
		print SOCKET "$text\n.\n.\n";
		close SOCKET;
	}
}

# send_stuff("(+ 2 2)");
# send_stuff("(+ 3 3)");

# ---------------------------------------------------

send_stuff('(use-modules (opencog logger))', 1);
# send_stuff('(cog-logger-set-stdout! #t)');
# send_stuff('(cog-logger-set-stdout! #f)');

$nrep=1000;
# Direct socket writes
$loop = 0;
$starttime = time;
while ($loop < $nrep) {
	# send_stuff('(Concept "foo")');
	# send_stuff('(cog-logger-info "cbgb omfug")', 0);
	send_stuff('(cog-logger-info "cbgb omfug ' . $loop . '")', 0);
	$loop += 1;
}
$endtime = time;
$elapsed = $endtime - $starttime;
$rate = $elapsed / $nrep;

printf "Elapsed=%f secs; direct logger/sec=%f\n", $elapsed, $rate;

# ---------------------------------------------------

# Same as above, but wit for cogserver response.
$nrep=100;
$loop = 0;
$starttime = time;
while ($loop < $nrep) {
	# send_stuff('(Concept "foo")');
	# send_stuff('(cog-logger-info "cbgb omfug")', 1);
	send_stuff('(cog-logger-info "cbgb omfug ' . $loop . '")', 1);
	$loop += 1;
}
$endtime = time;
$elapsed = $endtime - $starttime;
$rate = $elapsed / $nrep;

printf "Elapsed=%f secs; direct logger+response/sec=%f\n", $elapsed, $rate;
