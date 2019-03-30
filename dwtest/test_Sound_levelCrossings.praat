# test_Sound_levelCrossings.praat
# djmw 20190328

appendInfoLine: "test_Sound_levelCrossings.praat"

frequency = 100
# exactly 10 periods
endTime = 10 / frequency
sine = Create Sound from formula: "sine", 2, 0, endTime, 44100, "sin(2*pi*frequency*x)"
level = 0

# Corner cases
# time is outside left
time = Get nearest level crossing: 1, -1, level, "left"
assert time = undefined
# time is outside right
time = Get nearest level crossing: 1, 1, level, "left"
assert time = undefined
# level to high
time = Get nearest level crossing: 1, 0.05, 1.1, "left"
assert time = undefined
# level to low
time = Get nearest level crossing: 1, 0.05, -1.1, "left"
assert time = undefined
# no crossing to the left
time = Get nearest level crossing: 1, 1 / (4 * frequency), level, "left"
assert time = undefined
# no crossing to the right
time = Get nearest level crossing: 1, endTime - 1 / (4 * frequency), level, "right"
assert time = undefined

delta = 1e-5
for i to 9
	timeAtLevel = i  / (2 * frequency)
	delta = 1e-5
	timeLeft = Get nearest level crossing: 1, timeAtLevel +delta, level, "left"
	assert abs(timeAtLevel - timeLeft) < delta
	timeRight = Get nearest level crossing: 1, timeAtLevel - delta, level, "right"
	assert abs(timeAtLevel - timeRight) < delta
	timeNearest = Get nearest level crossing: 1, timeAtLevel - 2*delta, level, "nearest"
	assert abs(timeAtLevel - timeNearest) < delta
endfor



removeObject: sine

appendInfoLine: "test_Sound_levelCrossings.praat OK"