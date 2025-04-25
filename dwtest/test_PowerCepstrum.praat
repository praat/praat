# test_PowerCepstrum.praat
# djmw 20190910, 20220721, 20241201

appendInfoLine: "test_PowerCepstrum.praat"

@testCalibratedPowerCepstrums: 10

@testCalibratedPowerCepstrograms: 10

@test_trendSubtraction

@draw_powercepstrum

@testPowerCepstrogramSmoothing

@calibratedPowerCepstrumSmoothing: 1

@test_perfectCosineSpectra

@powerCepstrumSmoothingsOldAndNew

appendInfoLine: "test_PowerCepstrum.praat OK"

procedure testCalibratedPowerCepstrums: .ntries
	appendInfoLine: tab$, "testCalibratedPowerCepstrums"
	for .itry to .ntries
		.duration = randomUniform (0.05, 0.1)
		.approximateF0 = randomUniform (61, 330)
		.peak_dB = randomUniform (20, 70)
		.backgroundDistance_dB = randomUniform (10, 50)
		@testCalibratedPowerCepstrumCPP: .duration, .approximateF0, .peak_dB, .peak_dB - .backgroundDistance_dB
	endfor
	appendInfoLine: tab$, "testCalibratedPowerCepstrums OK"
endproc

procedure testCalibratedPowerCepstrograms: .ntries
	for .itry to .ntries
		.duration = randomUniform (0.3, 2.0)
		.approximateF0 = randomUniform (61, 330)
		.peak_dB = randomUniform (20, 70)
		.backgroundDistance_dB = randomUniform (10, 50)
		@testCalibratedPowerCepstrogram: .duration, .approximateF0, .peak_dB, .peak_dB - .backgroundDistance_dB
	endfor
endproc

procedure other
	selectObject: powercepstrogram
	cpps_subtractTrend = Get CPPS: "yes", 0, 0, 60, 333.3, 0.05, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
	cpps_nosubtractTrend = Get CPPS: "no", 0, 0, 60, 333.3, 0.05, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
	assert abs (cpps_subtractTrend - cpps_nosubtractTrend) < 1e-10

	cpps = Get CPPS: "yes", 0.02, 0.0005, 60, 333.3, 0.05, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
	assert cpps <= cpps_subtractTrend
	appendInfoLine: tab$, "CPPS from Cepstrogram: ", cpps, " dB"
	table = To Table (cepstral peak prominences): "yes", "yes", 6, 3, "yes", 3, 60, 330, 0.05, "parabolic", 0.001, 0.05, "Straight", "Robust slow"
	removeObject: table
	selectObject: powercepstrogram
	powercepstrum = To PowerCepstrum (slice): 0.1
	peak_dB = Get peak: 60, 333, "Parabolic"
	peak_q = Get quefrency of peak: 60, 333, "Parabolic"
endproc

procedure old_commands
	selectObject: powercepstrum
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
endproc

procedure test_perfectCosineSpectra
	appendInfoLine: tab$, "test_perfectCosineSpectra"
	for ipitch to 5
		.f0 = randomInteger (600, 3300) / 10
		@test_perfectCosine: .f0
	endfor
	appendInfoLine: tab$, "test_perfectCosineSpectra OK"
endproc

procedure powerCepstrumSmoothingsOldAndNew
	appendInfoLine: tab$, "powerCepstrumSmoothings (old and new)"
	random_initializeWithSeedUnsafelyButPredictably (111)
	Debug: "no", -4
	@testPowerCepstrumSmoothing: "Old smoothing (square)"
	Debug: "no", -5
	@testPowerCepstrumSmoothing: "Gaussian smoothing (4 sigmas)"
	Debug: "no", 0
	@testPowerCepstrumSmoothing: "Default smoothing (integration)"
	random_initializeSafelyAndUnpredictably ()
	appendInfoLine: tab$, "powerCepstrumSmoothings (old and new) OK"
