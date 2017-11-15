# test_TextGrid_and_DurationTier.praat

# check the interface 

# Create a TextGrid

appendInfoLine: "test_TextGrid_and_DurationTier.praat"

for i to 10
	f = randomUniform (100, 400)
	sound[i] = Create Sound as pure tone: string$ (f), 1, 0, 0.4, 44100, f, 0.2, 0.01, 0.01
endfor

selectObject: sound[1]
for i from 2 to 10
	plusObject: sound[i]
endfor

Concatenate recoverably
sound = selected ("Sound")
textgrid = selected ("TextGrid")

for i to 10
	removeObject: sound[i]
endfor

# some duration manipulations
selectObject: sound
duration = Get total duration
durationTier = Create DurationTier: "chain", 0, duration

selectObject: textgrid
numberOfIntervals = Get number of intervals: 1

deltaTime = 0.0000001
for i to numberOfIntervals
	if i mod 2 = 0
		selectObject: textgrid
		t1 = Get start time of interval: 1, i
		t2 = Get end time of interval: 1, i
		selectObject: durationTier
		Add point: t1, 1
		Add point: t1+deltaTime, 1.9
		Add point: t2-deltaTime, 1.9
		Add point: t2, 1
	endif
endfor

selectObject: durationTier, textgrid
textgrid2 = To TextGrid (scale times)

selectObject: sound
manipulation = To Manipulation: 0.01, 75, 600
plusObject: durationTier
Replace duration tier
selectObject: manipulation
sound2 = Get resynthesis (overlap-add)

removeObject: sound, sound2, textgrid, textgrid2, manipulation, durationTier

appendInfoLine: "test_TextGrid_and_DurationTier.praat OK"
