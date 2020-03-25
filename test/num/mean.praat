writeInfoLine: "Mean..."

durations# = zero# (100)
for n from 1 to 100
	result$ = Praat test: "TimeMean", string$ (10^8 / n), string$ (n), "", ""
	durations# [n] = extractNumber (result$, newline$)
	appendInfoLine (n, " ", durations# [n])
endfor

appendInfoLine: "mean ", mean (durations#), " nanoseconds"

n = 1e5+1
n7 = 7 * n
d = 0
d = 0.234567
;d = 0.000547462463
big0 = 1 + d 
sequenceA# = { 1, 2, 3, 4, 5, 6, 7 }
meanA = mean (sequenceA#)
stdevA = stdev (sequenceA#) * sqrt (6 / 7) / sqrt (1 - 1 / n7)
sequenceB# = linear# (1, n, n, 0)
meanB = mean (sequenceB#)
stdevB = stdev (sequenceB#)
sequenceC# = { 0 }
meanC = 0.0
stdevC = 0.0
Debug: "no", 48   ; naive mean
big = big0
for power from 1 to 25
	big *= 10
	a# = repeat# (big + sequenceA#, n)
	mean1 = mean (a#)
	dmean1 = mean1 - big - meanA
	mean2 = a# [1] + mean (- a# [1] + a#)
	dmean2 = mean2 - big - meanA
	assert big <> round (big) or dmean2 = 0 or power > 18 - log10 (n)
	mean3 = mean (a#) + mean (- mean1 + a#)
	dmean3 = mean3 - big - meanA
	assert big <> round (big) or dmean3 = 0 or power > 18 - log10 (n)
	diffSquare1# = (a# - mean1) * (a# - mean1)
	meanSquare1 = mean (diffSquare1#)
	diffSquare2# = (a# - mean2) * (a# - mean2)
	meanSquare2 = mean (diffSquare2#)
	diffSquare3# = (a# - mean3) * (a# - mean3)
	meanSquare3 = mean (diffSquare3#)
	;mean2q = mean2 + mean (- mean2 + diff2#)
	stdev1 = sqrt (meanSquare1 * n7 / (n7 - 1))
	stdev2 = sqrt (meanSquare2 * n7 / (n7 - 1))
	stdev3 = sqrt (meanSquare3 * n7 / (n7 - 1))
	appendInfoLine: power, " mean: ", dmean1, " ", dmean2, " ", dmean3, " ; stdev: ", stdev1 - stdevA, " ", stdev2 - stdevA, " ", stdev3 - stdevA
endfor
Debug: "no", 0

debug# = { 0 }
debug$ [1] = "Pairwise base case 64"

appendInfoLine: newline$, "OFFSET"
for idebug from 1 to size (debug#)
	appendInfoLine: newline$, debug$ [idebug]
	Debug: "no", debug# [idebug]
	big = big0
	for power from 1 to 25
		big *= 10
		a# = repeat# (big + sequenceA#, n)
		b# = big + sequenceB#
		c# = repeat# (big + sequenceC#, n7)
		appendInfoLine: "Power ", power, ". Sawtooth: mean ", mean (a#) - big - meanA, ", stdev ", stdev (a#) - stdevA,
		... ". Line: mean ", mean (b#) - big - meanB, ", stdev ", stdev (b#) - stdevB,
		... ". Constant: mean ", mean (c#) - big - meanC, ", stdev ", stdev (c#) - stdevC
	endfor
	Debug: "no", 0
endfor

if mean ({ -1e18, 3, 1e18 }) <> 1
	appendInfoLine: "THIS PLATFORM SEEMS TO HAVE 64-BIT LONG DOUBLES."
endif
if mean ({ -1e19, 3, 1e19 }) <> 1
	appendInfoLine: "THIS PLATFORM SEEMS TO HAVE 64-BIT LONG DOUBLES."
endif

for power from 1 to 16
	assert (mean (repeat# (10^power + sequenceA#, n)) - 10^power = mean (sequenceA#))
endfor

appendInfoLine: newline$, "TIMING"
numberOfTrials = 100
stopwatch
for i to numberOfTrials
	size: a#
	size: a#
	size: a#
	size: a#
	size: a#
	size: a#
	size: a#
	size: a#
	size: a#
	size: a#
endfor
appendInfoLine: "Baseline: ", stopwatch / numberOfTrials / n7 * 1e9 / 10, " ns"
for idebug from 1 to size (debug#)
	Debug: "no", debug# [idebug]
	stopwatch
	for i to numberOfTrials
		mean: a#
		mean: a#
		mean: a#
		mean: a#
		mean: a#
		mean: a#
		mean: a#
		mean: a#
		mean: a#
		mean: a#
	endfor
	appendInfoLine: debug$ [idebug], " mean: ", stopwatch / numberOfTrials / n7 * 1e9 / 10, " ns"
endfor
for idebug from 1 to size (debug#)
	Debug: "no", debug# [idebug]
	stopwatch
	for i to numberOfTrials
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
		stdev: a#
	endfor
	appendInfoLine: debug$ [idebug], " stdev: ", stopwatch / numberOfTrials / n7 * 1e9 / 10, " ns"
endfor

appendInfoLine: newline$, "ONE PEAK"
procedure do_single_peak: peakLocation, zeroLocation
	a# = d + repeat# ({ 1e13+1e5 }, 1e6 + 2)
	a# [peakLocation] = d + (-1e19-1e11)
	a# [zeroLocation] = d
	appendInfoLine: debug$ [idebug], ": ", mean (a#) - d, " ", mean (a#) + mean (a# - mean (a#)) - d
endproc
for idebug from 1 to size (debug#)
	Debug: "no", debug# [idebug]
	@do_single_peak: 1, 2
endfor

appendInfoLine: newline$, "OK"
