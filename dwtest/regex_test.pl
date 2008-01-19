#!/usr/bin/perl -w

$s = "ababccccdd";

$s1 = $s;
$s1 =~ s/^ab/x/g;
print "\"",$s1,"\"\n";

$s1 = $s;
$s1 =~ s/ab/x/g;
print "\"",$s1,"\"\n";

$s1 = $s;
$s1 =~ s/d$/x/g;
print "\"",$s1,"\"\n";

$s1 = $s;
$s1 =~ s/d\$/x/g;
print "\"",$s1,"\"\n";

$s1 = "";
$s1 =~ s/^/x/g;
print "\"",$s1,"\"\n";
