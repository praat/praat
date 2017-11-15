# test_ActivationList.praat
# djmw 20151020, 20160524

appendInfoLine: "test_ActivationList.praat"

appendInfoLine: tab$ , "Read old format from disk"
old = Read from file: "old_type.Activation"
removeObject: old
appendInfoLine: tab$ , "Read old format OK"

@test_with_old_type

procedure test_with_old_type
	for irun to 2
		nrows = 10
		for ncols to 10
			tab = Create simple Matrix: "act", nrows, ncols, "0"
			act = To Activation
			Formula: ~ randomUniform (0, 0.99)
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
endproc
		
appendInfoLine: "test_ActivationList.praat OK"


