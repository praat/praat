# test_spectrogramTypes.praat
# djmw 20161014

appendInfoLine: "test_spectrogramTypes.praat"
appendInfoLine: tab$ + "read old type ""MelFilter"" and convert"
melf = Read from file: "old_type.MelFilter"
mels = To MelSpectrogram
removeObject: melf, mels

startTime = 0
endTime = 0.5

@testMelSpectrogramInterface

appendInfoLine: "test test_spectrogramTypes.praat OK"

procedure testMelSpectrogramInterface
	.tone = Create Sound as pure tone: "t", 1, startTime, endTime, 44100, 440, 0.5, 0.01, 0.01
	.mels = To MelSpectrogram: 0.015, 0.005, 100, 100, 0
	# 
	# Draw
	#
	Erase all
	Select outer viewport: 0, 4, 0, 3
	Draw frequency scale: 0, 0, 0, 0, "yes"
	Select outer viewport: 4, 8, 0, 3
	Paint image: 0, 0, 0, 0, 0, 0, "yes"
	Select outer viewport: 0, 4, 3, 6
	Draw triangular filter functions: 0, 0, "Mel", 0, 0, "no", 0, 0, "yes"
	Select outer viewport: 4, 8, 3, 6
	Draw spectrum at nearest time slice: (startTime + endTime)/2, 0, 0, 0, 0, "yes"
	#
	# query
	#
	@queryTimeDomain
	@queryTimeSampling
	@queryCommons
	Equalize intensities: 80
	.mfcc = To MFCC: 12
	selectObject: .mels
	.intensity = To Intensity
	selectObject: .mels
	.matrix = To Matrix: "yes"
	removeObject: .matrix, .intensity, .mfcc, .mels, .tone
endproc

procedure queryCommons
	.lowestFrequency = Get lowest frequency
	.highestFrequency = Get highest frequency
	assert .lowestFrequency < .highestFrequency; '.lowestFrequency' < '.highestFrequency'
	.numberOfFrequencies = Get number of frequencies
	.frequencyDistance = Get frequency distance
	.fim1 = 0
	for .i to .numberOfFrequencies
		.fi = Get frequency from row: .i
		assert .fim1 < .fi; '.fim1' < '.fi'
		.fim1 = .fi
	endfor
	for .i to 100
		.value = Get value in cell: randomUniform (startTime, endTime), randomInteger (1, .numberOfFrequencies)
	endfor
endproc

procedure queryTimeDomain
	.startTime = Get start time
	.endTime = Get end time
	.duration = Get total duration
endproc

procedure queryTimeSampling
	.numberOfFrames = Get number of frames
	.timeStep = Get time step
	.previousTime = startTime
	for .i to .numberOfFrames
		.time = Get time from frame number: .i
		assert .time > .previousTime; '.time' > '.previousTime'  
		.previousTime = .time
	endfor
	for .i to .numberOfFrames / 10
		.frameNumber = Get frame number from time: randomUniform (startTime, endTime)
	endfor
endproc
