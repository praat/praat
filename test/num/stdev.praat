a# = zero# (1000000)
writeInfoLine: stdev (randomGauss# (a#, 0, 1))
assert stdev ({ 40, 50, 60 }) = 10
assert stdev ({ { 40, 50, 60 } }) = 10
assert stdev ({ { 40 }, { 50 }, { 60 } }) = 10

for power from 1 to 18
	a# = 10 ^ power - randomUniform (0, 1) +
	... { 14.34629189464373, 7.23754354546, 13.645326754, 16.45342671345 }
	appendInfoLine: power, " ", stdev (a#)
endfor

for power from 1 to 18
	sigma = 10 ^ - power
	a# = randomGauss# (a#, 1, sigma)
	stdev = stdev (a#)
	appendInfoLine: sigma, " ", stdev
endfor
