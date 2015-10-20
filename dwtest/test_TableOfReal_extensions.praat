# test_TableOfReal_extensions.praat

eps = 2.3e-16

printline TableOfReal_extensions test

call means_by_row_labels
printline TableOfReal_extensions test... OK

procedure means_by_row_labels
	printline 'tab$'means_by_row_labels
	.nrows = 100
	.ncols = 3
	.tab1 = Create TableOfReal... tab .nrows .ncols
	for .i to .nrows
		if (.i mod 2) = 0
			Set row label (index)... .i even
		else
			Set row label (index)... .i odd
		endif
	endfor
	Formula... if (row mod 2) = 0 then col else 2*col fi
	.tabm = To TableOfReal (means by row labels)... no
	.nrowsm = Get number of rows
	.ncolsm = Get number of columns
	assert .nrowsm = 2
	assert .ncolsm = .ncols
	for .i to .nrowsm
		.rowLabel$ = Get row label... .i
		for .j to .ncols
			if .rowLabel$ = "even"
				.dif = abs (Object_'.tabm'[.i,.j] - .j)
			else
				.dif = abs(Object_'.tabm'[.i,.j] - .j*2)
			endif
			assert .dif <= eps
		endfor
	endfor
	removeObject: 	.tab1, .tabm	
endproc
