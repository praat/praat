form: "manually/Form2"
	positive Frequency_(Hz) 10000
	infile: 1, "Input file (Hz)", "innetje.wav"
	outfile: "Output_file", "uitje.wav"
	folder: "Folder", "mapje"
	;realvector Numbers1 (formula) { 10, 80, 20 }
	;realvector Numbers2 (whitespace-separated) 8 19 -1.5
endform

appendInfoLine: "Form2..."
appendInfoLine: "Frequency: <<", frequency, ">>"
appendInfoLine: "Input file: <<", input_file$, ">>"
appendInfoLine: "Output file: <<", output_file$, ">>"
appendInfoLine: "Folder file: <<", folder$, ">>"
;appendInfoLine: "Numbers1: <<", numbers1#, ">>"
;appendInfoLine: "Numbers2: <<", numbers2#, ">>"

# if called from test/script/form1.praat:
;assert numbers1# = { 16, -17.6, 5 }
;assert numbers2# = { 6, 7, 8 }
assert endsWith (input_file$, "test/script/hello.wav")
assert endsWith (output_file$, "test/script/../abc.txt")
assert endsWith (folder$, "test/script/subfolder/wav2vec")

appendInfoLine: "OK manually/Form2"
