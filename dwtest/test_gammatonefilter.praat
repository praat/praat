# test_gammatonefilter.praat
# djmw 20170531

for i to 5
	puls [i] = Create Sound from formula: "puls", 1, 0, i * 0.2, 44100, "col=1"
endfor

appendInfoLine: "test_gammatonefilter.praat"
appendInfoLine: tab$, "Filter pulses of duration i * 0.2"
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

appendInfoLine: tab$, "Compare incomplete gamma with Gamma[n,z]"
# values in the csv file were calculated by the Mathematica function 
# N[Gamma[i, i + I i/j], 10]
igt = Read Table from comma-separated file: "incompleteGamma.csv"
numberOfLines = Get number of rows
eps = 1e-7
for irow to numberOfLines
	i = Get value: irow, "i"
	j = Get value: irow, "j"
	re = Get value: irow, "re"
	im = Get value: irow, "im"
	z$ = Get incomplete gamma: i, 0.0, i, i / j
	@complex (z$)
	pre = complex.re
	pim = complex.im
	;appendInfoLine: irow, " ", re, " ", im, " ", pre, " ", pim
	assert abs((pre - re)/pre) < eps && abs ((pim - im)/pim) < eps; 'irow' 'pre' 're' 'pim' 'im' <'z$'>
endfor
removeObject: igt

procedure complex: .z$
	.re = extractNumber (.z$, "")
	.im = extractNumber (.z$, "+")
	if .im = undefined
		; im < 0
		.index = rindex (.z$, "-")
		.length = length (.z$)
		.last$ = mid$ (.z$, .index, .length - .index)
		.im = number (.last$)
	endif
endproc
appendInfoLine: "test_gammatonefilter.praat OK"
