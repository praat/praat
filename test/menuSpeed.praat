pitch = Create TextGrid: 0, 1, "Mary John bell", "bell"
sound = Create Sound as pure tone: "tone", 1, 0, 0.4, 44100, 440, 0.2, 0.01, 0.01
intens = To Intensity: 100, 0, "yes"
Down to IntensityTier
To AmplitudeTier
selectObject: sound
;harm = To Harmonicity (cc): 0.01, 75, 0.1, 1
stopwatch
for i to 1000000
	x = i
endfor
t1 = stopwatch
stopwatch
for i to 1000000
	x = Get duration
endfor
t2 = stopwatch
writeInfoLine: round ((t2 - t1) * 1000), " nanoseconds"
removeObject: pitch, sound, intens, "IntensityTier tone", "AmplitudeTier tone"