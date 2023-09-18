# test/fon/Sound_to_LPC.praat
# Paul Boersma, 22 November 2011
# 17 September 2023: disallow too-short windows

writeInfoLine: "Sound_to_LPC:"
numberOfFailures = 0
for i to 30
	duration = randomUniform (0.002, 0.020)
	windowDuration = randomUniform (0.001, 0.010)
	appendInfo: i, " ", duration, " ", windowDuration
	samplingFrequency = randomUniform (16000, 96000)
	sound = Create Sound from formula: "test", 1, 0, duration, samplingFrequency, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
	if duration < 2 * windowDuration
		asserterror Your sound is too short: it should be at least as long as two window lengths.
		noprogress To LPC (burg): 16, windowDuration, 0.005, 50
		numberOfFailures += 1
		appendInfoLine: "   failure"
	else
		noprogress To LPC (burg): 16, windowDuration, 0.005, 50
		appendInfoLine: "   success"
		Remove
	endif
	removeObject: sound
endfor
appendInfoLine: numberOfFailures, " failures"
appendInfoLine: "OK"