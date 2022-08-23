# time/fourier.praat
# Paul Boersma 2022-01-08

nmin = 2^19-30
nmax = 2^19+30

for power from 1 to 30
	@measure: 2^power - 1
endfor

procedure measure: .n
	.sound = Create Sound from formula: "sound", 1, 0, 1, .n, ~ 0
	.numberOfReplications = ceiling (100000 / nmax)
	stopwatch
	for .irep to .numberOfReplications
		select .sound
		.spectrum = To Spectrum (resampled): 50
		Remove
	endfor
	.duration_nanoseconds = stopwatch / .numberOfReplications * 10^9
	.nflops = .n * log2 (.n)
	.nanoseconds_per_flop = .duration_nanoseconds / .nflops
	appendInfoLine: .n, " ", fixed$ (.nanoseconds_per_flop, 2)
	removeObject: .sound
endproc