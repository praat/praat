writeInfoLine: "Testing `LongSound: Save as stereo WAV file`..."
@test: 27
@test: 60
@test: 110
@test: 120
@test: 180
@test: 205
appendInfoLine: "OK"

procedure test: duration
	appendInfoLine: duration, " seconds..."
	orig_float = Create Sound from formula: "sineWithNoise", 2, 0.0, duration, 44100,
	... ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
	nowarn Save as WAV file: "kanweg_orig.wav"
	removeObject: orig_float
	orig_16bit = Read from file: "kanweg_orig.wav"
	channel1 = Extract one channel: 1
	Save as WAV file: "kanweg_channel1.wav"
	Remove
	selectObject: orig_16bit
	channel2 = Extract one channel: 2
	Save as WAV file: "kanweg_channel2.wav"
	Remove
	channel1 = Open long sound file: "kanweg_channel1.wav"
	part = Extract part: 0.0, 1.0, "yes"
	channel2 = Open long sound file: "kanweg_channel2.wav"
	selectObject: channel1, channel2
	Save as stereo WAV file: "kanweg_stereo.wav"
	stereo = Read from file: "kanweg_stereo.wav"
	#
	# This failed in 6.1.07 and earlier but was corrected for 6.1.08.
	#
	assert objectsAreIdentical: stereo, orig_16bit ;   'duration'
	removeObject: stereo, orig_16bit
	selectObject: channel1
	partAfterWriting = Extract part: 0.0, 1.0, "yes"
	#
	# This would have failed in 6.1.07 if `Extract part` had used the internal buffer.
	#
	assert objectsAreIdentical: partAfterWriting, part ;   'duration'
	removeObject: channel1, channel2, part, partAfterWriting
endproc
