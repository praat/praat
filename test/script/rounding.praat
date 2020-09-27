writeInfoLine: "Approximating one third in 64-bit IEEE floating point:"

oneThird$ = fixed$ (1/3, 60)
#
# This one is closer to 1/3 than 0.333333333333333370340767487505218014121055603027343750000000 is:
#
assert oneThird$ = "0.333333333333333314829616256247390992939472198486328125000000"
... or oneThird$ = "0.333333333333333310000000000000000000000000000000000000000000"   ; 'oneThird$'

a [0] = 0
d = 0.5
for i to 65
	a [i] = a [i - 1] + d
	d *= -0.5
	string$ [i] = fixed$ (a [i], 60)
	appendInfoLine: i, " ", string$ [i]
endfor
appendInfoLine: "1/3 -> ", fixed$ (1/3, 60)
assert string$ [54] = fixed$ (1/3, 60)
assert a [53] <> 1/3
assert a [54] = 1/3
assert a [55] <> 1/3   ; this is allowed to vary on platforms
assert a [55] = a [54] or a [55] = a [53]   ; the somewhat laxer condition