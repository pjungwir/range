#!/usr/bin/perl -w
use strict;

my $usage = "USAGE: range [options] start end";
my @tests = (
	['', '', '', $usage],
	['-h', '', '', $usage],
	['-?', '', '', $usage],
	['', 1, 4, '1 2 3 4 '],
	['', 4, 1, '4 3 2 1 '],
	['', 1, 1, '1 '],
	['', -1, 1, '-1 0 1 '],
	['', 1, -1, '1 0 -1 '],
	['', -2, -4, '-2 -3 -4 '],
	['', -4, -2, '-4 -3 -2 '],
	['', 'a', 'c', $usage],
	['', '01', '03', '01 02 03 '],
	['', '08', '11', '08 09 10 11 '],
	['-x', 'a', 'c', 'a b c '],
	['-x', '9', 'C', '9 a b c '],
	['-x', '-a', '-c', '-a -b -c '],
	['-o', 6, 12, '6 7 10 11 12 '],
	['-o', 12, 6, '12 11 10 7 6 '],
	['-b', '110', '1010', '110 111 1000 1001 1010 '],
	['-b', '1010', '110', '1010 1001 1000 111 110 '],
	['-b', '-10', '1', '-10 -1 0 1 '],
	['-b', '0110', '1001', '0110 0111 1000 1001 '],
	['-a', 'a', 'd', 'a b c d '],
	['-a', 'A', 'D', 'a b c d '],
	['-a', 'ab', 'ad', 'ab ac ad '],
	['-a', 'x', 'ab', 'x y z aa ab '],
	['-a', 'ay', 'bb', 'ay az ba bb '],
	['-a', 'zy', 'aab', 'zy zz aaa aab '],
	['-a', '-c', 'b', '-c -b -a a b '],
	['-A', 'a', 'd', 'A B C D '],
	['-A', 'A', 'D', 'A B C D '],
	['-X', 'a', 'c', 'A B C '],
	['-X', '9', 'C', '9 A B C '],
	['-f', 4, 1, ' '],
	# TODO: forward only
);

my $correct = 0;
my $total = 0;

for my $test (@tests) {
	my $expected = $test->[3];
	$expected =~ s/ /\n/g unless $expected eq $usage;
	my $out = `./range $test->[0] $test->[1] $test->[2] 2>&1`;
	chomp $out;
	chomp $expected;
	if ($out eq $expected) {
		print "PASSED: range $test->[0] $test->[1] $test->[2]\n";
		$correct++;
		$total++;
	} else {
		print "FAILED: range $test->[0] $test->[1] $test->[2]\n";
		print "\t$out\n";
		$total++;
	}
}

print "$correct / $total tests passed.\n";
if ($correct < $total) {
	print "THERE WERE ERRORS!\n";
	exit 1;
}
exit 0;
