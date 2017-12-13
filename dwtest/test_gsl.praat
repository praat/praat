# test_gsl.praat
# djmw 20071017, 20080317, 20120223

debug = 0
eps = 2.2204460492503131e-16
tol0 =       2.0*eps
tol_2=       4.0*eps
tol1 =      16.0*eps
tol2 =     256.0*eps
tol3 =    2048.0*eps
tol4 =   16384.0*eps
tol5 =  131072.0*eps
tol6 = 1048576.0*eps
sqrt_tol0 = sqrt(2.0*eps)

appendInfoLine: "test_gsl.praat"

@test_besselI
@test_besselK
@test_erfc
@test_erf
@test_lnGamma
@test_incompleteBeta
@test_incompleteGammaP
@test_sincpi

appendInfoLine: "test_gsl.praat OK"

procedure func_1arg: .func$, .arg, .r, .tol
	assert$ = ""
	.form$ = .func$ + "(" + string$(.arg) + ")"
	.res = evaluate (.form$)
	.result$ = .form$ + "=" + string$ (.res) + " (" + string$ (.r) + ")"
	.denom = .res+.r
	if .denom = 0
		.denom = 1
	endif
	.z = abs ((.res-.r) / .denom)
	if .z > .tol
		assert$ = "!!Assertion failed: "
	endif 
	if debug <> 0 or assert$ <> ""
		.neps = .z / eps
		appendInfoLine: tab$, tab$, assert$, .result$, ";  n*eps=", fixed$(.neps,2)
	endif
endproc

procedure func_1argzero: .func$, .arg, .tol
	assert$ = ""
	.form$ = .func$ + "(" + string$(.arg) + ")"
	.res = evaluate (.form$)
	.z = abs (.res)
	if .z > .tol
		assert$ = "!!Assertion failed: "
	endif 
	if debug <> 0 or assert$ <> ""
		.neps = .z / eps
		appendInfoLine: tab$, tab$, assert$, " ", .form$, "=0,  n*eps=", fixed$ (.neps,2)
	endif
endproc

procedure func_2args: .func$, .arg1, .arg2, .r, .tol
	.form$ = .func$ + "(" + string$(.arg1) + "," + string$ (.arg2) + ")"
	.res = evaluate (.form$)
	.result$ = .form$ + "=" + string$ (.res) + " (" + string$ (.r) + ")"
	.denom = .res+.r
	if .denom <> 0
		.z = abs((.res-.r)/(.res+.r))
		if debug = 1
			.neps = .z/eps 
			appendInfoLine: tab$, tab$, .result$, "; ", fixed$(.neps,2), " ", .z
		endif
		assert abs((.res-.r)/(.res+.r)) < .tol; '.result$' ('.z')
	else
		if debug = 1
			.z = abs(.res-.r)
			.neps = .z / eps 
			appendInfoLine: tab$, tab$, .form$, "=", .res, " (",.r, "); ", fixed$ (.neps,2), " ", .z
		endif
		assert abs(.res-.r) < .tol; '.result$'
	endif
endproc

procedure func_3args .func$ .arg1 .arg2 .arg3 .r .tol
	.form$ = .func$ + "(" + string$(.arg1) + "," + string$ (.arg2) + "," + string$ (.arg3) + ")"
	.res = evaluate (.form$)
	.result$ = .form$ + "=" + string$ (.res) + " (" + string$ (.r) + ")"
	.denom = .res+.r
	if .denom <> 0
		if debug = 1
			.z = abs((.res-.r)/(.res+.r))
			.neps = .z/eps 
			appendInfoLine: tab$, tab$, .result$, "; ", fixed$ (.neps,2), " ", .z
		endif
		assert abs((.res-.r)/(.res+.r)) < .tol; '.result$'
	else
		if debug = 1
			.z = abs(.res-.r)
			.neps = .z / eps 
			appendInfoLine: tab$, tab$, .result$, "; ", fixed$ (.neps,2), " ", .z
		endif
		assert abs(.res-.r) < .tol; '.result$'
	endif
