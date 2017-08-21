writeInfoLine: "inner..."

assert inner(linear#(1,100,100,0),linear#(1,100,100,0)) = 338350
assert inner(linear#(1,100,100,0),1000000+linear#(1,100,100,0)) = 5050338350

for size from 1 to 100
	x# = linear# (1, size, size, 0)
	y# = 1000000 + x#
	assert inner (x#, y#) = sumOver (i to size, x# [i] * y# [i])   ; 'i'
endfor

durations# = zero# (100)
for n from 1 to 100
	result$ = Praat test: "TimeInner", string$ (10^8 / n), string$ (n), "", ""
	durations# [n] = extractNumber (result$, newline$)
	appendInfoLine (n, " ", durations# [n])
endfor

appendInfoLine: "mean ", mean (durations#), " nanoseconds"

;for n to 1000
appendInfoLine: "OK"
