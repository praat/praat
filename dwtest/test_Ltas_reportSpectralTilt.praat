# test_Ltas_reportSpectralTilt.praat
# djmw 20130813

appendInfoLine ("test_Ltas_reportSpectralTilt")

s[1] = do ("Create Sound from formula...", "sineWithNoise", 1, 0, 1, 44100, "randomGauss(0,0.1)")
s[2] = do ("Filter (de-emphasis)...", 50)
# some of the following values are rough guesses, if the assertion failes adapt the values somewhat
slope[1] = 0 ; dB/Hz (this value is exact)
slopemargin[1] = 0.5 ; approximate
slope[2] = -20 ; dB/decade (this value is exact)
slopemargin[2] = 1.5 ; approximate
offset[1] = 30.8 ; dB approximate
offsetmargin[1] = 3 ; approximate
offset[2] = 95 ; dB ; approximate
offsetmargin[2] = 4 ; approximate
for i to 2
	selectObject (s[i])
	ltas = do ("To Ltas...", 100)
	info$ = do$ ("Report spectral tilt...", 100, 10000, "Logarithmic", "Robust")
	slope =  extractNumber (info$, "Slope:")
	offset = extractNumber (info$, "Offset:")
	appendInfoLine (tab$, "Slope= ", fixed$ (slope, 4), "; Offset= ", fixed$ (offset, 4))
	assert slope > (slope[i] - slopemargin[i]) and slope < (slope[i] + slopemargin[i])
	assert offset > (offset[i] - offsetmargin[i]) and offset < (offset[i] + offsetmargin[i])
	removeObject (s[i], ltas)
endfor

appendInfoLine ("test_Ltas_reportSpectralTilt OK")
