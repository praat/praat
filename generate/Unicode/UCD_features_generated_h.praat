# UCD_features_generated_h.praat
# Paul Boersma 20190617

Text writing preferences: "UTF-8"

table = Read Table from semicolon-separated file: "UnicodeData.txt"
numberOfRows = Get number of rows

stopwatch
outfile$ = "../../kar/UCD_features_generated.h"
writeFileLine: outfile$, "/* This file was generated automatically by generate/Unicode/UCD_features_generated_h.praat */"

previousLineCodePoint = -1
for irow from 1 to numberOfRows
	code$ = Get value: irow, "code"
	lineCodePoint = number ("0x" + code$)
	goto finished lineCodePoint > 0x2FFFF
	while lineCodePoint > previousLineCodePoint + 1
		previousLineCodePoint += 1
		if isFirst
			line$ = "/* " + hexadecimal$ (previousLineCodePoint, 4) + " " + (name$ - "First>" + "Next>") + " */" + newline$
			line$ += tab$ + "{ " + categoryFeature$ +
			... ", 0x" + hexadecimal$ (previousLineCodePoint, 4) + ", 0x" + hexadecimal$ (previousLineCodePoint, 4) +
			... ", 0x" + hexadecimal$ (previousLineCodePoint, 4) + ", nullptr, '\0', '\0' }," + newline$
		else
			line$ = "/* " + hexadecimal$ (previousLineCodePoint, 4) + " UNASSIGNED */" + newline$
			line$ += tab$ + "{ kUCD_UNASSIGNED " +
			... ", 0x" + hexadecimal$ (previousLineCodePoint, 4) + ", 0x" + hexadecimal$ (previousLineCodePoint, 4) +
			... ", 0x" + hexadecimal$ (previousLineCodePoint, 4) + ", nullptr, '\0', '\0' }," + newline$
		endif
		appendFile: outfile$, line$
	endwhile
	name$ = Get value: irow, "name"
	isFirst = ( index (name$, " First>") <> 0 )
	categoryCode$ = Get value: irow, "category"
	alternativeName$ = Get value: irow, "dum1"
	upper$ = Get value: irow, "upper"
	lower$ = Get value: irow, "lower"
	title$ = Get value: irow, "title"
	decomp$ = Get value: irow, "decomp"
	symbol$ = if lineCodePoint >= 0xD800 and lineCodePoint <= 0xDFFF
	... then " SURROGATE " else unicode$ (lineCodePoint) fi
	line$ = "/* " + code$ + " " +
	... if name$ <> "<control>" then "(x" + symbol$ + "x) " else "" fi +
	... name$ + " " +
	... if alternativeName$ <> "" then "(" + alternativeName$ + ") " else "" fi +
	... "*/" + newline$
	majorCategoryCode$ = mid$ (categoryCode$, 1, 1)
	minorCategoryCode$ = mid$ (categoryCode$, 2, 1)
	if majorCategoryCode$ = "L"
		categoryFeature$ = "mUCD_WORD_CHARACTER"
		if minorCategoryCode$ = "u"
			categoryFeature$ += " | mUCD_UPPERCASE_LETTER"
		elsif minorCategoryCode$ = "l"
			categoryFeature$ += " | mUCD_LOWERCASE_LETTER"
		elsif minorCategoryCode$ = "t"
			categoryFeature$ += " | mUCD_TITLECASE_LETTER"
		elsif minorCategoryCode$ = "m"
			categoryFeature$ += " | mUCD_MODIFIER_LETTER"
		elsif minorCategoryCode$ = "o"
			categoryFeature$ += " | mUCD_OTHER_LETTER"
		else
			exitScript: "Unknown letter category code in row ", irow, "."
		endif
	elsif majorCategoryCode$ = "M"
		categoryFeature$ = "mUCD_WORD_CHARACTER"
		if minorCategoryCode$ = "n"
			categoryFeature$ += " | mUCD_NONSPACING_MARK"
		elsif minorCategoryCode$ = "c"
			categoryFeature$ += " | mUCD_SPACING_MARK"
		elsif minorCategoryCode$ = "e"
			categoryFeature$ += " | mUCD_ENCLOSING_MARK"
		else
			exitScript: "Unknown mark category code in row ", irow, "."
		endif
	elsif majorCategoryCode$ = "N"
		categoryFeature$ = "mUCD_WORD_CHARACTER"
		if minorCategoryCode$ = "d"
			categoryFeature$ += " | mUCD_DECIMAL_NUMBER"
		elsif minorCategoryCode$ = "l"
			categoryFeature$ += " | mUCD_LETTER_NUMBER"
		elsif minorCategoryCode$ = "o"
			categoryFeature$ += " | mUCD_OTHER_NUMBER"
		else
			exitScript: "Unknown number category code in row ", irow, "."
		endif
	elsif majorCategoryCode$ = "P"
		if minorCategoryCode$ = "c"
			categoryFeature$ = "mUCD_WORD_CHARACTER | mUCD_CONNECTOR_PUNCTUATION"
		elsif minorCategoryCode$ = "d"
			categoryFeature$ = "mUCD_DASH_PUNCTUATION"
		elsif minorCategoryCode$ = "s"
			categoryFeature$ = "mUCD_OPEN_PUNCTUATION"
		elsif minorCategoryCode$ = "e"
			categoryFeature$ = "mUCD_CLOSE_PUNCTUATION"
		elsif minorCategoryCode$ = "i"
			categoryFeature$ = "mUCD_INITIAL_PUNCTUATION"
		elsif minorCategoryCode$ = "f"
			categoryFeature$ = "mUCD_FINAL_PUNCTUATION"
		elsif minorCategoryCode$ = "o"
			categoryFeature$ = "mUCD_OTHER_PUNCTUATION"
		else
			exitScript: "Unknown punctuation category code in row ", irow, "."
		endif
	elsif majorCategoryCode$ = "S"
		if minorCategoryCode$ = "m"
			categoryFeature$ = "mUCD_MATH_SYMBOL"
		elsif minorCategoryCode$ = "c"
			categoryFeature$ = "mUCD_CURRENCY_SYMBOL"
		elsif minorCategoryCode$ = "k"
			categoryFeature$ = "mUCD_MODIFIER_SYMBOL"
		elsif minorCategoryCode$ = "o"
			categoryFeature$ = "mUCD_OTHER_SYMBOL"
		else
			exitScript: "Unknown symbol category code in row ", irow, "."
		endif
	elsif majorCategoryCode$ = "Z"
		if minorCategoryCode$ = "s"
			if lineCodePoint = 0x00A0 or lineCodePoint = 0x202F
				categoryFeature$ = "mUCD_NON_BREAKING_SPACE"
			else
				categoryFeature$ = "mUCD_BREAKING_SPACE"
			endif
		elsif minorCategoryCode$ = "l"
			categoryFeature$ = "mUCD_LINE_SEPARATOR"
		elsif minorCategoryCode$ = "p"
			categoryFeature$ = "mUCD_PARAGRAPH_SEPARATOR"
		else
			exitScript: "Unknown separator category code in row ", irow, "."
		endif
	elsif majorCategoryCode$ = "C"
		if minorCategoryCode$ = "c"
			categoryFeature$ = "mUCD_CONTROL"
			#
			# Special cases that diverge from UnicodeData.txt.
			#
			if lineCodePoint = 0
				categoryFeature$ += " | mUCD_NULL"
			elsif lineCodePoint = 9   ; tab
				categoryFeature$ += " | mUCD_BREAKING_SPACE"
			elsif (lineCodePoint >= 10 and lineCodePoint <= 13) or lineCodePoint = 0x0085   ; line feed, vertical tab, form feed, carriage return, next line
				categoryFeature$ += " | mUCD_LINE_SEPARATOR"
			endif
		elsif minorCategoryCode$ = "f"
			categoryFeature$ = "mUCD_FORMAT"
		elsif minorCategoryCode$ = "s"
			categoryFeature$ = "kUCD_UNASSIGNED"
		elsif minorCategoryCode$ = "o"
			categoryFeature$ = "mUCD_PRIVATE_USE"
		elsif minorCategoryCode$ = "n"
			categoryFeature$ = "kUCD_UNASSIGNED"
		else
			exitScript: "Unknown other category code in row ", irow, "."
		endif
	else
		categoryFeature$ = "0"
	endif
	lower$ = if lower$ = "" then code$ else lower$ fi
	upper$ = if upper$ = "" then code$ else upper$ fi
	title$ = if title$ = "" then code$ else title$ fi
	if decomp$ = ""
		decomp$ = "nullptr"
	elsif index (decomp$, "<")
		decomp$ = "nullptr"
	elsif index (decomp$, " ") = 0
		decomp$ = "nullptr"
	else
		decomp$ = "U""\u" + replace$ (decomp$, " ", "\u", 0) + """"
	endif
	line$ += tab$ + "{ " + categoryFeature$ +
	... ", 0x" + upper$ + ", 0x" + lower$ + ", 0x" + title$ + ", " + decomp$ + ", '\0', '\0' }," + newline$
	appendFile: outfile$, line$
	previousLineCodePoint = lineCodePoint
endfor

label finished
appendInfoLine: stopwatch