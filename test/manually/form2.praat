form: "manually/Form2"
	positive: "Frequency (Hz)", "100*100"
	boolean: "Called from outside", 0
	infile: 1, "Input file (Hz)", "innetje.wav"
	outfile: "Output_file", "uitje.wav"
	folder: "Folder", "mapje"
	real: "left Pitch range (Hz)", "0.01 (= auto)"
	positive: "right Pitch range (Hz)", "500.0"
	word: "Any word", "here"
	text ttt Paul
	text: "Tekstje", "abc"
	text: 2, "Tekst", "abcdef"
	realvector: "Numbers1", "(formula)", "{ 10, 80, 20 }"
	realvector: 3, "Numbers2", "(whitespace-separated)", "8 19 -1.5"
	choice: "Foreground colour", 2
		option: "Red"
		option: "Green"
		option: "Blue"
	optionmenu: "Texture", 1
		option: "Smooth"
		option: "Rough"
		option: "With holes"
endform

appendInfoLine: "Form2..."
appendInfoLine: "Frequency: <<", frequency, ">>"
appendInfoLine: "Called from outside: <<", called_from_outside, ">>"
appendInfoLine: "Input file: <<", input_file$, ">>"
appendInfoLine: "Output file: <<", output_file$, ">>"
appendInfoLine: "Folder file: <<", folder$, ">>"
appendInfoLine: "fmin: <<", left_Pitch_range, ">>"
appendInfoLine: "fmax: <<", right_Pitch_range, ">>"
appendInfoLine: "word: <<", any_word$, ">>"
appendInfoLine: "ttt: <<", ttt$, ">>"
appendInfoLine: "Tekstje: <<", tekstje$, ">>"
appendInfoLine: "Tekst: <<", tekst$, ">>"
appendInfoLine: "Numbers1: <<", numbers1#, ">>"
appendInfoLine: "Numbers2: <<", numbers2#, ">>"
appendInfoLine: "Colour: <<", foreground_colour, ">>"
appendInfoLine: "Colour: <<", foreground_colour$, ">>"
appendInfoLine: "Texture: <<", texture, ">>"
appendInfoLine: "Texture: <<", texture$, ">>"

# if called from test/script/form1.praat:
assert numbers1# = { 16, -17.6, 5 }
assert numbers2# = { 6, 7, 8 }
if windows
	assert endsWith (input_file$, "test\script\2345")   ; should not be possible
	assert endsWith (output_file$, "test\script\..\abc.txt")
	assert endsWith (folder$, "test\script\subfolder\wav2vec")
else
	assert endsWith (input_file$, "test/script/2345")   ; should not be possible
	assert endsWith (output_file$, "test/script/../abc.txt")
	assert endsWith (folder$, "test/script/subfolder/wav2vec")
endif
assert texture$ = "With holes"
assert any_word$ = "there you are"

appendInfoLine: "OK manually/Form2"
