# Praat script PairDistributions_append.praat
# Paul Boersma, 7 May 2011

if numberOfSelected ( ) < 1 or numberOfSelected ("PairDistribution") <> numberOfSelected ( )
	exit Select one or more PairDistribution objects.
endif
numberOfDistributions = numberOfSelected ( )
for i to numberOfDistributions
	distribution [i] = selected (i)
endfor
#
# Write header.
#
fileName$ = temporaryDirectory$ + "/appended.PairDistribution"
deleteFile (fileName$)
fileappend "'fileName$'" "ooTextFile"'newline$'
fileappend "'fileName$'" "PairDistribution"'newline$'
#
# How many pairs do all the distributions together have?
#
totalNumberOfPairs = 0
for i to numberOfDistributions
	select distribution [i]
	numberOfPairs [i] = Get number of pairs
	totalNumberOfPairs += numberOfPairs [i]
endfor
fileappend "'fileName$'" 'totalNumberOfPairs''newline$'
#
# Write the data.
#
for i to numberOfDistributions
	select distribution [i]
	for j to numberOfPairs [i]
		string1$ = Get string1... j
		string2$ = Get string2... j
		weight = Get weight... j
		fileappend "'fileName$'" "'string1$'" "'string2$'" 'weight''newline$'
	endfor
endfor
#
# Read the result.
#
Read from file... 'fileName$'
deleteFile (fileName$)
