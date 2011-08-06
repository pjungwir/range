#!/usr/bin/perl -w
use strict;

# This is just a quick perl script to help me get the calculations right before committing them to C code.

my @qties = qw(a b y z aa ab az ba bb yy yz za zb zz aaa zzz aaaa);

print "log(26,25) = ", (log(25)/log(26)), "\n";
print "log(26,26) = ", (log(26)/log(26)), "\n";
print "log(26,675) = ", (log(675)/log(26)), "\n";
print "log(26,676) = ", (log(676)/log(26)), "\n";
print "log(26,17575) = ", (log(17575)/log(26)), "\n";
print "log(26,17576) = ", (log(17576)/log(26)), "\n";

for my $q (@qties) {
	my $val = convert_to_10($q);
	my $digits = digits_of($val);
	my $back = convert_from_10($val);

	print "$q\t$val\t$digits\t$back\n";
}

# log(26,26)	= 1
# log(26, 676)	= 2
# log(26, 17576) = 3

#	1	a				       26*0  +1*1
#		z				       26*0  +1*26
#	2	aa				       26*1  +1*1	=27
#		zz					   26*26 +1*26	=702
#	3	aaa				676*1 +26*1	 +1*1	=703
#		zzz				676*26+26*1  +1*1	=17603		(26**3=17576)
#	4	aaaa	17576*1+676*1 +26*1	 +1*1	=

sub convert_from_10 {
	my $v = shift;
	my $n = digits_of($v);
	my $i;
	my $tmp;
	my $s = 'X' x $n;
	for ($i = $n-1; $i >= 0; $i--) {
		$tmp = $v % 26;
		if ($tmp == 0) {
			$tmp = 26;
			$v -= 26;
		}
		substr($s, $i, 1) = chr(96+$tmp);
		$v = int($v/26);
		# $v -= 26 if $tmp == 26;
		# $tmp = int($v / (26**$i));
		# print "\t$tmp\n";
		# substr($s, $n - $i - 1, 1) = chr(96+$tmp);
		# $v -= $tmp*(26**$i);
	}
	return $s;
}

sub digits_of {
	my $v = shift;
	my $s = 0;
	my $i = 0;

	while ($s < $v) {
		$s += 26 * (26 ** $i);
		$i++;
	}
	return $i;
}

sub convert_to_10 {
	my $s = shift;
	my $c;
	my $len = length $s;
	my $v;
	my $ret = 0;
	for (my $i = 0; $i < $len; $i++) {
		$c = ord(substr($s, $len-$i-1, 1));
		if ($c >= 65 and $c <= 90) {
			$v = $c - 64;
		} elsif ($c >= 97 and $c <= 122) {
			$v = $c - 96;
		} else {
			die "not an alphabetic value: $s\n";
		}
		$ret += $v * (26 ** $i);
	}
	return $ret;
}
