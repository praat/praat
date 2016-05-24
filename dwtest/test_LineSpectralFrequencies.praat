# test_LineSpectralFrequencies.praat
# djmw 20160511

appendInfoLine: "test_LineSpectralFrequencies"
samplingFrequency = 8000
eps = 1e-5
for i to 9
	s = Create Sound as tone complex: "toneComplex", 0, 0.5, samplingFrequency, "Cosine", 100, 0, 0, 0
	appendInfoLine: tab$, "sampling frequency = ", samplingFrequency
	for nc from 8 to 20
		appendInfoLine: tab$, tab$, "lpc order = ", nc
		selectObject: s
		lpc1 = To LPC (autocorrelation): nc, 0.025, 0.005, 50
		f1 = To Formant
		selectObject: lpc1
		lsf = To LineSpectralFrequencies: 0.0
		lpc2 = To LPC
		f2 = To Formant
		for iformant to nc/2
			selectObject: f1
			fi1 = Get value at time: iformant, 0.25, "Hertz", "Linear"
			selectObject: f2
			fi2 = Get value at time: iformant, 0.25, "Hertz", "Linear"
			assert abs (fi1 - fi2) < eps * fi1
		endfor
		removeObject: lpc1, f1, lsf, lpc2, f2
	endfor
	removeObject: s
	samplingFrequency += 2000
endfor

appendInfoLine: "test_LineSpectralFrequencies OK"
