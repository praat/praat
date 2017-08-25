# mul.praat

iterations = 1000
size = 1000
vec# = zero# (size)
mat## = zero## (size, size)
vecje# = zero# (1)
matje## = zero## (1, 1)

stopwatch
for i to iterations
	b## = mat##
endfor
writeInfoLine: stopwatch * 1e9 / size ^ 2 / iterations

stopwatch
for i to iterations
	a# = mul# (vec#, mat##)
endfor
appendInfoLine: stopwatch * 1e9 / size ^ 2 / iterations

stopwatch
for i to iterations
	a# = mul# (mat##, vec#)
endfor
appendInfoLine: stopwatch * 1e9 / size ^ 2 / iterations
