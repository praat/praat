# test_detectSilences.praat
# djmw 20170808

appendInfoLine: "test_detectSilences.praat"

text$ [0] = "+-, +-+-, ..., +-+-+-+-+-"
text$ [1] = "-+, -+-+, ..., -+-+-+-+-+"
db0 = -30
for k from 0 to 1
	appendInfoLine: tab$, text$ [k]
	sound [k] = Create Sound from formula: "s"+ string$(k), 1, 0, 1, 44100, "randomGauss (0,0.1)"
	for i to 5
		selectObject: sound [k]
		Formula (part): (2*i-1 -k)*0.1, (2*i-k) *0.1, 1, 1, "self*10^(db0 / 20)"
		tg = To TextGrid (silences): 100, 0, -25, 0.01, 0.01, "silent", "sounding"
		numberOfIntervals = Get number of intervals: 1
		numberOfIntervals2 = i * 2 + (if i < 5 then 1-k else 0 fi)
		assert numberOfIntervals = numberOfIntervals2; 'numberOfIntervals' 'i' 'k'
		removeObject: tg
	endfor
endfor

appendInfoLine: tab$, "silence is  +3dB or -3dB w.r.t  threshold"
db0 = -30
for k from 0 to 1
	for idb to 2
		db = -27 - (idb -1) * 6; 3 db around db0
		selectObject: sound [k]
		tg = To TextGrid (silences): 100, 0, db, 0.01, 0.01, "silent", "sounding"
		numberOfIntervals = Get number of intervals: 1
		numberOfIntervals2 = if db < db0 then 1 else 10 fi
		assert numberOfIntervals = numberOfIntervals2; 'db'
		removeObject: tg
	endfor
endfor

# sound [0] and sound [1] have 10 intervals
appendInfoLine: tab$, "minimum sounding/silent interval exceeds largest interval"
for k from 0 to 1
	selectObject: sound [k]
	tg = To TextGrid (silences): 100, 0, db, 0.15, 0.15, "silent", "sounding"
	numberOfIntervals = Get number of intervals: 1
	assert numberOfIntervals = 1
	removeObject: sound [k], tg
endfor

# more tests...

#removeObject: sound
appendInfoLine: "test_detectSilences.praat OK"
