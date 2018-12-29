writeInfoLine: "sum..."

for i from 2 to 1000
	assert sum (linear# (1, i, i, 0)) = sum (linear# (1, i - 1, i - 1, 0)) + i   ; 'i'
endfor

for i from 1 to 10
	appendInfoLine: sum (linear# (1, i, i, 0)), " ", mean (linear# (1, i, i, 0))
	assert sum (linear# (1, i, i, 0)) = i * mean (linear# (1, i, i, 0))   ; 'i'
endfor

numberOfChecks = 100
durations# = zero# (numberOfChecks)
for n from 1 to numberOfChecks
	result$ = Praat test: "TimeSum", string$ (10^8 / n), string$ (n), "", ""
	durations# [n] = extractNumber (result$, newline$)
	appendInfoLine (n, " ", durations# [n])
endfor

appendInfoLine: "mean ", mean (durations#), " Gflop/s"

;for n to 1000
appendInfoLine: "OK"
