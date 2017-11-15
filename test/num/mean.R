n = 1e5+1
n7 = 7 * n
d = 0
d = 0.234567
#d = 0.000547462463
big0 = 1 + d
sequenceA = seq (1, 7)
meanA = mean (sequenceA)
stdevA = sd (sequenceA) * sqrt (6 / 7) / sqrt (1 - 1 / 7 / n)
sequenceB = seq (1, n)
meanB = mean (sequenceB)
stdevB = sd (sequenceB)
big = big0
for (power in seq (1, 25)) {
	big = big * 10
	a = rep (big + sequenceA, n)
	b = big + sequenceB
	cat (power, mean (a) - big - meanA, sd (a) - stdevA, mean (b) - big - meanB, sd (b) - stdevB, '\n')
}

numberOfTrials = 100
t = system.time (for (i in 1:numberOfTrials) mean (a))
cat ('mean:', t[1] / numberOfTrials / n * 1e9 / 7, 'ns per element\n')
t = system.time (for (i in 1:numberOfTrials) sd (a))
cat ('stdev:', t[1] / numberOfTrials / n * 1e9 / 7, 'ns per element\n')

a = rnorm (7 * n, 0, 1)
t = system.time (for (i in 1:numberOfTrials) mean (a))
cat ('mean:', t[1] / numberOfTrials / n * 1e9 / 7, 'ns per element\n')
t = system.time (for (i in 1:numberOfTrials) sd (a))
cat ('stdev:', t[1] / numberOfTrials / n * 1e9 / 7, 'ns per element\n')

#d = 1.23456
do_single_peak = function (peakLocation, zeroLocation) {
	a = d + rep (1e13+1e5, 1e6 + 2)
	a [peakLocation] = d + (-1e19-1e11)
	a [zeroLocation] = d
	cat (mean (a) - d, mean (a) + mean (- mean (a) + a) - d, '\n')
}
do_single_peak (1, 2)
do_single_peak (2, 1)
do_single_peak (2, 3)
do_single_peak (1e6+1, 1e6+2)
do_single_peak (1e6+2, 1e6+1)
