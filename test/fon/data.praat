# data.praat
# Paul Boersma, 15 November 2022
# Checks Copy, Equal, Read, Save.

tracing$ = "no"

writeInfoLine: "Data test"
stopwatch

appendInfoLine: "Sound"
sound = Create Sound from formula: "kanweg", 1, 0, 1.2345, 44100, ~ 1/2 * sin(2*pi*377*x) + randomGauss (0, 0.1)
@test (sound)
Remove

appendInfoLine: "Pitch"
sound = Read from file: "test.wav"
pitch = To Pitch: 0, 75, 600
@test (pitch)

appendInfoLine: "Formant"
selectObject: sound
formant = To Formant (burg): 0, 5, 5000, 0.025, 50
@test (formant)

appendInfoLine: "PointProcess"
selectObject: sound, pitch
pulses = To PointProcess (cc)
@test (pulses)

appendInfoLine: "PitchTier"
pitchTier = To PitchTier: 0.02
@test (pitchTier)

appendInfoLine: "Manipulation"
selectObject: sound
manipulation = To Manipulation: 0.01, 75, 600
@test (manipulation)

appendInfoLine: "Matrix"
matrix = Create simple Matrix: "xy2", 10, 20, ~ x * y^2
@test (matrix)
Save as matrix text file: "kanweg.txt"
matrix2 = Read from file: "kanweg.txt"
assert objectsAreIdentical: matrix, matrix2
Remove
selectObject: matrix
Save as headerless spreadsheet file: "kanweg.txt"
matrix3 = Read Matrix from raw text file: "kanweg.txt"
assert objectsAreIdentical: matrix, matrix3
Remove

appendInfoLine: "Speaker"
speaker = Create Speaker: "man", "male", "2"
@test (speaker)

appendInfoLine: "OTGrammar"
grammar = Create metrics grammar: "equal", "FtNonfinal", "no", "no", "no", "Nonfinal", "yes", "no", "no"
@test (grammar)

appendInfoLine: "Table"
table = Create formant table (Peterson & Barney 1952)
@test (table)
Save as tab-separated file: "kanweg.txt"
table2 = Read Table from tab-separated file: "kanweg.txt"
assert objectsAreIdentical: table, table2
Remove
table3 = Read Table from tab-separated file: "kanweg.txt"
assert objectsAreIdentical: table, table3
Remove

appendInfoLine: "TableOfReal"
tableOfReal = Create TableOfReal (Pols 1973): "no"
@test (tableOfReal)
Save as headerless spreadsheet file: "kanweg.txt"
tableOfReal2 = Read TableOfReal from headerless spreadsheet file: "kanweg.txt"
assert objectsAreIdentical: tableOfReal, tableOfReal2
Remove

appendInfoLine: "FFNet"
Create iris example: 0, 0
ffnet = selected: "FFNet"
pattern = selected: "Pattern"
categories = selected: "Categories"
@test (ffnet)
@test (pattern)
@test (categories)

appendInfoLine: "Discriminant"
selectObject: pattern, categories
discriminant = To Discriminant
@test (discriminant)

appendInfoLine: "DTW"
selectObject: sound
soundCopy = Copy: "soundCopy"
plusObject: sound
dtw = To DTW: 0.015, 0.005, 0.1, "1/2 < slope < 2"
removeObject: soundCopy
@test (dtw)

appendInfoLine: "TextGrid"
textgrid = Create TextGrid: 0, 1, "tier", ""
Set interval text: 1, 1, "hup åçé pořád 𝄐𝄑 𐌀𐌁𐌂"
@test (textgrid)

appendInfoLine: "Network"
instar = 0.5
outstar = 0.5
weight_leak = 0
hasToBeWorkingAlways = 0
audf.numberOfNodes = if hasToBeWorkingAlways then 20 else 30 fi
sf.numberOfNodes = 10
inhibitionAtSF = if hasToBeWorkingAlways then -0.5 else -0.1 fi   ; -0.2
shunting = if hasToBeWorkingAlways then 0 else 0.0 fi   ; 0
net.spreadingRate = 0.01
net.activityLeak = 1
net.amin = 0
net.amax = 10
net.wmin = -10   ; minimum weight of a connection (negative to allow inhibition)
net.wmax = 10   ; maximum weight of a connection (positive to allow excitation)
net.learningRate = 0.01   ; how much do the connection weights change as a function of the product of the two activities?
net.xmin = 0   ; for drawing
net.xmax = 10   ; for drawing
net.ymin = 0   ; for drawing
net.ymax = 10   ; for drawing
net.initialWmin = 0
net.initialWmax = 0.1
audf.y = 4.0
sf.y = 9.0
audf.offsetNode = 0
sf.offsetNode = audf.offsetNode + audf.numberOfNodes
network = Create empty Network... integration
... net.spreadingRate Linear net.amin net.amax net.activityLeak
... net.learningRate net.wmin net.wmax weight_leak
... net.xmin net.xmax net.ymin net.ymax
for .i to audf.numberOfNodes
	Add node... net.xmin+(net.xmax-net.xmin)/audf.numberOfNodes*(.i-0.5) audf.y 0 yes
endfor
for .i to sf.numberOfNodes
	Add node... net.xmin+(net.xmax-net.xmin)/sf.numberOfNodes*(.i-0.5) sf.y 0 no
