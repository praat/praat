# data.praat
# Paul Boersma, 22 January 2009
# Checks Copy, Equal, Read, Write.
# 10 October 2012

echo Data test
stopwatch

printline Sound
sound = Create Sound from formula... kanweg Mono 0 1.2345 44100 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
call test sound
Remove

printline Pitch
sound = Read from file... test.wav
pitch = To Pitch... 0 75 600
call test pitch

printline Formant
select sound
formant = To Formant (burg)... 0 5 5000 0.025 50
call test formant

printline PointProcess
select sound
plus pitch
pulses = To PointProcess (cc)
call test pulses

printline PitchTier
pitchTier = To PitchTier... 0.02
call test pitchTier

printline Manipulation
select sound
manipulation = To Manipulation... 0.01 75 600
call test manipulation

printline Matrix
matrix = Create simple Matrix... xy2 10 20 x*y^2
call test matrix
Write to matrix text file... kanweg.txt
matrix2 = Read from file... kanweg.txt
assert objectsAreIdentical (matrix, matrix2)
Remove
select matrix
Write to headerless spreadsheet file... kanweg.txt
matrix3 = Read Matrix from raw text file... kanweg.txt
assert objectsAreIdentical (matrix, matrix3)
Remove

printline Speaker
speaker = Create Speaker... man male 2
call test speaker

printline OTGrammar
grammar = Create metrics grammar... equal FtNonfinal no no no Nonfinal yes no no
call test grammar

printline Table
table = Create formant table (Peterson & Barney 1952)
call test table
Write to table file... kanweg.txt
table2 = Read Table from tab-separated file... kanweg.txt
assert objectsAreIdentical (table, table2)
Remove
table3 = Read Table from table file... kanweg.txt
assert objectsAreIdentical (table, table3)
Remove

printline TableOfReal
tableOfReal = Create TableOfReal (Pols 1973)... no
call test tableOfReal
Write to headerless spreadsheet file... kanweg.txt
tableOfReal2 = Read TableOfReal from headerless spreadsheet file... kanweg.txt
assert objectsAreIdentical (tableOfReal, tableOfReal2)
Remove

printline FFNet
Create iris example... 0 0
ffnet = selected ("FFNet")
pattern = selected ("Pattern")
categories = selected ("Categories")
call test ffnet
call test pattern
call test categories

printline KNN
select pattern
plus categories
knn = To KNN Classifier... Classifier random
call test knn

printline Discriminant
select pattern
plus categories
discriminant = To Discriminant
call test discriminant

procedure selectAll
	select sound
	plus pitch
	plus formant
	plus pulses
	plus pitchTier
	plus manipulation
	plus matrix
	plus speaker
	plus grammar
	plus table
	plus tableOfReal
	plus ffnet
	plus pattern
	plus categories
	plus knn
	plus discriminant
endproc
procedure readCheckCollectionFile
	Read from file... kanweg.Collection
	sound2 = selected ("Sound")
	assert objectsAreIdentical (sound, sound2)
	pitch2 = selected ("Pitch")
	assert objectsAreIdentical (pitch, pitch2)
	formant2 = selected ("Formant")
	assert objectsAreIdentical (formant, formant2)
	pulses2 = selected ("PointProcess")
	assert objectsAreIdentical (pulses, pulses2)
	pitchTier2 = selected ("PitchTier")
	assert objectsAreIdentical (pitchTier, pitchTier2)
	manipulation2 = selected ("Manipulation")
	assert objectsAreIdentical (manipulation, manipulation2)
	matrix2 = selected ("Matrix")
	assert objectsAreIdentical (matrix, matrix2)
	speaker2 = selected ("Speaker")
	assert objectsAreIdentical (speaker, speaker2)
	grammar2 = selected ("OTGrammar")
	assert objectsAreIdentical (grammar, grammar2)
	table2 = selected ("Table")
	assert objectsAreIdentical (table, table2)
	tableOfReal2 = selected ("TableOfReal")
	assert objectsAreIdentical (tableOfReal, tableOfReal2)
	ffnet2 = selected ("FFNet")
	assert objectsAreIdentical (ffnet, ffnet2)
	pattern2 = selected ("Pattern")
	assert objectsAreIdentical (pattern, pattern2)
	categories2 = selected ("Categories")
	assert objectsAreIdentical (categories, categories2)
	knn2 = selected ("KNN")
	assert objectsAreIdentical (knn, knn2)
	discriminant2 = selected ("Discriminant")
	assert objectsAreIdentical (discriminant, discriminant2)
	select sound2
	plus pitch2
	plus formant2
	plus pulses2
	plus pitchTier2
	plus manipulation2
	plus matrix2
	plus speaker2
	plus grammar2
	plus table2
	plus tableOfReal2
	plus ffnet2
	plus pattern2
	plus categories2
	plus knn2
	plus discriminant2
	Remove
endproc

printline text Collection
call selectAll
Write to text file... kanweg.Collection
call readCheckCollectionFile

printline short text Collection
call selectAll
Write to short text file... kanweg.Collection
call readCheckCollectionFile

printline binary Collection
call selectAll
Write to binary file... kanweg.Collection
call readCheckCollectionFile

select sound
plus pitch
plus formant
plus pulses
plus pitchTier
plus manipulation
plus matrix
plus speaker
plus grammar
plus table
plus tableOfReal
plus ffnet
plus pattern
plus categories
plus knn
plus discriminant
Remove
deleteFile ("kanweg.Object")
deleteFile ("kanweg.Collection")

t = stopwatch
printline OK ('t:3' seconds)

procedure test .object1
	select .object1
	.object2 = Copy... kanweg2
	assert objectsAreIdentical (.object1, .object2)
	Remove
	select .object1
	.object2 = Copy... kanweg2
	assert objectsAreIdentical (.object1, .object2)
	Remove
	# Test verbose ASCII text writing (for correct data).
	select .object1
	Write to text file... kanweg.Object
	.object2 = Read from file... kanweg.Object
	assert objectsAreIdentical (.object1, .object2)   ; verbose ASCII write and read
	Remove
	# Test concise ASCII text writing (for correct data).
	select .object1
	Write to short text file... kanweg.Object
	.object2 = Read from file... kanweg.Object
	assert objectsAreIdentical (.object1, .object2)   ; concise ASCII write and read
	Remove
	# Test binary writing.
	select .object1
	Write to binary file... kanweg.Object
	.object2 = Read from file... kanweg.Object
	assert objectsAreIdentical (.object1, .object2)   ; binary write and read
	Remove
	# Test binary writing.
	select .object1
	Write to binary file... kanweg.Object
	Debug... no 18
	.object2 = Read from file... kanweg.Object
	Debug... no 0
	assert objectsAreIdentical (.object1, .object2)   ; binary write and read
	Remove
	# Test binary writing.
	select .object1
	Debug... no 18
	Write to binary file... kanweg.Object
	Debug... no 0
	.object2 = Read from file... kanweg.Object
	assert objectsAreIdentical (.object1, .object2)   ; binary write and read
	Remove
	# Good neighbour.
	select .object1
endproc
