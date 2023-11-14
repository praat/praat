echo texio
# Paul Boersma 2020-07-26

tex1 = Read from file... texio/texio1.TextGrid
Write to chronological text file... kanweg.txt
tex2 = Read from file... kanweg.txt
assert objectsAreIdentical (tex1, tex2)
removeObject: tex1, tex2

asserterror Found a number while looking for a string in text (line 10).
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio2.TextGrid

asserterror Found a number while looking for an enumerated value in text (line 6).
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio4.TextGrid

asserterror Found a string while looking for a real number in text (line 13).
...'newline$'"xmax" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio3.TextGrid

asserterror Found a string while looking for an integer in text (line 14).
...'newline$'Signed integer not read from text file.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio13.TextGrid

asserterror Found a string while looking for an enumerated value in text (line 6).
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio12.TextGrid

asserterror Found an enumerated value while looking for a string in text (line 18).
...'newline$'String "text" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio5.TextGrid

asserterror Found an enumerated value while looking for a real number in text (line 17).
...'newline$'"xmax" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio6.TextGrid

asserterror Found an enumerated value while looking for an integer in text (line 14).
...'newline$'Signed integer not read from text file.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio14.TextGrid

asserterror Character x following quote (line 18). End of string or undoubled quote?
...'newline$'String "text" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio15.TextGrid

asserterror Early end of text detected while looking for a real number (line 21).
...'newline$'"xmax" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio7.TextGrid

asserterror Early end of text detected while looking for an enumerated value (line 6).
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio8.TextGrid

asserterror Early end of text detected while looking for an integer (line 14).
...'newline$'Signed integer not read from text file.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio9.TextGrid

asserterror Early end of text detected while looking for a string (line 18).
...'newline$'String "text" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio10.TextGrid

asserterror Early end of text detected while looking for a real number (line 18).
...'newline$'"xmin" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio11.TextGrid

asserterror Early end of text detected while reading a string (line 18).
...'newline$'String "text" not read.
...'newline$'IntervalTier not read.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio16.TextGrid

asserterror Early end of text detected while reading an enumerated value (line 6).
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio17.TextGrid

asserterror "exi sts" is not a value of the enumerated type.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio18.TextGrid

asserterror Found strange text while reading an enumerated value in text (line 6).
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio19.TextGrid

asserterror "exi" is not a value of the enumerated type.
...'newline$'TextGrid not read.
...'newline$'Data not read from text file
Read from file... texio/texio20.TextGrid

if windows
	asserterror Cannot open file “'defaultDirectory$'\texio\texio99.TextGrid”.
	Read from file... texio/texio99.TextGrid
else
	asserterror Cannot open file “'defaultDirectory$'/texio/texio99.TextGrid”.
	Read from file... texio/texio99.TextGrid
endif

printline OK
