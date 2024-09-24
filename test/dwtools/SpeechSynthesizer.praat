writeInfoLine: "testing SpeechSynthesizer..."
if 1
	appendInfoLine: "testing French..."
	my.SpeechSynthesizer = Create SpeechSynthesizer: "French (France)", "Female1"
	text$ = "voici les clés de ton bonheur"
	phonemes$ = Get phonemes from text: text$
	writeInfoLine: length (text$), " ", length (phonemes$), " ", phonemes$
	assert length (text$) = 29
	assert length (phonemes$) = 27
	assert phonemes$ = "vwasi le- kle də- tɔ̃ bɔnœʁ"
	Remove
endif

if 1
	appendInfoLine: "testing Hindi..."
	my.SpeechSynthesizer = Create SpeechSynthesizer: "Hindi", "Female1"
	text$ = "अवग्रहः"
	phonemes$ = Get phonemes from text: text$
	appendInfoLine: length (text$), " ", length (phonemes$), " ", phonemes$
	assert phonemes$ = "ʌʋʌɡɾəhəh"
	Remove
endif

#
# bug solved on 2024-09-23: too long sound after "Get phonemes from text"
# (wav-buffer was cleared late instead of at the beginning)
#
if 1
	appendInfoLine: "testing clearing of buffer..."
	my.SpeechSynthesizer = Create SpeechSynthesizer: "English (Great Britain)", "Female1"
	text$ = "This is some text."
	my.Sound = To Sound: text$, "no"
	duration = Get total duration
	Remove
	assert duration > 1.0
	assert duration < 1.2
	selectObject: my.SpeechSynthesizer
	ipa$ = Get phonemes from text: text$
	assert ipa$ = "ðɪs ɪz sʌm tɛkst"
	ipa$ = Get phonemes from text: text$
	assert ipa$ = "ðɪs ɪz sʌm tɛkst"
	my.Sound = To Sound: text$, "no"
	duration = Get total duration
	Remove
	assert duration > 1.0
	assert duration < 1.2
	removeObject: my.SpeechSynthesizer
endif

#
# The following was correct in 6.2 through 6.4.17,
# but showed empty clause and word tiers in 6.4.18 through 6.4.20.
# Still existing on 2024-09-23.
#
if 1
	appendInfoLine: "testing appearance of clause and word tiers..."
	my.SpeechSynthesizer = Create SpeechSynthesizer: "English (Great Britain)", "Female1"
	Speech output settings: 44100, 0.01, 1, 1, 130, "IPA" 
	To Sound: "This is some text.", "yes"
	my.Sound = selected ("Sound")
	my.TextGrid = selected ("TextGrid")
	selectObject: my.TextGrid
	#
	# Tier 1: "sentence". Should contain the original text.
	#
	numberOfIntervals = Get number of intervals: 1
	assert numberOfIntervals = 1
	sentence$ = Get label of interval: 1, 1
	assert sentence$ = "This is some text."
	#
	# Tier 2: "clause". Should contain the original text, followed by a space.
	#
	numberOfIntervals = Get number of intervals: 2
	assert numberOfIntervals = 2
	clause$ = ""
	for interval to numberOfIntervals
		interval$ = Get label of interval: 2, interval
		clause$ += if interval$ = "" then "·" else interval$ fi
	endfor
	assert clause$ = "This is some text.·"
	#
	# Tier 3: "word". Should contain the words, separated by empty intervals.
	#
	numberOfIntervals = Get number of intervals: 3
	assert numberOfIntervals = 9
	word$ = ""
	for interval to numberOfIntervals
		interval$ = Get label of interval: 3, interval
		word$ += if interval$ = "" then "·" else interval$ fi
	endfor
	assert word$ = "·This·is·some·text.·"
	#
	# Tier 4: "phoneme". Should contain the IPA version of the text, with words separated by empty intervals.
	#
	numberOfIntervals = Get number of intervals: 4
	assert numberOfIntervals = 18
	phonemes$ = ""
	for interval to numberOfIntervals
		interval$ = Get label of interval: 4, interval
		phonemes$ += if interval$ = "" then "·" else interval$ fi
	endfor
	assert phonemes$ = "·ðɪs·ɪz·sʌm·tɛkst·"
	#
	# The following crashed in 6.4.17.
	#
	;selectObject: my.Sound, my.TextGrid
	;View & Edit
	#
	# Clean up.
	#
	removeObject: my.SpeechSynthesizer, my.Sound, my.TextGrid
endif

appendInfoLine: "OK"
