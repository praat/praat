# test_PowerCepstrum.praat
# djmw 20190910

appendInfoLine: "test_PowerCepstrum.praat"
toneComplex = Create Sound as tone complex: "toneComplex", 0, 1, 44100, "cosine", 100, 0, 0, 0
powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
cpps = Get CPPS: "yes", 0.02, 0.0005, 60, 333.3, 0.05, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
appendInfoLine: tab$, "CPPS from Cepstrogram: ", cpps, " dB"
powercepstrum = To PowerCepstrum (slice): 0.1
peak_dB = Get peak: 60, 333, "Parabolic"
peak_q = Get quefrency of peak: 60, 333, "Parabolic"

lineType$ = "Exponential decay"
peak_prominence = Get peak prominence: 60, 333.3, "Parabolic", 0.001, 0, lineType$, "Robust"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (exponential, robust)"
peak_prominence = Get peak prominence: 60, 333.3, "Parabolic", 0.001, 0, lineType$, "Least squares"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (exponential, least squares)"
peak_prominence = Get peak prominence: 60, 333.3, "Parabolic", 0.001, 0, lineType$, "Robust slow"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (exponential, robust slow)"
peak_prominence = Get peak prominence: 60, 333.3, "Parabolic", 0.001, 0, "Straight", "Robust slow"
appendInfoLine: tab$, "Prominence: ", peak_prominence, " dB (straight, robust)"

# old commands
slope_old = Get tilt line slope: 0.001, 0, "Exponential decay", "Robust"
slope = Get trend line slope: 0.001, 0, "Exponential decay", "Robust"
assert (slope_old = slope)
appendInfoLine: tab$, "old Slope: ", slope, " (exponential, robust)"
intercept_old = Get tilt line intercept: 0.001, 0, "Straight", "Robust"
intercept = Get trend line intercept: 0.001, 0, "Straight", "Robust"
assert intercept_old = intercept
appendInfoLine: tab$, "old Intercept: ", intercept, " dB (straight, robust)"
rnr = Get rhamonics to noise ratio: 60, 333.3, 0.05
appendInfoLine: tab$, "old Rhamonics-to-noise-ratio: ", rnr

@draw_powercepstrum: powercepstrum

@testPowerCepstrogramSmoothing

Debug: "no", -4
@testPowerCepstrumSmoothing: "Old smoothing (square)"
Debug: "no", -5
@testPowerCepstrumSmoothing: "Gaussian smoothing (4 sigmas)"
Debug: "no", 0
@testPowerCepstrumSmoothing: "Default smoothing (integration)"

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

procedure testPowerCepstrumSmoothing: .info$
	appendInfoLine: tab$, "test smoothing PowerCepstrogram of noisy sound: ", .info$
	.kg = Create KlattGrid from vowel: "a", 0.4, 125, 800, 50, 1200, 50, 2300, 100, 2800, 0.05, 1000
	.cpps_min = 1000
	.cpps_max = - .cpps_min
	.cpps_min1 = .cpps_min
	.cpps_max1 = - .cpps_min1
	for .i to 10
		selectObject: .kg
		.sounda = To Sound
		Formula: ~ self + randomGauss (0, 0.1)
		selectObject: .sounda
		.powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
		.powercepstrum = To PowerCepstrum (slice): 0.1
		.cpps = Get peak prominence: 60, 333.3, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
		.cpps_min = min (.cpps_min , .cpps)
		.cpps_max = max (.cpps_max , .cpps)
		.smooth = Smooth: 0.0005, 1
		.cpps1 = Get peak prominence: 60, 333.3, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
		.cpps_min1 = min (.cpps_min1 , .cpps1)
		.cpps_max1 = max (.cpps_max1 , .cpps1)
		removeObject: .powercepstrogram, .powercepstrum, .smooth, .sounda
	endfor
	appendInfoLine: tab$, tab$, fixed$ (.cpps_min, 2),  " / " , fixed$ (.cpps_max, 2), " dB min / max; ", 
		...  fixed$ (.cpps_min1, 2),  " / " , fixed$ (.cpps_max1, 2), "dB min / max after smoothing"
	removeObject: .kg
	appendInfoLine: tab$, "test smoothing of PowerCepstrogram OK"
endproc

procedure testPowerCepstrogramSmoothing
	appendInfoLine: tab$, "compare PowerCepstrogram and PowerCepstrum smoothing"
	.toneComplex = Create Sound as tone complex: "toneComplex", 0, 1, 44100, "cosine", 100, 0, 0, 0
	.powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
	.powercepstrum = To PowerCepstrum (slice): 0.1
	for .ismooth to 5
		.quefrencyAveragingWindow = randomUniform (0.0006, 0.0003)
		selectObject: .powercepstrogram
		# smooth only in y-direction
		.smoothcepstrogram = Smooth: 0.0000001, .quefrencyAveragingWindow
		.smoothpowercepstrum = To PowerCepstrum (slice): 0.1
		.cpps = Get peak prominence: 60, 333.3, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
		selectObject: .powercepstrum
		.smoothpowercepstrum2 = Smooth: .quefrencyAveragingWindow, 1
		.cpps2 = Get peak prominence: 60, 333.3, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
		assert .cpps == .cpps2 ;
		removeObject: .smoothcepstrogram, .smoothpowercepstrum, .smoothpowercepstrum2
	endfor
	removeObject: .toneComplex, .powercepstrogram, .powercepstrum
	appendInfoLine: tab$, "test PowerCepstrogram and PowerCepstrum smoothing OK"
endproc


