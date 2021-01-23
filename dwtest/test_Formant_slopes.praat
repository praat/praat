# test_Formant_slopes.praat
# djmw 20210112

appendInfoLine: "test_Formant_slopes.praat"

sound = Create Sound from formula: "sineWithNoise", 1, 0, 0.2, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
formant = To Formant (burg): 0, 5, 5000, 0.025, 50
tmin = 0.1
tmax = 0.2

info_exp = 0
info_par = 1
info_sig = 0
for i to 10
	if 0
	@test_exponential
	endif
	@test_parabolical
	@test_sigmoid
endfor

procedure test_exponential
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
		@extraInfo: info_exp, 3, "Exp:"
		rSlope = abs ((slope - slope# [1]) / slope)
		rC = abs ((c - slope# [7]) / c)
		assert rC < 0.01
		assert rSlope < 0.01
	endfor
endproc

procedure extraInfo: .want_info, .npar, .text$
	if .want_info
		appendInfo:  .text$, " slope=(", fixed$(slope,1), " ", fixed$(slope#[1],1), ") "," r^2=(", fixed$(slope#[2],3), ")", 
		... newline$, tab$, "a=(", fixed$(a,1), ", ", fixed$(slope#[5],1), ")",
		... " b=(", fixed$(b,1), " ", fixed$(slope#[6],1), ") ", 
		... " c=(", fixed$(c,1), " ", fixed$(slope#[7],1), ") " 
		if .npar == 4
			appendInfo:  " d=(", fixed$(d,1), " ", fixed$(slope#[8],1), ") "
		endif
		appendInfo: newline$
	endif
endproc

procedure test_parabolical
	a = randomUniform (300, 700)
	b = randomUniform (-200, 200)
	c = randomUniform (-20, 20)
	Formula (frequencies): ~ a + b*x+ c*x^2 + 1000 * (row - 1)
	for iformant to 3
		slope# = List formant slope: iformant, tmin, tmax, "Parabolic"
		ai = a + 1000 * (iformant -1)
		flocus = ai 
		ftarget = ai + b * (tmax -tmin) + c * (tmax -tmin)^2
		slope = (ftarget - flocus) / (tmax - tmin)
		rSlope = abs ((slope - slope# [1]) / slope)
		rC = abs ((c - slope# [7]) / c)
		@extraInfo: info_par, 3, "Par:"
		assert rC < 0.01
		assert rSlope < 0.01
	endfor
endproc
procedure test_sigmoid
	ai = randomUniform (300, 700)
	b = randomUniform (-200, 200)
	c = randomUniform (-20, 20)
	d = randomUniform (0.1, 10)
	Formula (frequencies): ~ ai + b / (1 + exp (- (x - c)/d)) + 1000 * (row - 1)
	for iformant to 3
		slope# = List formant slope: iformant, tmin, tmax, "Sigmoid"
		a = ai + 1000 * (iformant -1)
		flocus = ai
		ftarget = a + b * (tmax -tmin) + c * (tmax -tmin)^2
		slope = (ftarget - flocus) / (tmax - tmin)
		rSlope = abs ((slope - slope# [1]) / slope)
		rC = abs ((c - slope# [7]) / c)
		@extraInfo: info_sig, 4, "Sig:"
		assert rC < 0.01
		assert rSlope < 0.01
	endfor
endproc


removeObject: sound, formant

appendInfoLine: "test_Formant_slopes.praat OK"
