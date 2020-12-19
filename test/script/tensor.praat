writeInfoLine: "vectors and matrices..."

allow_metal = 0

a# = zero#(16)
a#[3] = 4
assert a#[3] = 4
a# ~ col ^ 2
assert a# = { 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225, 256 }

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
appendInfoLine: a, " ", b, " ", c

d# = randomUniform# (c#, 7, 10)
a = d# [98]
b = d# [99]
c = d# [100]
appendInfoLine: a, " ", b, " ", c

d# = randomInteger# (c#, 7, 10)
a = d# [98]
b = d# [99]
c = d# [100]
appendInfoLine: a, " ", b, " ", c

e# = a# + a#
assert e# [3] = 18

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
for irow to 100
	for icol to 100
		assert mat## [irow, icol] = x# [irow] * y# [icol]
	endfor
endfor

squares# = { 1, 4, 9, 16, 25 }
assert sum (squares#) = 55
assert mean (squares#) = 11
assert abs (mean (squares#) - sum (squares#) / 5) < 1e-14
assert abs (stdev (squares#) - 9.669539802906858) < 1e-14
assert abs (stdev (squares#) - sqrt (sumOver (i to 5, (squares# [i] - mean (squares#)) ^ 2) / 4)) < 1e-14
assert abs (center (squares#) - 4.090909090909091) < 1e-14
assert abs (center (squares#) - sumOver (i to 5, i * squares# [i]) / sum (squares#)) < 1e-14
other# = { 2, 1.5, 1, 0.5, 0 }
assert inner (squares#, other#) = 25
assert sumOver (i to 5, squares# [i] * other# [i]) = 25

combi## = { squares#, other# }
assert combi## = { { 1, 4, 9, 16, 25 }, { 2, 1.5, 1, 0.5, 0 } }

a# = squares# + 5
assert a# = { 6, 9, 14, 21, 30 }
b# = a# + { 3.14, 2.72, 3.16, -1, 7.5 }
assert b# = { 9.14, 11.72, 17.16, 20, 37.5 }
c# = b# / 2
appendInfoLine: c#
assert c# = { 4.57, 5.86, 8.58, 10, 18.75 }
d# = b# * c#
norm = norm (d# - { 41.7698, 68.6792, 147.2328, 200, 703.125 })
assert norm < 1e-13   ; 'norm'

stopwatch
iterations = 1e5
for i to iterations
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
t = stopwatch / 10 * 1e9 / iterations
appendInfoLine: t, " ns"

stopwatch
a# = { 4, 9, 16 }
b# = { 25, 36, 49 }
iterations = 1e5
for i to iterations
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
	c# = a# - b#
endfor
t = stopwatch / 10 * 1e9 / iterations / 3
appendInfoLine: t, " ns"

stopwatch
n = 100
z# = zero# (n)
a# = randomGauss# (z#, 0, 1)
b# = randomGauss# (z#, 0, 1)
iterations = 1e4
for i to iterations
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
	m## = outer## (a#, b#)
endfor
t = stopwatch / 10 * 1e9 / iterations / n^2
appendInfoLine: t, " ns"

a## = zero## (20, 50)
b## = zero## (50, 17)
a## ~ row ^ 2 + col ^ 3
b## ~ row ^ 5 + col ^ 7
appendInfoLine: "mul##:", newline$, mul## (a##, b##)
appendInfoLine: "mul_fast##", newline$, mul_fast## (a##, b##)
if allow_metal
	appendInfoLine: "mul_metal##", newline$, mul_metal## (a##, b##)
endif

a## = {{ 2, 3, 5 }, { 7, 3, 2 }}
b## = {{ 11, 1, 5, 2 }, { 8, 2, 3, 6 }, { 1, 3, 4, 9 }}
product## = mul## (a##, b##)
assert product## = {{ 51, 23, 39, 67 }, { 103, 19, 52, 50 }}
product_fast## = mul_fast## (a##, b##)
appendInfoLine: product_fast##
assert numberOfRows (product_fast##) = 2
assert numberOfColumns (product_fast##) = 4
assert product## = product_fast##
if allow_metal
	product_metal## = mul_metal## (a##, b##)
	appendInfoLine: product_metal##
	assert numberOfRows (product_metal##) = 2
	assert numberOfColumns (product_metal##) = 4
	assert product## = product_metal##
endif
at## = transpose## (a##)
bt## = transpose## (b##)
assert at## = {{ 2, 7 }, { 3, 3 }, { 5, 2 }}
assert transpose## (bt##) = b##
assert mul_tn## (at##, b##) = product##
assert mul_nt## (a##, bt##) = product##
assert mul_tt## (at##, bt##) = product##

@do: { 1, 103, 7 }
procedure do: v#
	.result = mean (v#)
endproc
assert do.result = 37

appendInfoLine: "OK"
