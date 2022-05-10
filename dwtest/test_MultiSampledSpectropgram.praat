# test_MultiSampledSpectropgram.praat
# djmw 20220510

appendInfoLine: "test_MultiSampledSpectrogram.praat"
@test_GaborSpectrogram
@test_ConstantQLog2FSpectrogram
appendInfoLine: "test_MultiSampledSpectrogram.praat OK"

procedure test_GaborSpectrogram
	appendInfoLine: tab$, "GaborSpectrogram: windows:"
	.windows$# = { "rectangular", "triangular", "parabolic", "Hanning", "Hamming", "Gaussian1", "Kaiser1"  }
	.sound = Create Sound from formula: "s", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
	for iwindow to size (.windows$#)
		selectObject: .sound
		appendInfoLine: tab$, tab$, .windows$# [iwindow]
		.gabor = To GaborSpectrogram: 0, 40, 20, 1, .windows$# [iwindow]
		.resynthesis = To Sound
		Formula: ~ self - object [.sound]
		.extremum = Get absolute extremum: 0, 0, "none"
		assert .extremum < 1e-14
		removeObject: .gabor, .resynthesis
	endfor
	appendInfoLine: tab$, "GaborSpectrogram: bandwidth, stepSize"
	.numberOfAnalysis = 10
	.bandwidths# = randomUniform# (.numberOfAnalysis, 10, 500)
	for .ianal to .numberOfAnalysis
		selectObject: .sound
		.bandwidth = .bandwidths# [.ianal]
		.stepSize = randomUniform (1, 0.98 * .bandwidth)
		appendInfoLine: tab$, tab$, fixed$ (.bandwidth, 3), " ", fixed$ (.stepSize, 3)
		.gabor = To GaborSpectrogram: 0, .bandwidth, .stepSize, 1, "Hanning"
		.resynthesis = To Sound
		Formula: ~ self - object [.sound]
		.extremum = Get absolute extremum: 0, 0, "none"
		assert .extremum < 1e-14 ; '.extremum'
		removeObject: .gabor, .resynthesis
	endfor
	appendInfoLine: tab$, "GaborSpectrogram: oversampling"
	.oversampling# = randomUniform# (5, 1, 5)
	for .ianal to size (.oversampling#)
		selectObject: .sound
		.oversampling = .oversampling# [.ianal]
		appendInfoLine: tab$, tab$, fixed$ (.oversampling, 3)
		.gabor = To GaborSpectrogram: 0, 40, 20, .oversampling, "Hanning"
		.resynthesis = To Sound
		Formula: ~ self - object [.sound]
		.extremum = Get absolute extremum: 0, 0, "none"
		assert .extremum < 1e-14 ; '.extremum'
		removeObject: .gabor, .resynthesis
	endfor
	removeObject: .sound
endproc

procedure test_ConstantQLog2FSpectrogram
	appendInfoLine: tab$, "ConstantQLog2FSpectrogram: windows:"
	.windows$# = { "rectangular", "triangular", "parabolic", "Hanning", "Hamming", "Gaussian1", "Kaiser1"  }
	.sound = Create Sound from formula: "s", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
	for iwindow to size (.windows$#)
		selectObject: .sound
		appendInfoLine: tab$, tab$, .windows$# [iwindow]
		.cq = To ConstantQLog2FSpectrogram: 110, 0.0, 24, 1.0, 1, .windows$# [iwindow]
		.resynthesis = To Sound
		Formula: ~ self - object [.sound]
		.extremum = Get absolute extremum: 0, 0, "none"
		assert .extremum < 1e-14
		removeObject: .cq, .resynthesis
	endfor
	appendInfoLine: tab$, "ConstantQLog2FSpectrogram: numberOfFrequencyBins, frequencyResolution"
	.numberOfAnalysis = 10
	.numberOfFrequencyBins# = randomInteger# (.numberOfAnalysis, 5, 50)
	for .ianal to .numberOfAnalysis
		selectObject: .sound
		.numberOfFrequencyBins = .numberOfFrequencyBins# [.ianal]
		.frequencyResolution = randomUniform (1, 5)
		appendInfoLine: tab$, tab$, .numberOfFrequencyBins, " ", fixed$ (.frequencyResolution, 3)
		.cq = To ConstantQLog2FSpectrogram: 110, 0.0, .numberOfFrequencyBins, .frequencyResolution, 1, "Hanning"
		.resynthesis = To Sound
		Formula: ~ self - object [.sound]
		.extremum = Get absolute extremum: 0, 0, "none"
		assert .extremum < 1e-14 ; '.extremum'
		removeObject: .cq, .resynthesis
	endfor
	appendInfoLine: tab$, "ConstantQLog2FSpectrogram: oversampling"
	.oversampling# = randomUniform# (5, 1, 5)
	for .ianal to size (.oversampling#)
		selectObject: .sound
		.oversampling = .oversampling# [.ianal]
		appendInfoLine: tab$, tab$, fixed$ (.oversampling, 3)
		.cq = To ConstantQLog2FSpectrogram: 110, 0.0, 24, 1.0,  .oversampling, "Hanning"
		.resynthesis = To Sound
		Formula: ~ self - object [.sound]
		.extremum = Get absolute extremum: 0, 0, "none"
		assert .extremum < 1e-14 ; '.extremum'
		removeObject: .cq, .resynthesis
	endfor
	removeObject: .sound
endproc