# speechsynthesizer_test.praat
# djmw 20151209

# show memory leaks of espeak 

synth = Create SpeechSynthesizer: "English (Great Britain)", "Female1"
numberOfTries = 500
table = Create Table with column names: "m", numberOfTries, "run bytes"
for i to numberOfTries
	selectObject: synth
	s = To Sound: "This is some text.", "no"
	removeObject: s
	@get_memoryTotalCreated
	selectObject: table
	Set numeric value: i, "run", i
	Set numeric value: i, "bytes", get_memoryTotalCreated.bytes
endfor
selectObject: table
Append column: "diff"
Formula (column range): "diff", "diff", ~ self [row, "bytes"] - self [row-1, "bytes"]
result = Extract rows where: "row > 1"
removeObject: table
selectObject: result
minimum = Get minimum: "diff"
Append column: "rdiff"
Formula (column range): "rdiff", "rdiff", ~ self ["diff"] - minimum

Erase all
@asSpectrum: result, 3
selectObject: asSpectrum.sound
Select outer viewport: 0, 6, 0, 4
Draw: 0, 0, 0, 0, "yes", "poles" 
Text top: "no", "espeak version 1.47.04"
selectObject: asSpectrum.spectrum
Select outer viewport: 0, 6, 4, 8
Draw: 0, 0, 0, 0, "yes"
Marks bottom every: 1, 50 , "yes", "yes", "yes"

;removeObject: result, asSpectrum.spectrum, asSpectrum.sound

procedure get_memoryTotalCreated
	.report$ = Report memory use
	.end = index (.report$, " bytes)")
	.power = 0
	.bytes = 0
	while .end > 0
		.end -= 1
		.ch$ = mid$ (.report$, .end, 1)
		if .ch$ = ","
			; continue
		elsif .ch$ = "("
			.end = 0
		else
			.bytes += 10^.power * number (.ch$)
			.power += 1
		endif
	endwhile
endproc

procedure asSpectrum: .table, .column
	selectObject: .table
	.tor = Down to TableOfReal: "run"
	.mat = To Matrix
	.matt = Transpose
	.sound1 = To Sound
	.sound = Extract one channel: .column
	Override sampling frequency: 1000
	.spectrum = To Spectrum: "no"
	Formula: ~ if col > 1 then self else 0 fi
	removeObject: .tor, .mat, .matt, .sound1
	selectObject: .spectrum	
endproc

