for power from 1 to 18
	a# = 10 ^ power - randomUniform (0, 1) +
	... { 14.34629189464373, 7.23754354546, 13.645326754, 16.45342671345 }
	appendInfoLine: power, " ", stdev (a#)
endfor
