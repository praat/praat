# test_SpeechSynthesizer.praat
# djmw 20120130, 20120522, 20160524, 20171103

appendInfoLine: "SpeechSynthesizer test..."

voiceslist = Extract espeak data: "Voices properties"
numberOfVoices = Get number of rows

languageslist = Extract espeak data: "Language properties"
numberOfLanguages = Get number of rows

numberOfSounds = 0
for ilang to numberOfLanguages
	selectObject: languageslist
	language$ = Get value: ilang, "name"
	appendInfo: tab$, language$, ":"
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
appendInfoLine: tab$, numberOfLanguages, " languages, ", numberOfSounds, " sounds created/removed"
appendInfoLine: tab$, "Writing and reading..."
ss = Create SpeechSynthesizer: language$, voice$
Save as text file: "kanweg.SpeechSynthesizer"
ss2 = Read from file: "kanweg.SpeechSynthesizer"

appendInfoLine: tab$, "Compatibility:"

ss3 = Create SpeechSynthesizer: "Default", "default"
ss4 = Create SpeechSynthesizer: "English", "f1"

removeObject: voiceslist, languageslist, ss, ss2,  ss3, ss4

appendInfoLine: "SpeechSynthesizer test OK"
