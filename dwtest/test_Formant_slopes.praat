# test_Formant_slopes.praat
# djmw 20210112

appendInfoLine: "test_Formant_slopes.praat"

sound = Create Sound from formula: "sineWithNoise", 1, 0, 0.2, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
formant = To Formant (burg): 0, 5, 5000, 0.025, 50
Formula (bandwidths): ~ 100
tmin = 0.1
tmax = 0.2

info_exp = 1
info_par = 1
info_sig = 1
for i to 10
	@test_exponential
	@test_parabolical
	@test_sigmoid
endfor

removeObject: sound, formant
appendInfoLine: "test_Formant_slopes.praat OK"

procedure test_exponential
	.model$ = "Exp"
	.pi# = {randomUniform (300, 700), randomUniform (-200, 200), randomUniform (-20, 20)}
	.p# = .pi#
	Formula (frequencies): ~ if x>=tmin and x <= tmax then .p#[1] + .p#[2]*exp(.p#[3]*(x - tmin)) + 1000 * (row - 1) else 0 fi
	for .iformant to 3
		.slope# = List formant slope: .iformant, tmin, tmax, "Exponential plus constant"
		.p# [1] = .pi#[1] + 1000 * (.iformant - 1)
		.flocus = .p#[1] + .p#[2]
		.ftarget = .p#[1] + .p#[2] * exp (.p#[3] * (tmax - tmin))
		.slope = (.ftarget - .flocus) / (tmax - tmin)
		.truth# = {.slope, 0, .flocus, .ftarget, .p# [1], .p# [2], .p# [3]}
		@extraInfo: info_exp, .truth#, .slope#, .model$ + string$ (.iformant)+ ": "
		assert .slope# [2] > 0.99
	endfor
endproc

procedure test_parabolical
	.model$ = "Par"
	.pi# = {randomUniform (300, 700), randomUniform (-200, 200), randomUniform (-20, 20)}
	.p# = .pi#
	Formula (frequencies): ~if x>=tmin and x <= tmax then .p#[1] + .p#[2]*(x - tmin)+ .p#[3]*(x-tmin)^2 + 1000 * (row - 1) else 0 fi
	;Formula (frequencies): ~ self+randomGauss (0, row*1) ;
	for .iformant to 3
		.slope# = List formant slope: .iformant, tmin, tmax, "Parabolic"
		.p# [1] = .pi# [1] + 1000 * (.iformant -1)
		.flocus = .p# [1] 
		.ftarget = .p# [1] + .p# [2] * (tmax - tmin) + .p# [3] * (tmax - tmin)^2
		.slope = (.ftarget - .flocus) / (tmax - tmin)
		.truth# = {.slope, 0, .flocus, .ftarget, .p# [1], .p# [2], .p# [3]}
		@extraInfo: info_par, .truth#, .slope#,  .model$+string$ (.iformant)+": "
		assert .slope# [2] > 0.99
	endfor
endproc

procedure test_sigmoid
	.model$ = "Sig"
	.pi# = {randomUniform (300, 700), randomUniform (-200, 200), randomUniform (tmin, tmax), randomUniform (0.1, 10)}
	.p# = .pi#
	Formula (frequencies): ~ if x>=tmin and x <= tmax then .p# [1] + .p# [2] / (1 + exp (- (x - .p# [3])/.p# [4])) + 1000 * (row - 1) else 0 fi
	for .iformant to 3
		.slope# = List formant slope: .iformant, tmin, tmax, "Sigmoid plus constant"
		.p# [1] = .pi# [1] + 1000 * (.iformant -1)
		.flocus = .p# [1] + .p# [2] / (1 + exp (- (- .p# [3])/.p# [4]))
		.ftarget = .p# [1] + .p# [2] / (1 + exp (- (tmax -tmin - .p# [3])/.p# [4]))
		.slope = (.ftarget - .flocus) / (tmax - tmin)
		.truth# = {.slope, 0, .flocus, .ftarget, .p# [1], .p# [2], .p# [3], .p# [4]}
		@extraInfo: info_sig, .truth#, .slope#,  .model$+string$ (.iformant)+": "
		assert .slope# [2] > 0.99
	endfor
endproc

procedure extraInfo: .want_info, .truth#, .slope#, .text$
	if .want_info
		appendInfo:  tab$ + .text$, " slope=(", fixed$(.truth#[1],1), ", ", fixed$(.slope#[1],1), ") "," r^2=", fixed$(.slope#[2],3), 
		... newline$, tab$, tab$, "a=(", fixed$(.truth#[5],1), ", ", fixed$(.slope#[5],1), ")",
		... " b=(", fixed$(.truth#[6],1), ", ", fixed$(.slope#[6],1), ") ", 
		... " c=(", fixed$(.truth#[7],1), ", ", fixed$(.slope#[7],1), ") " 
		if size (.slope#) == 8
			appendInfo:  " d=(", fixed$(.truth#[8],1), ", ", fixed$(.slope#[8],1), ") "
		endif
		appendInfo: newline$
	endif
endproc
