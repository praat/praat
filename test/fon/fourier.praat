# fourier.praat
# Paul Boersma, 2004,2025
# Checks "Sound: To Spectrum..."

writeInfoLine: "Fourier test"
appendInfoLine: "Low sampling frequencies:"
for itest from 1 to 50
   @test: 1, itest
endfor
appendInfoLine()
appendInfoLine: "Random sampling frequencies:"
@test: 1, 22049
@test: 1, 22050
for itest to 48
	duration = randomUniform (0.2, 3.0)
	samplingFrequency = randomInteger (10, 10000)
	@test: duration, samplingFrequency
endfor
appendInfoLine()
appendInfoLine: "Fourier test SUCCEEDED."

procedure test: duration, samplingFrequency
	appendInfo: "."
	original.Sound = Create Sound: "sound", 0.0, duration, samplingFrequency, ~ randomGauss (0, 1)
	Shift times to between zero and physical duration
	energy1 = Get energy: 0, 0
	fft.Spectrum = To Spectrum: "yes"
	energy2 = Get band energy: 0, 0
	assert abs (energy1 - energy2) < 1e-6 ; 'duration' 'samplingFrequency'
	loop.Sound = To Sound
	energy3 = Get energy: 0, 0
	assert abs (energy3 - energy1) < 1e-6 ; 'duration' 'samplingFrequency'
	Formula: ~ self - Sound_sound [ ]
	extremum = Get absolute extremum: 0, 0, "none"
	assert extremum < 1e-6 ; 'duration' 'samplingFrequency'
	removeObject: fft.Spectrum, loop.Sound
	selectObject: original.Sound
	dft.Spectrum = To Spectrum: "no"
	energy2 = Get band energy: 0, 0
	assert abs (energy1 - energy2) < 1e-6 ; 'duration' 'samplingFrequency' 'energy1' 'energy2'
	loop.Sound = To Sound
	energy3 = Get energy: 0, 0
	assert abs (energy3 - energy1) < 1e-6 ; 'duration' 'samplingFrequency' 'energy1' 'energy3'
	Formula: ~ self - Sound_sound [ ]
	extremum = Get absolute extremum: 0, 0, "none"
	assert extremum < 1e-4 ; 'duration' 'samplingFrequency'
	removeObject: loop.Sound, dft.Spectrum, original.Sound
endproc
