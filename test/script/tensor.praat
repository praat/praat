writeInfoLine: "vectors and matrices..."

a# = zero#(16)
a#[3] = 4
assert a#[3] = 4

asserterror Vector b# does not exist.
b# [5] = 3

asserterror A vector index cannot be less than 1 (the index you supplied is 0).
a# [0] = 932875289

asserterror A vector index cannot be greater than the number of elements (here 16). The index you supplied is 20.
a# [20] = 45786457

assert numberOfRows (zero## (5, 6)) = 5
assert numberOfColumns (zero## (5, 6)) = 6
a## = zero## (5, 6)
assert numberOfRows (a##) = 5
assert a## [3, 4] = 0

a## [5, 6] = 567
assert a##[5,6] = 567

c# = linear# (0, 100, 101)
assert c# [98] = 97
c# = linear# (0, 100, 101, 0)
assert c# [98] = 97
c# = linear# (0, 100, 100, 1)
assert c# [98] = 97.5

d# = randomGauss# (c#, 20, 1)
a = d# [98]
b = d# [99]
c = d# [100]
printline 'a' 'b' 'c'

d# = randomUniform# (c#, 7, 10)
a = d# [98]
b = d# [99]
c = d# [100]
printline 'a' 'b' 'c'

d# = randomInteger# (c#, 7, 10)
a = d# [98]
b = d# [99]
c = d# [100]
printline 'a' 'b' 'c'

e# = a# + a#
assert e# [3] = 8

asserterror numbers of elements should be equal
e# = a# + d#

; q### =
; data####
;e# = d# + c#

;speaker$# = empty$# [2]
;speaker$# [1] = "JM"
;speaker$# [2] = "PB"

;speaker$ [1] = "JM"
;speaker$ [2] = "PB"

#
# outer##
#
n# = zero# (100)
x# = randomInteger# (n#, 1, 1e7)
y# = randomInteger# (n#, 1, 1e7)
mat## = outer## (x#, y#)
for row to 100
	for col to 100
		assert mat## [row, col] = x# [row] * y# [col]
	endfor
endfor

a# = { 1, 4, 9, 16, 25 }
assert sum (a#) = 55
assert mean (a#) = 11
assert abs (mean (a#) - sum (a#) / 5) < 1e-14
assert abs (stdev (a#) - 9.669539802906858) < 1e-14
assert abs (stdev (a#) - sqrt (sumOver (i to 5, (a# [i] - mean (a#)) ^ 2) / 4)) < 1e-14
assert abs (center (a#) - 4.090909090909091) < 1e-14
assert abs (center (a#) - sumOver (i to 5, i * a# [i]) / sum (a#)) < 1e-14

stopwatch
for i to 1e6
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
	a = 10+5
endfor
t = stopwatch / 10 * 1000
appendInfoLine: t, " ns"

printline OK
