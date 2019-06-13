# test_TableOfReal_extensions.praat
# djmw 20151023, 20180911

eps = 2.3e-16

appendInfoLine: "test_TableOfReal_extensions.praat"

@means_by_row_labels
@centre_rows
@centre_columns
@normalize

appendInfoLine: "test_TableOfReal_extensions.praat OK"

procedure means_by_row_labels
	appendInfoLine: tab$, "means_by_row_labels"
	.nrows = 100
	.ncols = 3
	.tab1 = Create TableOfReal: "tab", .nrows, .ncols
	for .i to .nrows
		if (.i mod 2) = 0
			Set row label (index): .i, "even"
		else
			Set row label (index): .i, "odd"
		endif
	endfor
	Formula: ~ if (row mod 2) = 0 then col else 2*col fi
	.tabm = To TableOfReal (means by row labels): "no"
	.nrowsm = Get number of rows
	.ncolsm = Get number of columns
	assert .nrowsm = 2
	assert .ncolsm = .ncols
	for .i to .nrowsm
		.rowLabel$ = Get row label: .i
		for .j to .ncols
			if .rowLabel$ = "even"
				.dif = abs (object [.tabm, .i, .j] - .j)
			else
				.dif = abs (object [.tabm, .i, .j] - .j*2)
			endif
			assert .dif <= eps
		endfor
	endfor
	removeObject: .tab1, .tabm	
endproc

procedure centre_rows
	appendInfoLine: tab$, "centre_rows"
	.eps = 3*2.3e-16
	.nrow = 20
	.ncol = 3
	.tab = Create TableOfReal: "tab", .nrow, .ncol
	Formula: ~ if col=1 then row else if col = 2 then 0 else -row fi fi
	Formula: ~ self+1
	Centre rows
	for .irow to .nrow
		.dif = abs (object [.tab, .irow, 1] - .irow)  / .irow
		assert .dif < .eps
		.dif = abs (object [.tab, .irow, 3] + .irow)  / .irow
		assert .dif < .eps
		assert abs (object [.tab, .irow, 2]) < .eps
	endfor
	removeObject: .tab
endproc

procedure centre_columns
	appendInfoLine: tab$, "centre_columns"
	.eps = 3*2.3e-16
	.nrow = 3
	.ncol = 20
	.tab = Create TableOfReal: "tab", .nrow, .ncol
	Formula: ~ if row=1 then col else if row = 2 then 0 else -col fi fi
	Formula: ~ self+1
	Centre columns
	for .icol to .ncol
		.dif = abs (object [.tab, 1, .icol] - .icol)  / .icol
		assert .dif < .eps
		.dif = abs (object [.tab, 3, .icol] + .icol)  / .icol
		assert .dif < .eps
		assert abs (object [.tab, 2, .icol]) < .eps
	endfor
	removeObject: .tab
endproc

procedure normalize
	# simple check
	appendInfoLine: tab$, "normalize"
	.nrow = 10
	.ncol = 10
	.tab = Create TableOfReal: "tab", .nrow, .ncol
	Formula: ~ row
	.norm = Get table norm
	# sum(k,1,n, k^2)=n(n+1)(2n+1)/6
	.sumsq_column = .nrow *(.nrow+1)*(2*.nrow+1)/6
	.normwanted = sqrt (.ncol*.sumsq_column)
	.eps =1e-15
	assert 1-.eps < .norm/.normwanted && .norm/.normwanted < 1+.eps; before
	.nall = Copy: "all"
	Normalize table: 1.0
	.norm = Get table norm
	assert 1-.eps < .norm && .norm < 1+.eps; after
	selectObject: .tab
	.rows = Copy: "rows"
	Normalize rows: 1.0
	.norm = Get table norm
	.normwanted = sqrt (10)
	assert 1-.eps < .norm/.normwanted && .norm/.normwanted < 1+.eps; rows
	selectObject: .tab
	Normalize columns: 1.0
	.norm = Get table norm
	.normwanted = sqrt (10)
	assert 1-.eps < .norm/.normwanted && .norm/.normwanted < 1+.eps; rows
	removeObject: .tab, .nall, .rows	
endproc
