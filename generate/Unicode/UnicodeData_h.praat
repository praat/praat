# UnicodeData_h.praat
# Paul Boersma, 21 June 2025

Text writing preferences: "UTF-8"

table = Read Table from semicolon-separated file: "UnicodeData 16.0.txt"
numberOfRows = Get number of rows

stopwatch
outfile$ = "UnicodeData.h"
writeFileLine: outfile$, "/* This file was generated automatically by generate/Unicode/UnicodeData_h.praat */"

for irow from 2 to numberOfRows
	code$ = Get value: irow, "code"
	name$ = Get value: irow, "name"
	if name$ = "<control>"
		dum1$ = Get value: irow, "dum1"
		if dum1$ = ""
			name$ = "CONTROL_" + code$
		else
			name$ = "CONTROL_" + dum1$
		endif
	endif
	if left$ (name$) <> "<"
		positionOfParenthesis = index (name$, "(")
		if positionOfParenthesis <> 0
			name$ = left$ (name$, positionOfParenthesis - 2)
		endif
		name$ = replace$ (name$, " ", "_", 0)
		name$ = replace$ (name$, "-", "_", 0)
		appendFileLine: outfile$,
		... "#define UNICODE_" + name$ + “  0x” + code$, newline$,
		... "#define UNITEXT_" + name$ + if length (code$) = 5 then “  U"\U000” else “  U"\u” fi + code$ + “"”
	endif
endfor
appendInfoLine: stopwatch
