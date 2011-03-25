# pitch.praat
# Paul Boersma, December 10, 2007
# Tests "Sound: To Pitch..."

echo Pitch test

method$ = "cc"

for freq from 76.1 to 106
	call sineTest freq 0.1
endfor
for freq from 106.1 to 226
	call sineTest freq 0.01
endfor
for i from 2 to 20
	call sineTest i*100+77 0.02
endfor
for i from 21 to 29
	call sineTest i*100+77 0.1
endfor
for i from 30 to 35
	call sineTest i*100+77 1.0
endfor
for i from 36 to 60
	call sineTest i*100+77 5.0
endfor
for i from 61 to 107
	call sineTest i*100+77 10.0
endfor
for i from 1080 to 1095
	call sineTest i*10-0.1 10.0
endfor

for freq from 76.1 to 136
	call pulseTest freq 0.1
endfor
for freq from 136.1 to 226
	call pulseTest freq 0.2
endfor
for i from 2 to 13
	call pulseTest i*100+77 1
endfor
for i from 14 to 20
	call pulseTest i*100+77 2
endfor
for i from 210 to 219
	call pulseTest i*10+77 10
endfor
for i from 22 to 30
	call pulseTest i*100+77 2
endfor
for i from 31 to 53
	call pulseTest i*100+77 5
endfor
call pulseTest 219 1
call pulseTest 220 1

method$ = "ac"

for freq from 75.1 to 106
	call sineTest freq 0.1
endfor
for freq from 106.1 to 226
	call sineTest freq 0.01
endfor
for i from 2 to 105
	call sineTest i*100+77 0.02
endfor
for i from 106 to 107
	call sineTest i*100+77 0.1
endfor
for i from 1080 to 1095
	call sineTest i*10-0.1 0.3
endfor

for freq from 75.1 to 226
	call pulseTest freq 0.03
endfor
for i from 2 to 20
	call pulseTest i*100+77 1
endfor
for i from 210 to 219
	call pulseTest i*10+77 4
endfor
for i from 22 to 53
	call pulseTest i*100+77 1
endfor
call pulseTest 219 1
call pulseTest 220 1
# Tests above 5512.5 Hz are superfluous,
# since pulses are no different from since waves then.

printline Pitch test finished OK

procedure sineTest pitch precision
	Create Sound... sound 0 1 22050 sin (2 * pi * pitch * x)
	call analyse precision
endproc

procedure pulseTest pitch precision
	Create PitchTier... sound 0 1
	Add point... 0.5 pitch
	To Sound (pulse train)... 22050 1 1e-9 2000 no
	call analyse precision
	select PitchTier sound
	Remove
endproc

procedure analyse precision
	;To Pitch... 0.09457464735 75 11025
	noprogress To Pitch ('method$')... 0.09457464735 75 15 no 0.03 0.45 0.03 0.35 0.14 11025
	minPitch = Get minimum... 0 0 Hertz None
	maxPitch = Get maximum... 0 0 Hertz None
	diff1 = minPitch - pitch
	diff2 = maxPitch - pitch
	absDiff = max (abs (diff1), abs (diff2))
	relDiff = absDiff / pitch
	printline 'pitch' 'diff1:6' 'diff2:6' 'relDiff:3%'
	assert absDiff < precision ; 'pitch' 'minPitch' 'maxPitch'
	plus Sound sound
	Remove
endproc
