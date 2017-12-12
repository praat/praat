# test_SVD.praat
# djmw 20171208

appendInfoLine: "test_SVD.praat"

n = 4
eps = 1.5e-5
appendInfoLine: tab$, "Hilbert matrix order 4"
t = Create TableOfReal: "hilbert", n, n
for i to n
	for j to n
		Set value: i, j, 1 / (i + j - 1)
	endfor
endfor
svd = To SVD

utab = Extract left singular vectors
selectObject: svd
vtab = Extract right singular vectors
selectObject: svd
dvec = Extract singular values
u$= " -0.792608   0.582076  -0.179186  -0.029193 "
... + " -0.451923  -0.370502   0.741918   0.328712 "
... + " -0.322416  -0.509579  -0.100228  -0.791411 "
... + " -0.252161  -0.514048  -0.638283   0.514553 "
v$ = "-0.792608   0.582076  -0.179186  -0.029193 "
... + " -0.451923  -0.370502   0.741918   0.328712 "
... + " -0.322416  -0.509579  -0.100228  -0.791411 "
... + " -0.252161  -0.514048  -0.638283   0.514553"
d# = {1.5002, 1.6914e-01, 6.7383e-03, 9.6702e-05}

appendInfoLine: tab$, tab$, "UDV' test equality of U"
@string_to_table: u$, n, n
utab_octave = selected ("TableOfReal")
appendInfoLine: tab$, tab$, "UDV' test equality of V"
@string_to_table: v$, n, n
vtab_octave = selected ("TableOfReal")

@check_tors: utab, utab_octave, eps
@check_tors: vtab, vtab_octave, eps

removeObject: utab, utab_octave, vtab, vtab_octave, svd, t, dvec

appendInfoLine: tab$, "reconstruct 6x2 matrix"
@test_reconstruction: 6, 2, eps

appendInfoLine: tab$, "reconstruct 2x6 matrix"
@test_reconstruction: 2, 6, eps

appendInfoLine: tab$, "reconstruct 30x500 matrix"
@test_reconstruction: 30, 500, eps

appendInfoLine: "test_SVD.praat OK"

procedure test_reconstruction: .nrows, .ncols, .eps
	.t = Create TableOfReal: "t", .nrows, .ncols
	Formula: "randomUniform (-1,1)"
	.svd = To SVD
	.tr = To TableOfReal: 1, 0
	@check_tors: .t, .tr, .eps
	removeObject: .t, .svd, .tr
endproc

procedure check_tors: .tor1, .tor2, .eps
	selectObject: .tor1
	.nrows = Get number of rows
	.ncols = Get number of columns
	for .i to .nrows
		for .j to .ncols
			.d =abs ((object[.tor1, .i, .j]-object[.tor2, .i, .j])/object[.tor2, .i, .j])
			assert .d < .eps; ['.i','.j']: '.tor1', '.tor2', '.d'
		endfor
	endfor
endproc

procedure string_to_table: .string$, .nrows, .ncols
	.s = Create Strings as tokens: .string$
	.numberOfTokens = Get number of strings
	assert .numberOfTokens == .nrows * .ncols
	.t = Create TableOfReal: "t", .nrows, .ncols
	for .i to .nrows
		for .j to .ncols
			selectObject: .s
			.val$ = Get string: (.i-1)*.ncols + .j
			selectObject: .t
			Set value: .i, .j, number (.val$)
		endfor
	endfor
	removeObject: .s
	selectObject: .t
endproc


