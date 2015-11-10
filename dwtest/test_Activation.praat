# test_Activation.praat
# djmw 20151020

appendInfoLine: "test_Activation.praat"

for irun to 2
	nrows = 10
	for ncols to 10
		tab = Create simple Matrix: "act", nrows, ncols, "0"
		act = To Activation
		Formula:  "randomUniform (0, 0.99)"
		mat = To Matrix
		for irow to nrows
			for icol to ncols
				val = Get value in cell: irow, icol
				assert 0<= val and val <= 0.99; 'irow' icol' 'val'
			endfor
		endfor
		removeObject: tab, act, mat
	endfor
endfor
		
appendInfoLine: "test_Activation.praat OK"


