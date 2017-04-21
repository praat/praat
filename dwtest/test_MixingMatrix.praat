# test_MixingMatrix.praat
# djmw 20170421

appendInfoLine: "test_MixingMatrix"

stereo = Create Sound from formula: "s", 2, 0, 1, 44100, "sin(2*pi*row*300*x)"
mm [1] = Create simple MixingMatrix: "mm1", 2, 1, "1 0"
mm [2] = Create simple MixingMatrix: "mm2", 2, 1, "0 1"
mm [3] = Create simple MixingMatrix: "mm3", 2, 1, "1 1"
mm [4] = Create simple MixingMatrix: "mm4", 2, 2, "1 0 1 0"

for i to 4
	selectObject: mm [i]
	numberOfRows = Get number of rows
	selectObject: stereo, mm[i]
	s [i] = Mix
	numberOfChannels = Get number of channels
	assert numberOfChannels = numberOfRows
	removeObject: mm [i], s [i]
endfor

removeObject: stereo

appendInfoLine: "test_MixingMatrix OK"
