s1 = Create Sound from formula: "s1", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
s2 = Copy: "s2"
plusObject: s1
asserterror Multiple objects selected. Cannot assign object IDs to numeric variable. Perhaps use a vector variable instead.
a = To Spectrum: "yes"
removeObject: "Spectrum s1", "Spectrum s2"
selectObject: s1, s2
a# = To Spectrum: "yes"
assert a# = { s1, s2 } + 4
writeInfoLine: a#
plusObject: s1, s2
Remove