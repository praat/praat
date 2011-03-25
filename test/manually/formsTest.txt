#! Praat script formsTest.txt
# Paul Boersma, 20 January 2009

echo script
compression = 1
number_of_channels = 2
worth = 3
for i to 5
	beginPause ("Hi")
		comment ("Type a lot of nonsense below.")
		natural ("Number of people", 10)
		real ("Worth", worth+1)
		positive ("Sampling frequency (Hz)", "44100.0 (= CD quality)")
		word ("hi", "hhh")
		sentence ("lo", "two words")
		text ("ko", "jkgkjhkj g gdfg dfg")
		boolean ("You like it?", 1)
		if worth < 6
			choice ("Compression", compression)
				option ("lossless (FLAC)")
				option ("MP3")
				option ("Ogg")
		endif
		optionMenu ("Number of channels", number_of_channels)
			option ("mono")
			option ("stereo")
			option ("quadro")
		comment ("Then click Stop or one of the continuation buttons.")
	clicked = endPause ("Continue", "Next", "Proceed", 2)
	printline 'number_of_people' 'worth' 'sampling_frequency' 'clicked'
	printline Compression: 'compression' ('compression$')
	printline Number of channels: 'number_of_channels$'
endfor
printline end