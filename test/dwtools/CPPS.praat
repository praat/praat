writeInfoLine: "CPPS"
sound = Create Sound as tone complex: "toneComplex", 0, 1, 44100, "cosine", 100, 0, 0, 0
ceps = noprogress To PowerCepstrogram: 60, 0.0020001, 5000, 50
cpps = Get CPPS: "yes", 0.02, 0.0005, 60, 330, 0.05, "Parabolic", 0.001, 0, "Exponential decay", "Robust"
appendInfoLine: "cpps = ", cpps, " dB"
if fixed$ (cpps, 5) <> "29.06952"
	appendInfoLine: "WARNING: should have been 29.06952 dB"
endif
removeObject: sound, ceps

for i to 10
	sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
	ceps = noprogress To PowerCepstrogram: 60, 0.0020001, 5000, 50
	cpps = Get CPPS: "yes", 0.02, 0.0005, 60, 330, 0.05, "Parabolic", 0.001, 0, "Exponential decay", "Robust"
	appendInfoLine: cpps
	if not (3.6 < cpps and cpps < 4.6)
		appendInfoLine: "WARNING: should have been between 4.0 and 4.9 dB"
	endif
	removeObject: sound, ceps
endfor
appendInfoLine: "CPPS OK"
