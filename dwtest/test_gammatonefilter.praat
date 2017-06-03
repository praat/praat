# test_gammatonefilter.praat
# djmw 20170531

for i to 5
	puls [i] = Create Sound from formula: "puls", 1, 0, i * 0.2, 44100, "col=1"
endfor


header$ = "F"+tab$+"Q"+tab$+"Peak(dB)"+tab$+"dF"
output$ = header$ +newline$
q = 0.1
f = 1000

for ifreq to 20
	b = 100
	f = 500 + (ifreq - 1) * 100
	for i to 5
		selectObject: puls [i]
		s = Filter (gammatone): f, b
		spec = To Spectrum: "yes"
		be [i]  = Get band energy: f - 0.5 * b, f + 0.5 * b
		removeObject: s, spec
	endfor
	for i from 2 to 5
		;appendInfoLine: abs(be[i] -be [i-1]) / abs (be[i])
		assert abs(be[i] -be [i-1]) / abs (be[i]) < 1e-4
	endfor
endfor

for i to 5
	removeObject: puls [i]
endfor

