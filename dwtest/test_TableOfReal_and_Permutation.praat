# test_TableOfReal_and_Permutation.praat
# djmw 20210809

appendInfoLine: tab$, "test_TableOfReal_and_Permutation.praat"

numberOfRows = 100
numberOfColumns = 10

p_rows = Create Permutation: "pr", numberOfRows, "no"
p_rows# = List values
p_cols = Create Permutation: "pc", numberOfColumns, "no"
p_cols# = List values

tor = Create TableOfReal: "t", numberOfRows, numberOfColumns
Formula: "randomUniform (0,1)"

selectObject: p_rows, tor
tor_rows = Permute rows
for irow to numberOfRows
	for icol to numberOfColumns
		assert object[tor_rows, irow, icol] = object [tor, p_rows# [irow] , icol]
	endfor
endfor
selectObject: p_cols, tor
tor_cols = Permute columns
for icol to numberOfColumns
	for irow to numberOfRows
		assert object[tor_cols, irow,icol] = object [tor, irow , p_cols#[icol]]
	endfor
endfor
removeObject: tor_cols, tor_rows, tor, p_cols, p_rows

appendInfoLine: tab$, "test_TableOfReal_and_Permutation.praat OK"

