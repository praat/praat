# PitchTier.praat
# Paul Boersma 2015-12-21

writeInfoLine: "PitchTier test"

tier = Create PitchTier: "tier", -1.0, 1.8
Add point: 0.5, 150.0
Add point: 0.6, 178.0
Add point: 0.6, 180.0   ; shouldn't do anything
Add point: 0.3, 200.0   ; should be inserted as the first point

#
# Check time domain.
#
tmin = Get starting time
assert tmin = -1.0
tmax = Get end time
assert tmax = 1.8
duration = Get duration
assert duration = 2.8

n = Get number of points
assert n = 3

#
# Check values and sorting.
#
for i to n
	time [i] = Get time from index: i
	pitch [i] = Get value at index: i
endfor
assert time [1] = 0.3
assert time [2] = 0.5
assert time [3] = 0.6
assert pitch [1] = 200.0
assert pitch [2] = 150.0
assert pitch [3] = 178.0

removeObject: tier
appendInfoLine: "OK"
