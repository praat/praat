# fourier.praat
# Paul Boersma, May 9, 2004
# Checks "Sound: To Spectrum..."

echo Fourier test
printline Low sampling frequencies: 
for itest from 1 to 50
   call test 1 itest
endfor
printline
printline Random sampling frequencies:
call test 1 22049
call test 1 22050
for itest to 48
	duration = randomUniform (0.2, 3.0)
	samplingFrequency = randomInteger (10, 10000)
	call test duration samplingFrequency
endfor
printline
printline Fourier test SUCCEEDED.

procedure test duration samplingFrequency
	print .
	Create Sound... sound 0 duration samplingFrequency randomGauss (0, 1)
	energy1 = Get energy... 0 0
	To Spectrum (fft)
	Rename... fft
	energy2 = Get band energy... 0 0
	assert abs (energy1 - energy2) < 1e-6 ; 'duration' 'samplingFrequency'
	To Sound (fft)
	energy3 = Get energy... 0 0
	assert abs (energy3 - energy1) < 1e-6 ; 'duration' 'samplingFrequency'
	Formula... self - Sound_sound [ ]
	extremum = Get absolute extremum... 0 0 None
	assert extremum < 1e-6 ; 'duration' 'samplingFrequency'
	plus Spectrum fft
	Remove
	select Sound sound
	To Spectrum (dft)
	Rename... dft
	energy2 = Get band energy... 0 0
	assert abs (energy1 - energy2) < 1e-6 ; 'duration' 'samplingFrequency' 'energy1' 'energy2'
	To Sound (fft)
	energy3 = Get energy... 0 0
	assert abs (energy3 - energy1) < 1e-6 ; 'duration' 'samplingFrequency' 'energy1' 'energy3'
	Formula... self - Sound_sound [ ]
	extremum = Get absolute extremum... 0 0 None
	assert extremum < 1e-4 ; 'duration' 'samplingFrequency'
	plus Spectrum dft
	plus Sound sound
	Remove
endproc
