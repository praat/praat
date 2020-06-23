# pitch.praat
# Paul Boersma 2020-01-02
# Tests "Sound: To Pitch..."

writeInfoLine: "Pitch test"
stopwatch

method$ = "cc"

for freq from 76.1 to 106
	@sineTest: freq, 0.1
endfor
for freq from 106.1 to 226
	@sineTest: freq, 0.01
endfor
for i from 2 to 20
	@sineTest: i * 100 + 77, 0.02
endfor
for i from 21 to 29
	@sineTest: i * 100 + 77, 0.1
endfor
for i from 30 to 35
	@sineTest: i * 100 + 77, 1.0
endfor
for i from 36 to 60
	@sineTest: i * 100 + 77, 5.0
endfor
for i from 61 to 107
	@sineTest: i * 100 + 77, 10.0
endfor
for i from 1080 to 1095
	@sineTest: i * 10 - 0.1, 10.0
endfor

for freq from 76.1 to 136
	@pulseTest: freq, 0.1
endfor
for freq from 136.1 to 226
	@pulseTest: freq, 0.2
endfor
for i from 2 to 13
	@pulseTest: i * 100 + 77, 1
endfor
for i from 14 to 20
	@pulseTest: i * 100 + 77, 2
endfor
for i from 210 to 219
	@pulseTest: i * 10 + 77, 10
endfor
for i from 22 to 30
	@pulseTest: i * 100 + 77, 2
endfor
for i from 31 to 53
	@pulseTest: i * 100 + 77, 5
endfor
@pulseTest: 219, 1
@pulseTest: 220, 1

method$ = "ac"

for freq from 75.1 to 106
	@sineTest: freq, 0.1
endfor
for freq from 106.1 to 226
	@sineTest: freq, 0.01
endfor
for i from 2 to 105
	@sineTest: i * 100 + 77, 0.02
endfor
for i from 106 to 107
	@sineTest: i * 100 + 77, 0.1
endfor
for i from 1080 to 1095
	@sineTest: i * 10 - 0.1, 0.3
endfor

for freq from 75.1 to 226
	@pulseTest: freq, 0.03
endfor
for i from 2 to 20
	@pulseTest: i * 100 + 77, 1
endfor
for i from 210 to 219
	@pulseTest: i * 10 + 77, 4
endfor
for i from 22 to 53
	@pulseTest: i * 100 + 77, 1
endfor
@pulseTest: 219, 1
@pulseTest: 220, 1
# Tests above 5512.5 Hz are superfluous,
# since pulses are no different from sine waves then.

appendInfoLine: "Pitch test finished OK ", stopwatch

procedure sineTest: pitch, precision
	sound = Create Sound: "sound", 0, 1, 22050, ~ sin (2 * pi * pitch * x)
	@analyse: precision
	removeObject: sound
endproc

procedure pulseTest: pitch, precision
	pitchTier = Create PitchTier: "sound", 0, 1
	Add point: 0.5, pitch
	sound = To Sound (pulse train): 22050, 1, 1e-9, 2000, "no"
	@analyse: precision
	removeObject: pitchTier, sound
endproc

procedure analyse: precision
	timeStep = 0.09457464735
	;timeStep = 0.001
	;To Pitch... timeStep 75 11025
	my.Pitch = noprogress To Pitch ('method$'): timeStep, 75, 15, "no", 0.03, 0.45, 0.03, 0.35, 0.14, 11025
	minPitch = Get minimum... 0 0 Hertz None
	maxPitch = Get maximum... 0 0 Hertz None
	diff1 = minPitch - pitch
	diff2 = maxPitch - pitch
	absDiff = max (abs (diff1), abs (diff2))
	relDiff = absDiff / pitch
	printline 'pitch' 'diff1:6' 'diff2:6' 'relDiff:3%'
	;assert absDiff < precision ; 'pitch' 'minPitch' 'maxPitch'
	removeObject: my.Pitch
endproc
