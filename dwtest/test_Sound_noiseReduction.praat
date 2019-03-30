# test_Sound_noiseReduction.praat
# djmw 20190326

appendInfoLine: "test_Sound_noiseReduction.praat"
sound = Create Sound from formula: "test", 1, 0, 1, 44100, "0.0"
Formula: "randomGauss(0,0.1) + if x < 0.5 then 0.5*sin(2*pi*377*x) else 0 fi"
spectrum = To Spectrum: "no"
bandEnergy = Get band energy: 1000, 9000
for i to 10
	db = -10 * (i - 1)
	selectObject: sound
	copy = Copy: string$ (db)
	soundReduced = Reduce noise: 0.5, 1, 0.025, 80, 10000, 40, db, "spectral-subtraction"
	spectrumReduced = To Spectrum: "no"
	bandEnergyReduced = Get band energy: 1000, 9000
	appendInfoLine: tab$, "db=", db, " ", 10* log10 (bandEnergyReduced/4e-10), " ", 10* log10 (bandEnergyReduced / bandEnergy), " ", bandEnergy / bandEnergyReduced
	removeObject: spectrumReduced, soundReduced, copy
endfor
removeObject: spectrum, sound
appendInfoLine: "test_Sound_noiseReduction.praat OK"
