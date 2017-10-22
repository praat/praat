# test_SpeechSynthesizer.praat
# djmw 20120130, 20120522, 20160524

appendInfoLine: "SpeechSynthesizer test..."

voiceslist = Create copy from FileInMemorySet: "voices_names"
numberOfVoices = Get number of strings

languageslist = Create copy from FileInMemorySet: "languages_names"
numberOfLanguages = Get number of strings

numberOfSounds = 0
for ilang to numberOfLanguages
	selectObject: languageslist
	language$ = Get string: ilang
	appendInfo: tab$, language$, ":"
	for ivoice to numberOfVoices
		selectObject: voiceslist
		voice$ = Get string: ivoice
		appendInfo:  " ", voice$
		# some voices have spaces!
		ss = Create SpeechSynthesizer: language$, voice$
		sound = To Sound: "a e u", "no"
		;Play
		removeObject: ss, sound
		numberOfSounds += 1
	endfor
	appendInfo: newline$
endfor
appendInfoLine: tab$, numberOfLanguages, " voices, ", numberOfSounds, " sounds created/removed"
appendInfoLine: tab$, "Writing and reading..."
ss = Create SpeechSynthesizer: language$, voice$
Save as text file: "kanweg.SpeechSynthesizer"
ss2 = Read from file: "kanweg.SpeechSynthesizer"

removeObject: voiceslist, languageslist, ss, ss2

appendInfoLine: "SpeechSynthesizer test OK"
