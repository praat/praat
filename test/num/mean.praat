writeInfoLine: "Mean..."
n = 1e5+1
n7 = 7 * n
a# = zero# (n7)
d = 0
d = 0.23456
;d = 0.000547462463
big0 = 1 + d 
sequence# = { 1, 2, 3, 4, 5, 6, 7 }
accurateMean = mean (sequence#)
accurateStdev = stdev (sequence#) * sqrt (6 / 7) / sqrt (1 - 1 / n7)
Debug: "no", 48   ; naive mean
big = big0
for power from 1 to 25
	big *= 10
	a# = repeat# (big + sequence#, n)
	mean1 = mean (a#)
	dmean1 = mean1 - big - accurateMean
	mean2 = a# [1] + mean (- a# [1] + a#)
	dmean2 = mean2 - big - accurateMean
	assert big <> round (big) or dmean2 = 0 or power > 18 - log10 (n)
	mean3 = mean (a#) + mean (- mean1 + a#)
	dmean3 = mean3 - big - accurateMean
	assert big <> round (big) or dmean3 = 0 or power > 18 - log10 (n)
	diffSquare1# = (- mean1 + a#) * (- mean1 + a#)
	meanSquare1 = mean (diffSquare1#)
	diffSquare2# = (- mean2 + a#) * (- mean2 + a#)
	meanSquare2 = mean (diffSquare2#)
	diffSquare3# = (- mean3 + a#) * (- mean3 + a#)
	meanSquare3 = mean (diffSquare3#)
	;mean2q = mean2 + mean (- mean2 + diff2#)
	stdev1 = sqrt (meanSquare1 * n7 / (n7 - 1))
	stdev2 = sqrt (meanSquare2 * n7 / (n7 - 1))
	stdev3 = sqrt (meanSquare3 * n7 / (n7 - 1))
	appendInfoLine: power, " mean: ", dmean1, " ", dmean2, " ", " ", dmean3, " ; stdev: ", stdev1 - accurateStdev, " ", stdev2 - accurateStdev, " ", stdev3 - accurateStdev
endfor

appendInfoLine: "Pairwise:"
Debug: "no", 49
big = big0
for power from 1 to 25
	big *= 10
	a# = repeat# (big + sequence#, n)
	mean = mean (a#)
	dmean = mean - big - accurateMean
	assert big <> round (big) or dmean = 0 or power > 18 - log10 (n)
	diffSquare# = (- mean + a#) * (- mean + a#)
	meanSquare = sum (diffSquare#) / n7
	stdev = sqrt (meanSquare * n7 / (n7 - 1))
	appendInfoLine: power, " ", dmean, " ", stdev (a#) - accurateStdev, " ", stdev - accurateStdev
endfor

appendInfoLine: "Kahan:"
Debug: "no", 50
big = big0
for power from 1 to 25
	big *= 10
	a# = repeat# (big + sequence#, n)
	mean = mean (a#)
	dmean = mean - big - accurateMean
	assert big <> round (big) or dmean = 0 or power > 18 - log10 (n)
	diffSquare# = (- mean + a#) * (- mean + a#)
	meanSquare = sum (diffSquare#) / n7
	stdev = sqrt (meanSquare * n7 / (n7 - 1))
	appendInfoLine: power, " ", dmean, " ", stdev (a#) - accurateStdev, " ", stdev - accurateStdev
endfor

appendInfoLine: "First-element offset:"
Debug: "no", 0
big = big0
for power from 1 to 25
	big *= 10
	a# = repeat# (big + sequence#, n)
	mean = mean (a#)
	dmean = mean - big - accurateMean
	assert big <> round (big) or dmean = 0 or power > 18 - log10 (n)
	diffSquare# = (- mean + a#) * (- mean + a#)
	meanSquare = sum (diffSquare#) / n7
	stdev = sqrt (meanSquare * n7 / (n7 - 1))
	appendInfoLine: power, " ", dmean, " ", stdev (a#) - accurateStdev, " ", stdev - accurateStdev
endfor

assert mean ({ -1e18, 3, 1e18 }) = 1
assert mean ({ -1e19, 3, 1e19 }) = 1
;assert mean ({ -1e20, 3, 1e20 }) = 1

for power from 1 to 16
	assert (mean (repeat# (10^power + sequence#, n)) - 10^power = mean (sequence#))
endfor

numberOfTrials = 100
stopwatch
for i to numberOfTrials
	b# = a#
endfor
appendInfoLine: "baseline: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"

Debug: "no", 48
stopwatch
for i to numberOfTrials
	mean: a#
endfor
appendInfoLine: "mean real64: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"
Debug: "no", 49
stopwatch
for i to numberOfTrials
	mean: a#
endfor
appendInfoLine: "mean pairwise: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"
Debug: "no", 50
stopwatch
for i to numberOfTrials
	mean: a#
endfor
appendInfoLine: "mean Kahan: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"
Debug: "no", 0
stopwatch
for i to numberOfTrials
	mean: a#
endfor
appendInfoLine: "mean real80: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"

Debug: "no", 48
stopwatch
for i to numberOfTrials
	stdev: a#
endfor
appendInfoLine: "stdev real64: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"
Debug: "no", 49
stopwatch
for i to numberOfTrials
	stdev: a#
endfor
appendInfoLine: "stdev pairwise: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"
Debug: "no", 50
stopwatch
for i to numberOfTrials
	stdev: a#
endfor
appendInfoLine: "stdev Kahan: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"
Debug: "no", 0
stopwatch
for i to numberOfTrials
	stdev: a#
endfor
appendInfoLine: "stdev real80: ", stopwatch / numberOfTrials / n7 * 1e9, " ns"

procedure do_single_peak: debug, peakLocation, zeroLocation
	Debug: "no", debug
	a# = d + repeat# ({ 1e13+1e5 }, 1e6 + 2)
	a# [peakLocation] = d + (-1e19-1e11)
	a# [zeroLocation] = d
	appendInfoLine: debug, ": ", mean (a#) - d, " ", mean (a#) + mean (- mean (a#) + a#) - d
endproc
@do_single_peak: 48, 1, 2
@do_single_peak: 48, 2, 3
@do_single_peak: 48, 1e6+2, 1e6+1
@do_single_peak: 48, 1e6+1, 1e6+2
@do_single_peak: 49, 1, 2
@do_single_peak: 49, 2, 1
@do_single_peak: 49, 2, 3
@do_single_peak: 49, 1e6+2, 1e6+1
@do_single_peak: 49, 1e6+1, 1e6+2
@do_single_peak: 50, 1, 2
@do_single_peak: 50, 2, 1
@do_single_peak: 50, 2, 3
@do_single_peak: 50, 1e6+2, 1e6+1
@do_single_peak: 50, 1e6+1, 1e6+2
@do_single_peak: 0, 1, 2
@do_single_peak: 0, 2, 1
@do_single_peak: 0, 2, 3

appendInfoLine: "OK"