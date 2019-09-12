# test_PowerCepstrum.praat
# djmw 20190910

appendInfoLine: "test_PowerCepstrum.praat"
toneComplex = Create Sound as tone complex: "toneComplex", 0, 1, 44100, "cosine", 100, 0, 0, 0
powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
powercepstrum = To PowerCepstrum (slice): 0.1
peak_dB = Get peak: 60, 333, "Parabolic"
peak_q = Get quefrency of peak: 60, 333, "Parabolic"

peak_prominence = Get peak prominence: 60, 333, "Parabolic", 0.001, 0, "Exponential decay", "Robust"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (exponential, robust)"
peak_prominence = Get peak prominence: 60, 333, "Parabolic", 0.001, 0, "Exponential decay", "Least squares"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (exponential, least squares)"
peak_prominence = Get peak prominence: 60, 333, "Parabolic", 0.001, 0, "Exponential decay", "Robust slow"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (exponential, robust slow)"
peak_prominence = Get peak prominence: 60, 333, "Parabolic", 0.001, 0, "Straight", "Robust"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (straight, robust)"

# old commands
slope_old = Get tilt line slope: 0.001, 0, "Exponential decay", "Robust"
slope = Get trend line slope: 0.001, 0, "Exponential decay", "Robust"
assert (slope_old = slope)
appendInfoLine: tab$, "Slope: ", slope, " (exponential, robust)"
intercept_old = Get tilt line intercept: 0.001, 0, "Straight", "Robust"
intercept = Get trend line intercept: 0.001, 0, "Straight", "Robust"
assert intercept_old = intercept
appendInfoLine: tab$, "Intercept: ", intercept, " dB (straight, robust)"
rnr = Get rhamonics to noise ratio: 60, 333, 0.05
appendInfoLine: tab$, "Rhamonics-to-noise-ratio: ", rnr

@draw_powercepstrum: powercepstrum

removeObject: powercepstrum, powercepstrogram, toneComplex

appendInfoLine: "test_PowerCepstrum.praat OK"

procedure draw_powercepstrum: .pc
	selectObject: .pc
	Erase all
	Select outer viewport: 0, 5, 0, 4
	Draw: 0, 0, 0, 0, "yes"
	Colour: "Green"
	Draw trend line: 0, 0, 0, 0, 0.001, 0, "Exponential decay", "Robust"
	Colour: "Red"
	Draw trend line: 0, 0, 0, 0, 0.001, 0, "Straight", "Least squares"
	Colour: "Black"
endproc