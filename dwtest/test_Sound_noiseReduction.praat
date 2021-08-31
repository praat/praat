# test_Sound_noiseReduction.praat
# djmw 20190326, 20210831

appendInfoLine: "test_Sound_noiseReduction.praat"


@ test_reduction
@test_numberOfBands: 10

appendInfoLine: "test_Sound_noiseReduction.praat OK"

procedure test_reduction
	appendInfoLine: tab$, "test_reduction"
	.sound = Create Sound from formula: "test", 1, 0, 1, 44100, "0.0"
	Formula: "randomGauss(0,0.1) + if x < 0.5 then 0.5*sin(2*pi*377*x) else 0 fi"
	.spectrum = To Spectrum: "no"
	.bandEnergy = Get band energy: 1000, 9000
	for .i to 10
		.db = -10 * (.i - 1)
		selectObject: .sound
		.copy = Copy: string$ (.db)
		.soundReduced = Reduce noise: 0.5, 1, 0.025, 80, 10000, 40, .db, "spectral-subtraction"
		.spectrumReduced = To Spectrum: "no"
		.bandEnergyReduced = Get band energy: 1000, 9000
		appendInfoLine: tab$, tab$, "db=", .db, " ", 10* log10 (.bandEnergyReduced/4e-10), " ", 
		... 10* log10 (.bandEnergyReduced / .bandEnergy), " ", .bandEnergy / .bandEnergyReduced
		removeObject: .spectrumReduced, .soundReduced, .copy
	endfor
	removeObject: .spectrum, .sound
	appendInfoLine: tab$, "test_reduction OK"
endproc

procedure test_numberOfBands: .numberOfTries
	appendInfoLine: tab$, "test_numberOfBands"
	for .i to .numberOfTries
		.samplingFrequency = randomUniform(1000, 50000)
		.sound = Create Sound from formula: "test", 1, 0, 1, .samplingFrequency, "0.0"
		.windowLength = randomUniform (0.005,0.06)
		.soundReduced = Reduce noise: 0.5, 1, .windowLength, 80, 10000, 40, -20, "spectral-subtraction"
		appendInfoLine: tab$, tab$, "Fmax = ", .samplingFrequency, " windowLength = ", .windowLength
		removeObject: .sound, .soundReduced
	endfor
	appendInfoLine: tab$, "test_numberOfBands OK"
endproc