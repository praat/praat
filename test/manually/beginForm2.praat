appendInfoLine: "hallo"
beginForm: "manually/beginForm2"
	frequency = positive: "Frequency (Hz)", "10000.0 (= Nyquist)"
	duration = positive: "Duration (s)", "5.0"
	gain = real: "Gain (0..1)", "0.5"
	;fmin = real: "left Pitch range (Hz)", "0.01 (= auto)"
	;fmax = positive: "right Pitch range (Hz)", "500.0"
	;inputFile$ = infile: 1, "Input file (Hz)", "innetje.wav"
	;outputFile$ = outfile: "Output_file", "uitje.wav"
	;folder$ = folder: "Folder", "mapje"
	;numbers1# = realvector: "Numbers1", "(formula)", { 10, 80, 20 }
	;numbers2# = realvector: 3, "Numbers2", "(whitespace-separated)", "8 19 -1.5"
endform

appendInfoLine: "Form2..."
appendInfoLine: "Frequency: <<", frequency, ">>"
appendInfoLine: "Duration: <<", duration, ">>"
appendInfoLine: "Pitch floor: <<", fmin, ">>"
appendInfoLine: "Pitch ceiling: <<", fmax, ">>"
appendInfoLine: "Input file: <<", input_file$, ">>"
appendInfoLine: "Output file: <<", output_file$, ">>"
appendInfoLine: "Folder file: <<", folder$, ">>"
appendInfoLine: "Numbers1: <<", numbers1#, ">>"
appendInfoLine: "Numbers2: <<", numbers2#, ">>"

# if called from test/script/form1.praat:
assert numbers1# = { 16, -17.6, 5 }
assert numbers2# = { 6, 7, 8 }
assert endsWith (input_file$, "test/script/hello.wav")
assert endsWith (output_file$, "test/script/../abc.txt")
assert endsWith (folder$, "test/script/subfolder/wav2vec")

appendInfoLine: "OK manually/Form2"
