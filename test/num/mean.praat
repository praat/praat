n = 1e6
a# = zero# (7 * n)
big = 1
sequence# = { 1, 2, 3, 4, 5, 6, 7 }
accurateMean = mean (sequence#)
accurateStdev = stdev (sequence#) * sqrt (6 / 7) / sqrt (1 - 1 / 7 / n)
for power from 1 to 20
	big *= 10
	a# = repeat# (big + sequence#, n)
	appendInfoLine: power, " ", mean (a#) - big - accurateMean, " ",
	... stdev (a#) - accurateStdev
endfor
