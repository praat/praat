# test_Strings_and_Permutation.praat
# djmw 20220928

appendInfoLine: "test_Strings_and_Permutation"
veca# = {1, 10, 11, 22, 3, 5}
vecn# = {1, 3, 5, 10, 11, 22 }

strings = Create Strings from tokens: "s", "11 10 1 22 3 5", " " 
pa = To Permutation: "Alphabetical"
plusObject: strings
stringsa = Permute strings
for i to size (veca#)
	number$ = Get string: i
	assert number (number$) = veca# [i]
endfor
removeObject: stringsa, pa

selectObject: strings
pn = To Permutation: "number-aware"
plusObject: strings
stringsn = Permute strings
for i to size (veca#)
	number$ = Get string: i
	assert number (number$) = vecn# [i]
endfor
removeObject: stringsn, pn, strings

table = Create Table with column names: "table", 100, { "factor", "data" }
Formula: "data", "randomGauss (0, 1)"
Formula: "factor", "randomUniform (1,6)"
Formula: "factor",  "veca# [self]"

removeObject: table

appendInfoLine: "test_Strings_and_Permutation OK"





