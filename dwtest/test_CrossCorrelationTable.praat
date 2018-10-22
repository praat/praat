# test_CrossCorrelationTable.praat
# djmw 20181018

appendInfoLine: "test_CrossCorrelationTable.praat"

s1 = Create Sound from formula: "1", 2, 0, 0.5, 44100, "randomUniform(-1,1)"
s2 = Create Sound from formula: "2", 2, 0, 0.5, 44100, "randomUniform(-1,1)"
selectObject: s1, s2
s12 = Combine to stereo

dlag = 0.05
for ilag to 5
	selectObject: s1, s2
	cts1s2 = To CrossCorrelationTable (combined): 0, 0.5, (ilag - 1) * dlag
	ts1s2 = Extract row ranges: "1:4"
	selectObject: s12
	cts12 = To CrossCorrelationTable: 0, 0.5, (ilag - 1) * dlag
	ts12 = Extract row ranges: "1:4"

	for irow to 4
		for icol to 4
			assert object [ts12, irow, icol] == object [ts1s2, irow, icol]
		endfor
	endfor
	removeObject: ts12, cts12, ts1s2, cts1s2
endfor
removeObject: s1, s2, s12

appendInfoLine: "test_CrossCorrelationTable.praat OK"