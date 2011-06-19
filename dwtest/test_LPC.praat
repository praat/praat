# test_LPC.praat
# djmw 20080122, 20110528

printline test_LPC

# formants of straigth tube

for i to 5
	f[i] = (2*i-1)*500
	b[i] = i * 50
endfor

debug = 0

pt = Create PitchTier... empty 0 0.5
Add point... 0 150
source = To Sound (pulse train)... 44100 1 0.05 2000 no
ft = Create FormantTier... empty 0 0.5
Add point... 0  'f[1]' 'b[1]' 'f[2]' 'b[2]' 'f[3]' 'b[3]' 'f[4]' 'b[4]' 'f[5]' 'b[5]'
plus source
soundpre = Filter

sound = Resample... 10000 50

select pt
plus source
plus ft
plus soundpre
Remove

for imethod to 4
	select sound
	if imethod = 1
		method$ = "auto"
		lpc = To LPC (autocorrelation)... 10 0.025 0.005 50
	elsif imethod = 2
		method$ = "covar"
		lpc = To LPC (covariance)... 10 0.025 0.005 50
	elsif imethod = 3
		method$ = "burg"
		lpc = To LPC (burg)... 10 0.025 0.005 50
	elsif imethod = 4
		method$ = "marple"
		lpc = To LPC (marple)... 10 0.025 0.005 50 1e-06 1e-06
	endif
	call get_formants 'lpc' 'method$'
	deltaf1 = deltaf
	deltab1 = deltab
	assert deltaf1 < 0.07 and deltab1 < 3.2; ('method$')
	
	select lpc
	plus sound
	lpcr = To LPC (robust)... 0.025 50 1.5 5 1e-06 no
	call get_formants lpcr 'method$':robust
	deltaf2 = deltaf
	deltab2 = deltab
	assert deltaf2 < 0.005 and deltab2 < 0.2; ('method$':robust)
	select lpc
	plus lpcr
	Remove
endfor

select sound
Remove

printline test_LPC OK

procedure get_formants .lpc .method$
	select .lpc
	.f = To Formant (keep all)
	deltaf = 0
	deltab = 0
	for .nf to 5
		.f[.nf] = Get quantile... .nf 0 0 Hertz 0.5
		.b[.nf] = Get quantile of bandwidth... .nf 0 0 Hertz 0.5
		if .nf <= 3
			deltaf += abs(f[.nf]- .f[.nf]) / f[.nf]
			deltab += abs(b[.nf]- .b[.nf]) / b[.nf]
		endif
	endfor
	deltaf /= 3
	deltab /= 3
	printline deltaf = 'deltaf:4' deltab = 'deltab:4' '.f[1]:0' '.b[1]:0' '.f[2]:0' '.b[2]:0' '.f[3]:0' '.b[3]:0' '.f[4]:0' '.b[4]:0' '.f[5]:0' '.b]5:0' ('.method$')
	select .f
	Remove
endproc

