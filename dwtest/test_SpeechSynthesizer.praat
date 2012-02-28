# test_SpeechSynthesizer.praat
# djmw 20120130

printline SpeechSynthesizer test...

variantslist = Create copy from FilesInMemory... variants_names
nvariants = Get number of strings

voiceslist = Create copy from FilesInMemory... voices_names
nvoices = Get number of strings

for ivoice to nvoices
	select voiceslist
	voice$ = Get string... ivoice
	printline 'tab$' 'voice$'
	for ivariant to nvariants
		select variantslist
		variant$ = Get string... ivariant
		printline 'tab$''tab$' 'variant$'
		# some voices have spaces!
		ss = Create SpeechSynthesizer... "'voice$'" 'variant$' 44100 0.01 50 50 175 y y
		sound = To Sound... a e u
		Remove
		select ss
		Remove
	endfor
endfor

printline SpeechSynthesizer test OK
