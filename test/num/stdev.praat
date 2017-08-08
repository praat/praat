a# = zero# (1000000)
writeInfoLine: stdev (randomGauss# (a#, 0, 1))

for power from 1 to 18
	a# = 10 ^ power - randomUniform (0, 1) +
	... { 14.34629189464373, 7.23754354546, 13.645326754, 16.45342671345 }
	appendInfoLine: power, " ", stdev (a#)
endfor

for power from 1 to 18
	a# = randomGauss# (a#, 1, 10 ^ - power)
	stdev1 = stdev (a#)
	Debug: "no", 48
	stdev2 = stdev (a#)
	Debug: "no", 49
	stdev3 = stdev (a#)
	Debug: "no", 0
	appendInfoLine: stdev1, " ", stdev2, " ", stdev3
	appendInfoLine: "... ", abs (stdev2 - stdev1), " ", abs (stdev3 - stdev1)
endfor
