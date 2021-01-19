# test_Formant_slopes.praat
# djmw 20210112

appendInfoLine: "test_Formant_slopes.praat"

sound = Create Sound from formula: "sineWithNoise", 1, 0, 0.2, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
formant = To Formant (burg): 0, 5, 5000, 0.025, 50
tmin = 0.1
tmax = 0.2

for i to 10
	a = randomUniform (300, 700)
	b = randomUniform (-200, 200)
	c = randomUniform (-20, 20)
	Formula (frequencies): ~ a + b*exp(c*x) + 1000 * (row - 1)
	for iformant to 3
		slope# = List formant slope: iformant, tmin, tmax, "Exponential"
		ai = a + 1000 * (iformant -1)
		flocus = ai + b * exp (c * tmin)
		ftarget = ai + b * exp (c * tmax )
		slope = (ftarget - flocus) / (tmax - tmin)
		rSlope = abs ((slope - slope# [1]) / slope)
		rC = abs ((c - slope# [7]) / c)
		assert rC < 0.01
		assert rSlope < 0.01
	endfor
endfor
removeObject: sound, formant

appendInfoLine: "test_Formant_slopes.praat OK"
