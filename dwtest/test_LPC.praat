# test_LPC.praat
# djmw 20080122, 20110528, 20180318

printline test_LPC

@testLPCInterface

# formants of straight tube

fb5$ = ""
space$ = ""
for i to 5
	f[i] = (2*i-1)*500
	b[i] = i * 50
	fb5$ = fb5$ + space$ +  string$ (f[i] ) + " " + string$ (b[i]) 
	space$ = " "
endfor

debug = 0

pt = Create PitchTier: "pt", 0, 0.5
Add point: 0, 150
source = To Sound (pulse train): 44100, 1, 0.05, 2000, "no"
ft = Create FormantTier: "ft", 0, 0.5
Add point: 0,  fb5$

selectObject: ft, source
soundpre = Filter

sound = Resample: 10000, 50

removeObject:  pt, source, ft, soundpre

for imethod to 4
	selectObject: sound
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
	assert deltaf2 < 0.01 and deltab2 < 0.2; ('method$':robust) 'deltaf2' 'deltab2'
	select lpc
	plus lpcr
	Remove
endfor

select sound
Remove

printline test_LPC OK

procedure get_formants: .lpc, .method$
	selectObject: .lpc
	.formant = To Formant (keep all)
	deltaf = 0
	deltab = 0
	for .nf to 5
		.f[.nf] = Get quantile: .nf, 0, 0, "Hertz", 0.5
		.b[.nf] = Get quantile of bandwidth: .nf, 0, 0, "Hertz", 0.5
		if .nf <= 3
			deltaf += abs (f[.nf] - .f[.nf]) / f[.nf]
			deltab += abs (b[.nf]- .b[.nf]) / b[.nf]
		endif
	endfor
	deltaf /= 3
	deltab /= 3
	appendInfoLine: tab$, .method$,  " deltaf = ", fixed$ (deltaf, 4), " deltab = ", fixed$ (deltab, 4), 
	... 	" ", fixed$ (.f[1], 0), " ", fixed$ (.b[1], 0), " ", fixed$ (.f[2], 0), " ", fixed$ (.b[2], 0),
	...	" ", fixed$ (.f[3], 0), " ", fixed$ (.b[3], 0), " ", fixed$ (.f[4], 0), " ", fixed$ (.b[4], 0), 
	...	" ", fixed$ (.f[5], 0), " ", fixed$ (.b[5], 0)
	removeObject: .formant
endproc

procedure testLPCInterface
	.sound = Create Sound from formula: "noise", 1, 0, 1, 10000, "randomGauss(0,0.1)"
	.lpc = To LPC (autocorrelation): 10, 0.025, 0.005, 50

	appendInfoLine: tab$, "test LPC interface"
	appendInfoLine: tab$, tab$, "Query"
	.startTime = Get start time

	.t1 = Get time from frame number: 1
	.endTime = Get end time
	.totalDuration = Get total duration
	.numberOfFrames = Get number of frames
	.timeStep = Get time step
	for .iframe to .numberOfFrames
		.time = Get time from frame number: .iframe
		.frameNumber = Get frame number from time: .time
		.numberOfCoefficients = Get number of coefficients: .iframe
	endfor
	.samplingInterval = Get sampling interval

	appendInfoLine: tab$, tab$, "Modify"
	Shift times by: randomUniform (0.1, 0.9)
	Shift times to: "start time", .startTime
	Shift times to: "centre time", .startTime
	Shift times to: "end time", .endTime
	Scale times by: randomUniform (0.5, 2.5)
	Scale times to: .startTime, .endTime

	appendInfoLine: tab$, tab$, "Extract"
	for .i to 10
		selectObject: .lpc
		.bandwidthReduction = randomUniform (0, 200)
		.time = randomUniform (.startTime, .endTime)
		.spectrum = To Spectrum (slice): .time, 20, .bandwidthReduction, 50
		selectObject: .lpc
		.vocalTract1 = To VocalTract (slice): .time, 17.0
		selectObject: .lpc
		.glottalDamping = randomUniform (0.01, 0.4)
		.radiationDamping$ = if randomUniform (0,1) > 0.5 then "yes" else "no" fi
		.internalDamping$ = if randomUniform (0,1) > 0.5 then "yes" else "no" fi
		.vocalTract2 = To VocalTract (slice, special): .time, .glottalDamping, .radiationDamping$, .internalDamping$
		selectObject: .lpc
		.polynomial = To Polynomial (slice): .time
		removeObject: .spectrum, .vocalTract1, .vocalTract2, .polynomial
	endfor
	selectObject: .lpc
	.matrix = Down to Matrix (lpc)
	removeObject: .matrix

	appendInfoLine: tab$, tab$, "Analyse"
	selectObject: .lpc
	.formant1 = To Formant
	selectObject: .lpc
	.formant2 = To Formant (keep all)
	for .numberOfCoefficients to 12
		selectObject: .lpc
		.lfcc = To LFCC: .numberOfCoefficients
		removeObject: .lfcc
	endfor
	selectObject: .lpc
	.spectrogram = To Spectrogram: 20, 0, 50
	selectObject: .lpc
	.lineSpectralFrequencies = To LineSpectralFrequencies: 0.0
	removeObject: .spectrogram, .formant1, .formant2, .lineSpectralFrequencies, .sound, .lpc
endproc
	
