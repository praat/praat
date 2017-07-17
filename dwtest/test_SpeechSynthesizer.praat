# test_SpeechSynthesizer.praat
# djmw 20120130, 20120522, 20160524

appendInfoLine: "SpeechSynthesizer test..."

variantslist = Create copy from FileInMemorySet: "variants_names"
numberOfVariants = Get number of strings

voiceslist = Create copy from FileInMemorySet: "voices_names"
numberOfVoices = Get number of strings

numberOfSounds = 0
for ivoice to numberOfVoices
	selectObject: voiceslist
	voice$ = Get string: ivoice
	appendInfo: tab$, voice$, ":"
	for ivariant to numberOfVariants
		selectObject: variantslist
		variant$ = Get string: ivariant
		appendInfo:  " ", variant$
		# some voices have spaces!
		ss = Create SpeechSynthesizer: voice$, variant$
		sound = To Sound: "a e u", "no"
		;Play
		removeObject: ss, sound
		numberOfSounds += 1
	endfor
	appendInfo: newline$
endfor
appendInfoLine: tab$, numberOfVoices, " voices, ", numberOfSounds, " sounds created/removed"
appendInfoLine: tab$, "Writing and reading..."
ss = Create SpeechSynthesizer: voice$, variant$
Save as text file: "kanweg.SpeechSynthesizer"
ss2 = Read from file: "kanweg.SpeechSynthesizer"

removeObject: variantslist, voiceslist, ss, ss2

appendInfoLine: "SpeechSynthesizer test OK"
