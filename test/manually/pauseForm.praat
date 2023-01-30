# test/manually/pauseForm.praat
# Paul Boersma 2023-01-30

writeInfoLine: "script"
compression = 1
number_of_channels = 2
worth = 3
for i to 5
	beginPause ("Hi")
		comment: "Type a lot of nonsense below."
		natural: "Number of people", 10
		real: "Worth", worth + 1
		positive: "Sampling frequency (Hz)", "44100.0 (= CD quality)"
		word: "hi", "hhh"
		sentence: "lo", "two words"
		text: "shortText", "some one-line text here"
		text: 2, "longText", "some scrollable text here, within a height of 2 lines"
		boolean: "You like it?", 1   ; BUG: things like "yes" should be possible as well
		infile: "Input file", "hello.wav"
		outfile: 2, "Output file", "../out.txt"
		realvector: 2, "Array of reals", "(whitespace-separated)", "20 80 60"
		if worth < 6
			choice: "Compression", compression
				option ("lossless (FLAC)")
				option ("MP3")
				option ("Ogg")
		endif
		optionmenu: "Number of channels", number_of_channels
			option: "mono"
			option: "stereo"
			option: "quadro"
		comment: "Then click Stop or one of the continuation buttons."
	clicked = endPause: "Continue", "Next", "Proceed", 2
	appendInfoLine: number_of_people, " ", worth, " ", sampling_frequency, " ", clicked
	appendInfoLine: "Compression: ", compression, " (", compression$, ")"
	appendInfoLine: "Number of channels: ", number_of_channels$
	appendInfoLine: "Short text: ", shortText$
	appendInfoLine: "Long text: ", longText$
	appendInfoLine: "Input file: ", input_file$
	appendInfoLine: "Array: ", array_of_reals#
endfor
appendInfoLine: "OK"