endproc

procedure test_besselI
	appendInfoLine: tab$,  "besselI:"
	@func_2args: "besselI", 4, 0.1, 2.6054690212996573677e-07, tol0
	@func_2args: "besselI", 5, 2.0, 0.009825679323131702321, tol0
	@func_2args: "besselI", 100, 100.0, 4.641534941616199114e+21, tol2
endproc

procedure test_besselK
	appendInfoLine: tab$,  "besselK:"
	@func_2args: "besselK", 4, 0.1, 479600.2497925682849, tol_2
	@func_2args: "besselK", 5, 2.0, 9.431049100596467443, tol0
	@func_2args: "besselK", 100, 100.0, 7.617129630494085416e-25, tol2
endproc

procedure test_erfc
	appendInfoLine: tab$,  "erfc:"
	@func_1arg: "erfc", -10.0, 2.0, tol0
	@func_1arg: "erfc", -5.0000002, 1.9999999999984625433, tol0
	@func_1arg: "erfc", -5.0, 1.9999999999984625402, tol0
	@func_1arg: "erfc", -1.0, 1.8427007929497148693, tol0
	@func_1arg: "erfc", -0.5, 1.5204998778130465377, tol0
	@func_1arg: "erfc", 1.0, 0.15729920705028513066, tol0
	@func_1arg: "erfc", 3.0, 0.000022090496998585441373, tol1
	@func_1arg: "erfc", 7.0, 4.183825607779414399e-23, tol2
	@func_1arg: "erfc", 10.0, 2.0884875837625447570e-45, tol2
endproc

procedure test_erf
	appendInfoLine: tab$,  "erf:"
	@func_1arg: "erf", -10.0, -1.0000000000000000000, tol0
	@func_1arg: "erf", 0.5, 0.5204998778130465377, tol0
	@func_1arg: "erf", 1.0, 0.8427007929497148693, tol0
	@func_1arg: "erf", 10.0, 1.0000000000000000000, tol0
endproc

procedure test_lnGamma
	appendInfoLine: tab$, "lnGamma:"
	@func_1arg: "lnGamma", -0.1, 2.368961332728788655, tol0
	@func_1arg: "lnGamma", -1.0/256.0, 5.547444766967471595, tol0
	@func_1arg: "lnGamma", 1.0e-08, 18.420680738180208905, tol0
	@func_1arg: "lnGamma", 0.1, 2.252712651734205, tol0
	@func_1arg: "lnGamma", 1.0+1.0/256.0, -0.0022422226599611501448, tol0
	@func_1arg: "lnGamma", 2.0+1.0/256.0, 0.0016564177556961728692, tol0
	@func_1arg: "lnGamma", 100.0, 359.1342053695753, tol0
	@func_1arg: "lnGamma", -1.0-1.0/65536.0, 11.090348438090047844, tol0
	@func_1arg: "lnGamma", -1.0-1.0/268435456.0, 19.408121054103474300, tol0
	@func_1arg: "lnGamma", -100.5, -364.9009683094273518, tol0
	@func_1arg: "lnGamma", -100-1.0/65536.0, -352.6490910117097874, tol0
	@func_1arg: "lnGamma", -1.5, ln(4*sqrt(pi)/3), tol1
	@func_1arg: "lnGamma", 0.5, 0.5*ln(pi), tol0
	@func_1arg: "lnGamma", 1, 0, tol0
	@func_1arg: "lnGamma", 1.5, ln(sqrt(pi)/2), tol2
	@func_1arg: "lnGamma", 2, 0, tol0
	@func_1arg: "lnGamma", 2.5, ln(3*sqrt(pi)/4), tol1
	@func_1arg: "lnGamma", 3, ln(2), tol_2
	@func_1arg: "lnGamma", 3.5, ln(15*sqrt(pi)/8), tol_2
	@func_1arg: "lnGamma", 4, ln(6), tol_2
