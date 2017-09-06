writeInfoLine: "sort..."

numberOfChecks = 70
durations# = zero# (numberOfChecks)
for icheck from 1 to numberOfChecks
	n = icheck
	result$ = Praat test: "TimeSort", string$ (10^7 / n), string$ (n), "", ""
	durations# [icheck] = extractNumber (result$, "")
	appendInfoLine (n, " ", durations# [n])
endfor
for icheck from 1 to 8
	n = 10^icheck
	result$ = Praat test: "TimeSort", string$ (max (1, 10^7 / n)), string$ (n), "", ""
	duration = extractNumber (result$, "")
	appendInfoLine (n, " ", duration)
endfor

appendInfoLine: "mean ", mean (durations#), " nanoseconds"

;for n to 1000
appendInfoLine: "OK"