endfor
for .i to audf.numberOfNodes
	for .j to sf.numberOfNodes
		Add connection... audf.offsetNode+.i sf.offsetNode+.j randomUniform(net.initialWmin,net.initialWmax) 1.0
	endfor
endfor
for .i to sf.numberOfNodes - 1
	for .j from .i + 1 to sf.numberOfNodes
		Add connection... sf.offsetNode+.i sf.offsetNode+.j inhibitionAtSF 0.0
	endfor
endfor
Set instar... instar
Set outstar... outstar
Set shunting... shunting
@test (network)

procedure selectAll ( )
	selectObject: sound, pitch, formant, pulses, pitchTier, manipulation, matrix, speaker, grammar, table, tableOfReal,
	... ffnet, pattern, categories, discriminant, dtw, textgrid, network
endproc
procedure readCheckCollectionFile ( )
	Read from file: "kanweg.Collection"
	sound2 = selected: "Sound"
	assert objectsAreIdentical: sound, sound2
	pitch2 = selected: "Pitch"
	assert objectsAreIdentical: pitch, pitch2
	formant2 = selected: "Formant"
	assert objectsAreIdentical: formant, formant2
	pulses2 = selected: "PointProcess"
	assert objectsAreIdentical: pulses, pulses2
	pitchTier2 = selected: "PitchTier"
	assert objectsAreIdentical: pitchTier, pitchTier2
	manipulation2 = selected: "Manipulation"
	assert objectsAreIdentical: manipulation, manipulation2
	matrix2 = selected: "Matrix"
	assert objectsAreIdentical: matrix, matrix2
	speaker2 = selected: "Speaker"
	assert objectsAreIdentical: speaker, speaker2
	grammar2 = selected: "OTGrammar"
	assert objectsAreIdentical: grammar, grammar2
	table2 = selected: "Table"
	assert objectsAreIdentical: table, table2
	tableOfReal2 = selected: "TableOfReal"
	assert objectsAreIdentical: tableOfReal, tableOfReal2
	ffnet2 = selected: "FFNet"
	assert objectsAreIdentical: ffnet, ffnet2
	pattern2 = selected: "Pattern"
	assert objectsAreIdentical: pattern, pattern2
	categories2 = selected: "Categories"
	assert objectsAreIdentical: categories, categories2
	discriminant2 = selected: "Discriminant"
	assert objectsAreIdentical: discriminant, discriminant2
	dtw2 = selected: "DTW"
	assert objectsAreIdentical: dtw, dtw2
	textgrid2 = selected: "TextGrid"
	assert objectsAreIdentical: textgrid, textgrid2
	network2 = selected: "Network"
	assert objectsAreIdentical: network, network2
	removeObject: sound2, pitch2, formant2, pulses2, pitchTier2, manipulation2, matrix2, speaker2, grammar2, table2, tableOfReal2,
	... ffnet2, pattern2, categories2, discriminant2, dtw2, textgrid2, network2
endproc

appendInfoLine: "text Collection"
@selectAll ( )
Save as text file: "kanweg.Collection"
@readCheckCollectionFile ( )

appendInfoLine: "short text Collection"
@selectAll ( )
Save as short text file: "kanweg.Collection"
@readCheckCollectionFile ( )

appendInfoLine: "binary Collection"
@selectAll ( )
Save as binary file: "kanweg.Collection"
@readCheckCollectionFile ( )

removeObject: sound, pitch, formant, pulses, pitchTier, manipulation, matrix, speaker, grammar, table, tableOfReal,
... ffnet, pattern, categories, discriminant, dtw, textgrid, network
deleteFile: "kanweg.Object"
deleteFile: "kanweg.Collection"

t = stopwatch
appendInfoLine: "OK (", fixed$ (t, 3), " seconds)"

procedure test (.object1)
	selectObject: .object1
	.object2 = Copy: "kanweg2"
	assert objectsAreIdentical: .object1, .object2
	Remove
	selectObject: .object1
	.object2 = Copy: "kanweg2"
	assert objectsAreIdentical: .object1, .object2
	Remove
	# Test verbose Unicode text writing (for correct data).
	selectObject: .object1
	Save as text file: "kanweg.Object"
	.object2 = Read from file: "kanweg.Object"
	assert objectsAreIdentical: .object1, .object2   ; verbose Unicode write and read
	Remove
	# Test concise Unicode text writing (for correct data).
	selectObject: .object1
	Save as short text file: "kanweg.Object"
	.object2 = Read from file: "kanweg.Object"
	assert objectsAreIdentical: .object1, .object2   ; concise Unicode write and read
	Remove
	# Test binary writing.
	selectObject: .object1
	Save as binary file: "kanweg.Object"
	.object2 = Read from file: "kanweg.Object"
	assert objectsAreIdentical: .object1, .object2   ; binary write and read
	Remove
	# Test binary writing.
	selectObject: .object1
	Save as binary file: "kanweg.Object"
	Debug: tracing$, 18
	.object2 = Read from file: "kanweg.Object"
	Debug: tracing$, 0
	assert objectsAreIdentical: .object1, .object2   ; binary write and read
	Remove
	# Test binary writing.
	selectObject: .object1
	Debug: tracing$, 18
	Save as binary file: "kanweg.Object"
	Debug: tracing$, 0
	.object2 = Read from file: "kanweg.Object"
	assert objectsAreIdentical: .object1, .object2   ; binary write and read
	Remove
	# Good neighbour.
	selectObject: .object1
endproc