endproc


#	incompleteBeta
# Limiting values: $I_0(a,b)=0 I_1(a,b)=1$
# Symmetry: $I_x(a,b) = 1 - I_{1-x}(b,a)$

procedure test_incompleteBeta
	appendInfoLine: tab$, "incompleteBeta:"
	appendInfoLine: tab$, tab$, "incompleteBeta (a,b,0) = 0 & incompleteBeta (a,b,1) = 1:"
	for .i to 20
		.a = randomUniform (0, 50)
		.b = randomUniform (0, 50)
		@func_3args: "incompleteBeta", .a, .b, 0, 0, tol0
		@func_3args: "incompleteBeta", .a, .b, 1, 1, tol0
	endfor
	appendInfoLine: tab$, tab$, "incompleteBeta(i, 1, 0.1)=10^(-i) & incompleteBeta(a,b,x)=1-incompleteBeta(b,a,1-x):"
	for .i to 310
		.a = .i
		.b = 1
		.x = 0.1
		@func_3args: "incompleteBeta", .a, .b, .x ,10^(-.i), tol1*.i
		#	if i > 16 then 1-10^(-.i) equals 1!
		@func_3args: "incompleteBeta", .b, .a, 1-.x, 1-10^(-.i), tol1*.i
	endfor
endproc

procedure test_incompleteGammaP
	appendInfoLine: tab$, "incompleteGammaP:"
	appendInfoLine: tab$, tab$, "incompleteGammaP(a,0)=0 & incompleteGammaP(a,inf)=1:"	 
	for .i to 20
		.a = randomUniform (0, 100)
		@func_2args: "incompleteGammaP", .a, 0, 0, tol0
		@func_2args: "incompleteGammaP", .a, 10^200, 1, tol0
	endfor
	appendInfoLine: tab$, tab$, "incompleteGammaP(1,x)=1-exp(-x):"
	emax = round(19*ln(10))
	emin = 0.01 ; smaller values need larger eps
	for .i to 50
		.x = randomUniform (emin, emax)
		@func_2args: "incompleteGammaP", 1, .x, 1-exp(-.x), tol3
		.x = randomUniform (0, emin)
		@func_2args: "incompleteGammaP", 1, .x, 1-exp(-.x), 1e-6
	endfor
	appendInfoLine: tab$, tab$, "incompleteGammaP(2,x)=(1-(1+x)*exp(-x)):"
	for .i to 100
		.x = randomUniform (emin, emax)
		@func_2args: "incompleteGammaP", 2, .x, (1-(1+.x)*exp(-.x)), 2*tol4
	endfor
	appendInfoLine: tab$, tab$, "incompleteGammaP(3,x)=(1-(1+x+0.5*x^2)*exp(-x))/2:"
	for .i to 50
		.x = randomUniform (emin, emax)
		@func_2args: "incompleteGammaP", 3, .x, 1-(1+.x+0.5*.x^2)*exp(-.x), 2*tol6
		.x = randomUniform (0.0001, emin)
		@func_2args: "incompleteGammaP", 3, .x, 1-(1+.x+0.5*.x^2)*exp(-.x), 7e-4
	endfor
endproc

procedure test_sincpi
	appendInfoLine: tab$, "sincpi:"
	@func_1arg: "sincpi", 0, 1, tol0
	for .i to 200
		.arg = .i
		@func_1argzero: "sincpi", .arg, tol0
		.arg = -.arg
		@func_1argzero: "sincpi", .arg, tol0
	endfor
endproc

procedure gP
	for .i to 10
		.x = randomUniform(0,0.1)
		.r1= incompleteGammaP(2, .x)
		.r2 = 1-(1+.x)*exp(-.x)
		.diff = .r1 - .r2
		appendInfoLine: .x, " ", .diff, " ", .r1
	endfor
endproc
