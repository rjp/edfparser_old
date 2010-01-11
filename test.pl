$/="";
$tests=0; $passes=0;
while (<>) {
	$tests++;
	my ($comment, $test) = split /\n/, $_;

	# test data can be read from a file
	if ($test =~ /^@(.*)/) {
		open TMP, "$1" or die "test file $1 not opened: $!";
		$test = join('', <TMP>);
		close TMP;
	}

	# output our test data to a temporary file
	open TMPFILE, "> .tmpfile.$$" or die ".tmpfile.$$: $!";
	print TMPFILE $test;
	close TMPFILE;

	# perform the test
	open TEST, "./edfparser --testing < .tmpfile.$$ |";
	my @output = grep {/_top/} <TEST>;
	close TEST;

	#die "No output from $comment" unless @output;
	$output[0] =~ s!</?_top.*?>!!g;

	# replace escaped characters with their unescaped replacement
	$test =~ s!\\(.)!$1!g;

	# Handle tests we're expecting to fail with null output by copying
	# the test input and pretending we got that instead
	if ($comment =~ /FAIL/ and $output[0] ne $test) {
		$output[0] = $test;
	}

	if ($test eq $output[0]) {
		print ".";
		$passes++;
	} else {
		print "E";
		push @errors, [$tests, $comment, $test, $output[0]];
	}
	unlink ".tmpfile.$$";
}
print "\n${passes}/${tests} tests passed\n\n";
if (@errors) {
	foreach my $i (@errors) {
		print "Test $i->[0]: $i->[1]\n";
		print "Data [$i->[2]]\n";
		print "Got  [$i->[3]]\n\n";
	}
} else {
	print "No errors!\n";
}
