# test_MixingMatrix.praat
# djmw 20170421

appendInfoLine: "test_MixingMatrix"

stereo = Create Sound from formula: "s", 2, 0, 1, 44100, "sin(2*pi*row*300*x)"
mono = Create Sound from formula: "s", 1, 0, 1, 44100, "sin(2*pi*300*x)"
mm [1] = Create simple MixingMatrix: "mm1", 2, 1, "1 0"
mm [2] = Create simple MixingMatrix: "mm2", 2, 1, "0 1"
mm [3] = Create simple MixingMatrix: "mm3", 2, 1, "1 1"
mm [4] = Create simple MixingMatrix: "mm4", 2, 2, "1 0 1 0"
mm [5] = Create simple MixingMatrix: "mm5", 2, 1, "0.5 0.5"
mm [6] = Create simple MixingMatrix: "mm7", 2, 2, "0 1 1 0"

mm [7] = Create simple MixingMatrix: "mm6", 1, 2, "1 1"

appendInfoLine: tab$+ "Mix"

for i to 6
	selectObject: mm [i]
	numberOfRows = Get number of rows
	selectObject: stereo, mm[i]
	s [i] = Mix
	numberOfChannels = Get number of channels
	assert numberOfChannels = numberOfRows; 'i'
endfor

selectObject: mono, mm [7]
s [7] = Mix

eps = 1e-15
for i to 300
	stereo_300 = Object_'stereo' [1,i]
	stereo_600 = Object_'stereo' [2,i]
	mono_300 = Object_'mono' [1,i]
	for j to 7
		sj = s[j]
		s1_'j' = Object_'sj' [1,i]
	endfor
	assert abs (s1_1 - stereo_300) < eps; mm[1]
	assert abs(s1_2 - stereo_600) < eps; mm[2]
	assert abs(s1_3 - (stereo_300 + stereo_600)) < eps; mm[3]
	s4 = s [4]
	s2_4 = Object_'s4' [2,i]
	assert abs(s1_4 - stereo_300) < eps; mm[4]
	assert abs(s2_4 - stereo_300) < eps; mm[4]
	assert abs(s1_5 - 0.5*(stereo_300 + stereo_600)) < eps; mm[5]
	assert abs(s1_6 - stereo_600) < eps; mm[6]
	s6 = s [6]
	s2_6 = Object_'s6' [2,i]
	assert abs(s2_6 - stereo_300) < eps; mm[6]
	assert abs(s1_7 - stereo_300) < eps; mm[7]
	s7 = s [7]
	s2_7= Object_'s7' [2,i]
	assert abs(s2_7 - stereo_300) < eps; mm[7]		
endfor

for i to 7
	removeObject: s [i]
endfor

removeObject: stereo, mono, mm [7]

appendInfoLine: tab$+ "Mix part..."

stereo = Create Sound from formula: "s", 2, 0, 1, 44100, "row"
for i to 6
	t1 = randomUniform (-0.5, 1.5)
	numberOfSamples = randomInteger (1, 1000)
	t2 = t1 + numberOfSamples / 44100
	selectObject: mm [i]
	numberOfRows = Get number of rows
	selectObject: stereo, mm[i]
	s [i] = Mix part: t1, t2
	n = Get number of samples
	assert n == numberOfSamples; 'i'
	start = Get start time
	end = Get end time
	assert abs (start - t1) < eps; 'start' 'i'
	assert abs (end - t2) < eps; end' 'i'
endfor

for i to 6
	removeObject: mm [i], s[i]
endfor
removeObject: stereo

appendInfoLine: "test_MixingMatrix OK"
