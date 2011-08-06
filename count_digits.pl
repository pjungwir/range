#!/usr/bin/perl -w
use strict;

sub digits_of {
  my ($base, $value) = @_;
  $value = to_base_10($value);
  # return (log($value) / log($base)) + 1
  my $d = 1;
  while (($value = int($value / $base)) != 0) { print "$value\n"; $d++; }
  return $d;
}

sub to_base_10 {
  my $value = shift;
  sprintf("%d", oct("0b".$value));
}

print "1001\t", digits_of(2, '1001'), "\n";
print "1000\t", digits_of(2, '1000'), "\n";
print "111\t", digits_of(2, '111'), "\n";

