# test_SpeechSynthesizer.praat
# djmw 20120130, 20120522, 20160524, 20171103

appendInfoLine: "test_SpeechSynthesizer.praat"

voiceslist = Extract espeak data: "Voices properties"
numberOfVoices = Get number of rows

appendInfoLine: tab$, "Languages:"
languageslist = Extract espeak data: "Language properties"
numberOfLanguages = Get number of rows

numberOfSounds = 0
for ilang to numberOfLanguages
	selectObject: languageslist
	language$ = Get value: ilang, "name"
	appendInfo: tab$, tab$, language$, ":"
	for ivoice to 3
		selectObject: voiceslist
		voice$ = Get value: randomInteger (1, numberOfVoices), "name"
		appendInfo:  " ", voice$
		ss = Create SpeechSynthesizer: language$, voice$
		sound = To Sound: "a e u", "no"
		;Play
		removeObject: ss, sound
		numberOfSounds += 1
	endfor
	appendInfo: newline$
endfor
appendInfoLine: tab$, tab$, numberOfLanguages, " languages, ", numberOfSounds, " sounds created/removed"
appendInfoLine: tab$, "Languages: OK"

appendInfoLine: tab$, "Writing and reading:"
ss = Create SpeechSynthesizer: language$, voice$
Save as text file: "kanweg.SpeechSynthesizer"
ss2 = Read from file: "kanweg.SpeechSynthesizer"
appendInfoLine: tab$, "Writing and reading: OK"

appendInfoLine: tab$, "Compatibility:"
ss3 = Create SpeechSynthesizer: "Default", "default"
ss4 = Create SpeechSynthesizer: "English", "f1"
appendInfoLine: tab$, "Compatibility: OK"

appendInfoLine: tab$, "Get phonemes from text:"
text$ = "This is some text."
phonemes$ = "ðɪs ɪz sʌm tɛkst"
phonemesWithSpaces$ = "ð ɪ s   ɪ z   s ʌ m   t ɛ k s t "
textout$ = Get phonemes from text: text$
assert phonemes$ = textout$
textoutWithSpaces$ = Get phonemes from text (space-separated): text$
assert phonemesWithSpaces$ = textoutWithSpaces$
appendInfoLine: tab$, "Get phonemes from text: OK"

removeObject: voiceslist, languageslist, ss, ss2,  ss3, ss4

appendInfoLine: "test_SpeechSynthesizer.praat OK"
