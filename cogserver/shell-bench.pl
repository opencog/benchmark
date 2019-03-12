#! /usr/bin/env perl
#
# shell-bench.pl -- quick-n-dirty hack to measure cogserver perf.
#

# Ordinary time returns integer seconds,
# hi-res time returns floating-point (wall-clock) seconds
use Time::HiRes qw( time );

my $nrep=10;

my $now = localtime();
print "Cogserver shell performance measurements - $now\n";

# ---------------------------------------------------

sub netcat_timer {
	my $nrep = $_[0];
	my $label = $_[1];
	my $msg = $_[2];

	my $loop = 0;
	my $starttime = time;
	while ($loop < $nrep) {
		my $full_msg = sprintf($msg, $loop);
		my $reply = `echo '$full_msg' | nc -q 0 localhost 17001`;
		# print "loopcnt=$loop full-msg=$full_msg reply=$reply\n";
		$loop += 1;
	}
	my $endtime = time;
	my $elapsed = $endtime - $starttime;
	my $per_call = 1000.0 * $elapsed / $nrep;

	printf "nreps=%6d time=%6.3f secs; " . $label . ";\tcall=%6.3f msecs\n",
		$nrep, $elapsed, $per_call;

	# Sleep; allow cogserver to recover!?
	sleep 2;
}

# ---------------------------------------------------

netcat_timer(500, "netcat trivial shell", " ");
netcat_timer(100, "netcat shell help cmd", "help");
netcat_timer(100, "netcat trivial scheme", "(+ 2 2)");
netcat_timer(100, "netcat non-trivial scm", "(+ 2 %d)");
netcat_timer(100, "netcat create same atom", "(Concept \"foo\")");
netcat_timer(100, "netcat various atoms", "(List (Number %d) (Concept \"foo\"))");

# ---------------------------------------------------
# Avoid using netcat.

use Socket;
my $port = 17001;
my $server = "127.0.0.1";

# Don't use netcat; just send directly.
# Argument 1: what to send (ascii string)
# Argument 2: bool flag, either wait for response, or not.
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
	# print "Will send message: $text\n";
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

# ---------------------------------------------------

sub direct_timer {
	my $nrep = $_[0];
	my $label = $_[1];
	my $wait = $_[2];
	my $msg = $_[3];

	my $loop = 0;
	my $starttime = time;
	while ($loop < $nrep) {
		my $full_msg = sprintf($msg, $loop);
		send_stuff($full_msg, $wait);
		$loop += 1;
	}
	my $endtime = time;
	my $elapsed = $endtime - $starttime;
	my $per_call = 1000.0 * $elapsed / $nrep;

	printf "nreps=%6d time=%6.3f secs; " . $label . ";\tcall=%6.3f msecs\n",
		$nrep, $elapsed, $per_call;

	# Sleep; allow cogserver to recover!?
	sleep 2;
}

# ---------------------------------------------------

direct_timer(500, "no-wait socket atoms ", 0, "(Concept \"foo %d\")");
direct_timer(500, "no-wait socket logger", 0, "(cog-logger-info \"cbgb omfug %d\")");
direct_timer(50, "socket-wait response ", 1, "(cog-logger-info \"cbgb omfug %d\")");

# ---------------------------------------------------