endproc

procedure test_trendSubtraction
	appendInfoLine: tab$, "test_trendSubtraction"
	.toneComplex = Create Sound as tone complex: "toneComplex", 0, 1, 44100, "cosine", 100, 0, 0, 0
	.powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
	.powercepstrum = To PowerCepstrum (slice): 0.1
	.trendType$# = { "Straight", "Exponential decay" }
	.fitMethod$# = { "Robust", "Robust slow" }
	.interpolation$# = { "none", "parabolic", "cubic" }
	.epsilon# = {1e-10, 1e-2, 1e-2}
	for .interpolation to size (.interpolation$#)
		.interpolation$ = .interpolation$# [.interpolation]
		.eps = .epsilon# [.interpolation]
		for .trendType to size (.trendType$#)
			.trendType$ = .trendType$# [.trendType]
			for .fitMethod to size (.fitMethod$#)
				.fitMethod$ = .fitMethod$# [.fitMethod]
				appendInfoLine: tab$, tab$, .interpolation$, "; ", .trendType$, "; ", .fitMethod$
				selectObject: .powercepstrum				
				.prominence1 = Get peak prominence: 60, 350, .interpolation$, 
					... 0.001, 0.05, .trendType$, .fitMethod$
				.trendRemoved = Subtract trend: 0.001, 0.05, .trendType$, .fitMethod$
				.prominence2 = Get peak prominence: 60, 350, .interpolation$, 
					... 0.001, 0.05, .trendType$, .fitMethod$
				# the assertion fails for the least squares fit. 
				assert abs ((.prominence1 - .prominence2)/.prominence1) < .eps ; '.prominence1' '.prominence2' '.fitMethod$'
				removeObject: .trendRemoved
			endfor
		endfor
	endfor
	removeObject: .toneComplex, .powercepstrogram, .powercepstrum
	appendInfoLine: tab$, "test_trendSubtraction: OK"
endproc

procedure draw_powercepstrum
	.sound = Create Sound as tone complex: "t", 0, 1, 44100, "cosine",
		...	100, 0, 0, 0
	.powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
	.powercepstrum = To PowerCepstrum (slice): 0.1
	Erase all
	Colour: "Black"
	Select outer viewport: 0, 5, 0, 4
	Draw: 0, 0, 0, 0, "yes"
	Colour: "Green"
	Draw trend line: 0, 0, 0, 0, 0.001, 0, "Exponential decay", "Robust"
	Colour: "Red"
	Draw trend line: 0, 0, 0, 0, 0.001, 0, "Straight", "Least squares"
	Colour: "Black"
	removeObject: .sound, .powercepstrogram, .powercepstrum
endproc 

procedure testPowerCepstrumSmoothing: .info$
	appendInfoLine: tab$, tab$,"test smoothing PowerCepstrogram of noisy sound: ", .info$
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
	appendInfoLine: tab$, tab$,"test smoothing of PowerCepstrogram OK"
endproc

procedure testPowerCepstrogramSmoothing
	appendInfoLine: tab$, "test PowerCepstrogram and PowerCepstrum smoothing"
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

procedure test_perfectCosine: .f0
	appendInfo: tab$, tab$, "Cosine (2*pi*x/", .f0, ") in Spectrum"
	#
	# Suppose a Spectrum has only real values x[i] then the display of this 
	# spectrum in dB is S(f) = 10*log10 (2*df*x[i]^2/4e-10), for i=2..n-1 and 10*log10 (df*x[i]^2/4e-10) for i=1 & i=n
	# We want the Spectrum display in dB, S(f), to look like a perfect cos.
	#		What are the underlying x[i] ?
	#	S(f) only has real values.
	# 	S(f[i]) = offset + amplitude * cos(2*pi*f[i]/pitch) ; on a dB scale
	#			
	#   S(f[i]) = 10 log10 (d*x[i]^2*df/4e-10), where d=2 for i=2..n-1, d=1 for i=1 & i=n
	#			
	#		S(f[i]) = 10 log10((x[i]/2e-5)^2 * 2 *df),
	#						= 20 log10((x[f]/2e-5) + 10 log10(2*df) i=2..n-1
	#			b2 = 10 log10(2*df), b1 = 10 log10(df)
	#	20 log10((x(f)/2e-5) = S(f[i]) - b2
	# 	X(f)/2e-5 = 10^((S(f) - b2)/20)
	#		X(f) = 2e-5 * 10^((S(f) - b2)/20)
	.amplitude = 1
	.offset = 0
	.duration = 1
	.sound = Create Sound from formula: "s", 1, 0.0, 1, 44100, ~ .offset + .amplitude * cos(2*pi*.f0*x)
	.spectrum = To Spectrum: "yes"
	.spectrum2 = Copy: "c"
	@fillSpectrumWithCosine: .spectrum2, .f0
	.pc = To PowerCepstrum
	.qAtPeak = Get quefrency of peak: 1, 330.0, "parabolic"
	.qtoF0 = 1.0 / .qAtPeak
	assert abs(.qtoF0 - .f0)/ .f0 < 0.005; q: '.qtoF0' , '.f0'
	appendInfoLine: " OK"
	removeObject: .sound, .spectrum, .spectrum2, .pc  
endproc

procedure calibratedPowerCepstrumSmoothing: .duration
	appendInfo: tab$, "Calibrated smoothing (", .duration, ") s: "
	.sound = Create Sound from formula: "s", 1, 0.0, .duration, 10000, ~ 0
	.spectrum = To Spectrum: "yes"
	.pc = To PowerCepstrum
	.dq = Get quefrency step
	.nbins = Get number of quefrency bins
	.value = 1000000.0
	Formula: ~ .value
	for .ismooth to 5
		selectObject: .pc
		.smoothing = ((.ismooth - 1) * 2 + 0.3) * .dq
		.smoothed = Smooth: .smoothing, 4
		for .i to .nbins
			.rdif1 = abs (object[.smoothed, 1, .i] - .value) / .value
			assert .rdif1 < 1e-13 ; 1: '.rdif1' smooting = '.smoothing'
		endfor
		removeObject: .smoothed
	endfor
	selectObject: .pc
	# y = a x + b
	# y[1] = v; y[n] = 2*v
	# b = v; a = v/qend
	.qend = Get end quefrency
	.b = .value
	.a = .value / .qend
	Formula: ~  .a * x + .b
	.smooth2 = Smooth: 0.005, 1
	.error = 0.007 / .duration
	for .i to .nbins
		.rdif3 = abs (object[.smooth2, 1, .i] - object[.pc, 1, .i]) / object[.pc, 1, .i]; '.rdif'
		assert .rdif3 < .error; 3: '.rdif3'
	endfor
	removeObject: .sound, .spectrum, .pc, .smooth2
	appendInfoLine: "OK"
endproc

#
# Placing a peak exactly in the middle of one bin in the PowerCepstrum and setting the 
# rest of the bins to a constant background value makes it easy
# to check if the peak picking and the smoothing are done correctly.
#	Instead of the bin index to place the peak we use an approximate F0.
# The quefrency of this approximateF0 will be quefrency = 1 / approximateF0.
# The index of this quefrency can be queried by index_real = Get index from quefrency: quefrency
# This index will be rounded to an integer value and its value will be set to 'peak_dB' 
# while the other values will be set to 'background_dB'
# Therefore, if we query this PowerCepstrum for its peak value we should exactly get 'peak_dB',
# a query to the quefrency of the peak should exacly give the quefrency at the bin index,
# The CPP value should exactly give peak_dB - background_dB.
#
# The underlying values in the PowerCepstrum are amplitudes x[i]^2, however if we draw 
# a PowerCepstrum these values are displayed in dB's as 10 * log10 (x[i]^2). 
# The real values in the peak bins are therefore:
#			peak = 10^(peak_dB/10) 
#			background = 10^(background_dB/10)
# 
# After smoothing with a window 2*dq wide, the peak is now three bins wide (dq is the bin 
#	width of the PowerCeptrum).
#	The values in the peak bin and in its left and right neihbour are
#		peak1  = (0.5*background + peak + 0.5*background)/2
#		left1  = (0.5*background + background + 0.5*peak)/2  
# 	right1 = (0.5*peak + background + 0.5*background)/2 (left and right are equal)
#		cpp1 = 10*log10 (peak1) - background_dB
# The samples left and right from the peak will be pairwise equal.
# After a second smoothing with the same window the peak will be 5 bins wide
#		peak2 = (0.5*left1 + peak + 0.5*right1)/2 = (peak + left)/2
#		cpp2 = 10*log10 (peak2) - background_dB
# A smoothing with 3*dq gives a peak with value
#		peak3 = (left + peak + right) /3
#		cpp3 = 10*log10 (peak3) - background_dB
# Etc. The process can be generalized but we will only test the three smoothings mentioned.
# 
procedure testCalibratedPowerCepstrumCPP: .duration, .approximateF0, .peak_dB, .background_dB
	appendInfoLine: tab$, tab$, "testCalibratedPowerCepstrumCPP: Duration: ", .duration, " s, appr. F0 = ", fixed$(.approximateF0, 2), ","
	appendInfoLine: tab$, tab$, tab$, "peak= ", fixed$ (.peak_dB, 2), " dB, background = ", .background_dB, " dB" 
	.sound = Create Sound from formula: "s", 1, 0.0, .duration, 10000, ~ 0
	.spectrum = To Spectrum: "yes"
	.pc = To PowerCepstrum
	.dq = Get quefrency step
	.nbins = Get number of quefrency bins
	.peak = 10^(.peak_dB/10)
	.background = 10^(.background_dB/10)
	.peakIndex_real = Get index from quefrency: (1.0 / .approximateF0)
	.peakIndex = round (.peakIndex_real)
	.qindex = Get quefrency from index: .peakIndex
	.actualF0 = 1.0 / .qindex
	Formula: ~ if col = .peakIndex then .peak else .background fi
	@powercepstrum_assertions: .pc, .peakIndex, .peak, .background_dB, "Basic"

	#
	# Smoothing with a window 2*dq wide, the background wil stay at the same level
	# only the peak will be widened to three bins
	# 
	.smooth1 = Smooth: 2*.dq, 1
	.peak1 = 0.5*(.peak+.background)
	.left1  = 0.5 * (1.5*.background + 0.5*.peak) 
	@powercepstrum_assertions: .smooth1, .peakIndex, .peak1, .background_dB, "Smoothing 2*dq (first)"

	#
	# Smoothing with a window 2*dq wide of the already once the smoothed powercepstrum, 
	# the background wil stay at the same level, only the peak will be widened to 5 bins
	# 
	.smooth2 = Smooth: 2*.dq, 1
	.peak2 = 0.5*(.peak1+.left1)
	@powercepstrum_assertions: .smooth2, .peakIndex, .peak2, .background_dB, "Smoothing 2*dq (second)"

	#
	# Smoothing with a window 2.5*dq wide, the background wil stay at the same level
	# only the peak will be widened to 2 bins
	#

	selectObject: .pc
	.smooth25 = Smooth: 2.5*.dq, 1
	.peak25 = (.peak + 1.5*.background) / 2.5
	@powercepstrum_assertions: .smooth25, .peakIndex, .peak25, .background_dB, "Smoothing 2.5*dq"

	#
	# Smoothing with a window 3*dq wide, the background wil stay at the same level
	# only the peak will be widened to 5 bins
	#

	selectObject: .pc
	.smooth3 = Smooth: 3*.dq, 1
	.peak3 = (.peak + 2.0*.background) / 3
	@powercepstrum_assertions: .smooth3, .peakIndex, .peak3, .background_dB, "Smoothing 3*dq"


	removeObject: .sound, .spectrum, .pc, .smooth1, .smooth2, .smooth25, .smooth3
	appendInfoLine: tab$, tab$, "testCalibratedPowerCepstrumCPP OK"
endproc

procedure powercepstrum_assertions: .pc, .peakIndex, .peak, .background_dB, .info$
	appendInfoLine: tab$, tab$, .info$
	selectObject: .pc
	.qindex = Get quefrency from index: .peakIndex
	.queriedPeak_dB = Get peak: 60, 350, "parabolic"
	.qpeak = Get quefrency of peak: 60, 350, "parabolic"
	.qmax = Get end quefrency
	.cpp = Get peak prominence: 60, 350, "parabolic", 0.0001, .qmax, "Exponential decay", "Robust"
	.peak_dB = 10 * log10 (.peak)
	.cppGiven = .peak_dB - .background_dB
	.rdif_dB = abs (.peak_dB - .queriedPeak_dB) / .peak_dB
	assert .rdif_dB  < 1e-14; peak '.rdif_dB'
	.rdif_q = abs (.qpeak - .qindex) / .qpeak
	assert .rdif_q  < 1e-14; q '.rdif_q'
	.rdif_cpp = abs (.cpp - .cppGiven)/.cppGiven
	assert .rdif_cpp  < 1e-14; cpp '.rdif_cpp'	
endproc

procedure testCalibratedPowerCepstrogram: .duration, .approximateF0, .peak_dB, .background_dB
	appendInfoLine: tab$, "Calibrated Cepstrogram of ", .duration, "s, f0: ",.approximateF0, " Hz, peak: ",
	... .peak_dB, " dB, background: ", .background_dB, " dB"
	.s = Create Sound from formula: "s", 1, 0, .duration, 44100, ~0
	.powercepstrogram = To PowerCepstrogram: 60, 0.002, 5000, 50
	.numberOfFrames = Get number of frames
	.powercepstrum = To PowerCepstrum (slice): 0.1
	.qmax = Get end quefrency
	.dq = Get quefrency step
	.nbins = Get number of quefrency bins
	.peak = 10^(.peak_dB / 10)
	.background = 10^(.background_dB / 10)
	.peakIndex_real = Get index from quefrency: (1.0 / .approximateF0)
	.peakIndex = round (.peakIndex_real)
	.qindex = Get quefrency from index: .peakIndex
	.actualF0 = 1.0 / .qindex
	Formula: ~ if col = .peakIndex then .peak else .background fi
	selectObject: .powercepstrogram
	Formula: ~ object [.powercepstrum, 1, row]
	#
	# no smoothing, there is just one peak
	#
	.cppsGiven1 = .peak_dB - .background_dB
	.timeAveragingWindow = 0.0
	.quefrencyAveragingWindow = 0.0
	appendInfoLine: tab$, tab$, "Time window: ", .timeAveragingWindow, ", Quefrency window: dq, CPPS: ", .cppsGiven1
	.cpps1 = Get CPPS: "yes", .timeAveragingWindow, .quefrencyAveragingWindow, 60, 350, 0.05, "parabolic",
		... 0.001, 0.005, "Exponential decay", "Robust" 
	.rdif1 = abs (.cppsGiven1 - .cpps1)/.cppsGiven1
	assert .rdif1 < 1e-14; CPPS '.rdif1'
	#
	# Smoothing with a window 1*dq wide, the background will stay at the same level
	# only the peak will be widened to three bins
	# 
	.timeAveragingWindow = 0.005 ; should not make any difference
	.quefrencyAveragingWindow = .dq
	.cppsGiven2 = .peak_dB - .background_dB
	appendInfoLine: tab$, tab$, "Time window: ", .timeAveragingWindow, ", Quefrency window: dq, CPPS: ", .cppsGiven2
	.cpps2 = Get CPPS: "yes", .timeAveragingWindow, .quefrencyAveragingWindow, 60, 350, 0.05, "parabolic",
		... 0.001, 0.005, "Exponential decay", "Robust" 
	.rdif2 = abs (.cppsGiven2 - .cpps2)/.cppsGiven2
	assert .rdif2 < 1e-14; CPPS '.rdif2'
	#
	# Smoothing with a window 4*dq wide, the background wil stay at the same level
	# only the peak will be widened but the peak will be at the center so parabolic interpolation 
	# will go through the peak!
	# 
	.quefrencyAveragingWindow = 4.0 * .dq
	.peak3 = (.peak + 3.0 * .background) / 4.0
	.peak_dB3 = 10 * log10 (.peak3)
	.cppsGiven3 = .peak_dB3 - .background_dB
	appendInfoLine: tab$, tab$, "Time window: ", .timeAveragingWindow, ", Quefrency window: 4*dq, CPPS: ", .cppsGiven3
	.cpps3 = Get CPPS: "yes", .timeAveragingWindow, .quefrencyAveragingWindow, 60, 350, 0.05, "parabolic",
		... 0.001, 0.005, "Exponential decay", "Robust"
	.rdif3 = abs (.cppsGiven3 - .cpps3) / .cppsGiven3
	assert .rdif3 < 1e-14; CPPS '.rdif3'

	for .ismooth to 0
		.quefrencyAveragingWindow = randomUniform (0.0006, 0.0003)
		selectObject: .powercepstrogram
		# smooth only in y-direction
		.smoothcepstrogram = Smooth: 0.0000001, .quefrencyAveragingWindow
		.smoothpowercepstrum = To PowerCepstrum (slice): 0.1
		.cpps = Get peak prominence: 60, 350, "parabolic", 0.001, .qmax, "Straight", "Robust slow"
		selectObject: .powercepstrum
		.smoothpowercepstrum2 = Smooth: .quefrencyAveragingWindow, 1
		.cpps2 = Get peak prominence: 60, 350, "parabolic", 0.001, .qmax, "Straight", "Robust slow"
		assert .cpps == .cpps2 ;
		removeObject: .smoothcepstrogram, .smoothpowercepstrum, .smoothpowercepstrum2
	endfor
	removeObject: .s, .powercepstrum, .powercepstrogram
	appendInfoLine: tab$, "Calibrated Cepstrogram: OK"
endproc

procedure fillSpectrumWithCosine: .spectrum, .f0
	selectObject: .spectrum
	.df = Get bin width
	.b2 = 10*log10(2*.df)
	.s = 2e-5
	.amplitude = 1
	.offset = 0
	Formula: ~  if row = 1 then .s * 10^((.offset + .amplitude * cos(2 * pi * x / .f0) -.b2)/20) else 0 fi
endproc

;@timingComparison
procedure timingComparison
	.sound = Create Sound as tone complex: "toneComplex", 0, 1, 44100, "cosine", 100, 0, 0, 0
	.pc = To PowerCepstrogram: 60, 0.002, 5000, 50
	# version time CPPS
	# 6.1.55 14.497 29.0736
	# 6.2.14 14.621 29.0736
	# 6.4.01  4.684 29.0719 ??
	# 6.4.23 15.200 29.0719
	# 6.4.27 15.180 29.0582

	stopwatch
	for i to 1
		.cpps = Get CPPS: "yes", 0.02, 0.0005, 60, 330, 0.05, "parabolic", 0.001, 0.05,
		... "Exponential decay", "Robust slow"
	endfor
	writeInfoLine: fixed$ (stopwatch, 3), " ", .cpps
endproc
	




