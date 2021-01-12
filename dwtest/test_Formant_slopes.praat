# test_Formant_slopes.praat
# djmw 20210112

sound = Create Sound from formula: "sineWithNoise", 1, 0, 0.2, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
formant = To Formant (burg): 0, 5, 5000, 0.025, 50
tmin = 0.1
tmax = 0.2

for i to 10
	a = randomUniform (300, 700)
	b = randomUniform (-200, 200)
	c = randomUniform (-50, 50)
	Formula (frequencies): ~ a + b*exp(c*x) + 1000 * (row -1)
	for iformant to 4
		slope# = List formant slope: iformant, tmin, tmax
		flocus = a + b*exp(c*tmin)
		ftarget = a + b * exp (c*tmax )
		slope = (ftarget -flocus)/(tmax -tmin)
		slopeFit = slope#[1]
		cFit = slope# [6]
		assert abs ((slopeFit - slope) / slope) < 0.01; 'slopeFit' 'slope'
		assert abs ((cFit - c) / c) < 0.01; 
	endfor
endfor
removeObject: sound, formant
