writeInfoLine: "Sound_to_LPC:"

sound = Read from file: "../fon/logicalVersusPhysical.Sound"
lpc = noprogress To LPC (burg): 16, 0.025, 0.005, 50.0
numberOfFrames = Get number of frames
assert numberOfFrames = 190 or numberOfFrames = 191   ; 'numberOfFrames'
removeObject: sound, lpc

numberOfFailures = 0
for i to 30
	duration = randomUniform (0.002, 0.020)
	windowDuration = randomUniform (0.001, 0.010)
	appendInfo: i, " ", duration, " ", windowDuration
	samplingFrequency = randomUniform (16000, 96000)
	sound = Create Sound from formula: "test", 1, 0, duration, samplingFrequency, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
	if duration < 2 * windowDuration
		asserterror Your sound is shorter than two window lengths. Either your sound is too short or your window is too long.
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