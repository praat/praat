a# = repeat# ({5},100)
b# = a#
stopwatch
for i to 1000000
	b# += a#
endfor
writeInfoLine: stopwatch, " ", b# [3]

a# = repeat# ({5},100)
for i to 5
	a# += a#
endfor
a# [3] = 160

a# = { 1, 3, 5 }
a# += 10
assert a# = { 11, 13, 15 }
