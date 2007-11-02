#!/usr/bin/perl -w

$s = "ababccccd";

$s =~ s/^ab/x/g;
print $s,"\n";
$s =~ s/ab/x/g;
print $s,"\n";
