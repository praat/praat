# resamplingDepth.praat
# Paul Boersma, 28 October 2024

sound50k = Read from file: "examples/sounds/aaaa02.wav"
sound4k = Resample: 4000, 700
;Play
duration = Get total duration

depths# = { 700, 50, 20, 10, 6, 3, 2, 1 }

Erase all
Times
12

for idepth to size (depths#)
	depth = depths# [idepth]
	select sound4k
	stopwatch
	sound44k = Resample: 44100, depth
	speed$ = fixed$ (stopwatch / duration * 100, 3)
	;Play
	To Spectrum: "no"
	leak = Get band energy difference: 0, 2000, 2000, 22050
	Remove
	select sound44k
	To Spectrogram: 0.030, 10000, 0.002, 20.0, "Gaussian"
	col = (idepth - 1) div 4 + 1
	row = (idepth - 1) mod 4 + 1
	Select outer viewport: 6 * (col - 1), 6 * col, 3 * (row - 1), 3 * row
	Paint: 0, 0, 0, 0, 100.0, "yes", 100.0, 6.0, 0.0, "yes"
	Text top: "no", "depth " + string$ (depth) + ": " + speed$ + "\% , " + fixed$ (leak, 3) + " dB"
	Red
	Draw line: 0, 2000, duration, 2000
	Remove
	removeObject: sound44k
endfor
removeObject: sound4k, sound50k
Select outer viewport: 0, 12, 0, 12
