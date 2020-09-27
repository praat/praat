#
# test/num/interpolation.praat
# Paul Boersma 2020-09-24
#

writeInfoLine: "Interpolation, especially cubic..."
Erase all
Times
12
sound = Create Sound from formula: "fakerandom", 1, 0, 1, 10000, ~ cos (56426631 * x^2)
midTime = 0.5
timeDepth = 0.00026
top = -0.4
@drawValues: "nearest"
@drawValues: "linear"
@drawValues: "cubic"
@drawValues: "sinc70"
@drawValues: "sinc700"
Remove
appendInfoLine: "OK"

procedure drawValues: interpolation$
	bottom = top + 2.4
	Select outer viewport: 0, 6, top, bottom
	Draw: midTime - timeDepth, midTime + timeDepth, -1.1, 1.1, "no", "poles"
	Draw inner box
	Marks bottom every: 1.0, 0.0001, "yes", "yes", "yes"
	Marks left every: 1.0, 0.2, "yes", "yes", "yes"
	time = midTime - timeDepth
	while time <= midTime + timeDepth
		value = Get value at time: 1, time, interpolation$
		Paint circle (mm): "black", time, value, 0.5
		time += 1e-6
	endwhile
	peakInterpolation$ =
	... if interpolation$ = "nearest" then "none"
	... else if interpolation$ = "linear" then "parabolic"
	... else interpolation$ fi fi
	maximum = Get maximum: midTime - timeDepth / 2, midTime + timeDepth / 2, peakInterpolation$
	timeOfMaximum = Get time of maximum: midTime - timeDepth / 2, midTime + timeDepth / 2, peakInterpolation$
	Paint circle (mm): "red", timeOfMaximum, maximum, 2.0
	if peakInterpolation$ = interpolation$
		interpolatedValue = Get value at time: 1, timeOfMaximum, interpolation$
		appendInfoLine: interpolation$, ": peak at ", timeOfMaximum,
		... " s; peak height ", maximum, "; interpolated value ", interpolatedValue
		assert interpolatedValue = maximum
	endif
	top += 1.8
endproc
