a# = repeat# ({42},42)
stopwatch
for i to 10^6
	b# = a#
endfor
writeInfoLine: stopwatch
stopwatch
for i to 10^6
	b# = a# + 0
endfor
appendInfoLine: stopwatch