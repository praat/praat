# test_SpeechSynthesizer.praat
# djmw 20120130, 20120522

appendInfoLine: "SpeechSynthesizer test..."

variantslist = Create copy from FilesInMemory: "variants_names"
nvariants = Get number of strings

voiceslist = Create copy from FilesInMemory: "voices_names"
nvoices = Get number of strings

numberOfSounds = 0
for ivoice to nvoices
	selectObject: voiceslist
	voice$ = Get string: ivoice
	appendInfoLine: tab$, voice$
	for ivariant to nvariants
		selectObject: variantslist
		variant$ = Get string: ivariant
		appendInfoLine: tab$, tab$, variant$
		# some voices have spaces!
		ss = Create SpeechSynthesizer: voice$, variant$
		sound = To Sound: "a e u", "no"
		;Play
		removeObject: ss, sound
		numberOfSounds += 1
	endfor
endfor
appendInfoLine: tab$, numberOfSounds, " sounds created/removed"
appendInfoLine: tab$, "Writing and reading..."
ss = Create SpeechSynthesizer: voice$, variant$
Save as text file: "kanweg.SpeechSynthesizer"
ss2 = Read from file: "kanweg.SpeechSynthesizer"

removeObject: variantslist, voiceslist, ss, ss2

appendInfoLine: "SpeechSynthesizer test OK"
