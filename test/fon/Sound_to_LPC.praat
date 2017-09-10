# test/fon/Sound_to_LPC.praat
# Paul Boersma, 22 November 2011

for i to 30
	duration = randomUniform (0.001, 0.003)
	windowDuration = randomUniform (0.002, 0.005)
	samplingFrequency = randomUniform (16000, 96000)
	sound = Create Sound from formula: "test", 1, 0, duration, samplingFrequency, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
	noprogress To LPC (burg): 16, windowDuration, 0.005, 50
	plus sound
	Remove
endfor 
