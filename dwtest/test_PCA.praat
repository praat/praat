# test_PCA.praat
# djmw 20110525, 20211128

appendInfoLine: "test_PCA.praat"

call test_pca_simple
@test_projections

appendInfoLine: "test_PCA.praat OK"

procedure create_reference_TableOfReal 
	# 5 points,
	#  	p1, p2,p3 on a line through the origin with an angle of pi/6 with variance 6
	#	p4, p5 orthogonal with variance 2
	#
	.npoints = 5
	.t = Create TableOfReal... .t .npoints 2
	Set value... 1 1 -sqrt(2)
	Set value... 1 2 -1
	Set value... 2 1  0
	Set value... 2 2 0
	Set value... 3 1 sqrt(2)
	Set value... 3 2  1
	Set value... 4 1 -1/sqrt(3)
	Set value... 4 2 sqrt(2/3)
	Set value... 5 1 1/sqrt(3)
	Set value... 5 2 -sqrt(2/3)
endproc

procedure test_pca_simple
	appendInfoLine: tab$, "test_pca_simple"
	.tol = 1e-12
	@create_reference_TableOfReal
	.tor = selected ("TableOfReal")
	.nrows = Get number of rows
	.ncols = Get number of columns
	.pca = To PCA
	.numberOfEigenvectors = Get number of eigenvectors
	assert .numberOfEigenvectors = .ncols
	appendInfoLine:  tab$, tab$, "Eigenvalues"
	.eigenvalue1 = Get eigenvalue... 1
	.eigenvalue2 = Get eigenvalue... 2
	assert abs (.eigenvalue1 - 6 / (.nrows - 1)) < .tol
	assert abs (.eigenvalue2 - 2 / (.nrows - 1)) < .tol

	appendInfoLine:  tab$, tab$, "Fraction variance accounted for"
	.fvaf = Get fraction variance accounted for... 1 1
	assert abs(.fvaf - 6/8)< .tol

	appendInfoLine:  tab$, tab$, "Eigenvectors should be orthogonal"
	.ev11 = Get eigenvector element... 1 1
	.ev12 = Get eigenvector element... 1 2
	.ev21 = Get eigenvector element... 2 1
	.ev22 = Get eigenvector element... 2 2
	.inprod = .ev11*.ev21+.ev12*.ev22

	assert abs (.inprod) < .tol

	removeObject: .pca, .tor
	appendInfoLine:  tab$, "test_pca_simple OK"
endproc

procedure test_projections
	appendInfoLine: tab$, "test PCA & TableOfReal projections"
	.tol = 1e-12
	@create_reference_TableOfReal
	.tor = selected ("TableOfReal")
	.nrows = Get number of rows
	.ncols = Get number of columns
	.pca = To PCA
	.numberOfEigenvectors = Get number of eigenvectors
	.evectorLength = Get eigenvector dimension

	appendInfoLine:  tab$, tab$, "PCA & TableOfReal: Project rows"
	selectObject: .tor, .pca
	.projection1 = To TableOfReal (project rows): 0
	.ncols_p1 = Get number of columns
	assert .ncols_p1 = 2
	selectObject: .tor, .pca
	.projection2 = To TableOfReal (project rows): 1
	.ncols_p2 = Get number of columns
	assert .ncols_p2 = 1
	
	selectObject: .tor
	.minuscolumn = Copy: "1"
	Remove column (index): 2
	.ncolminus = Get number of columns
	selectObject: .minuscolumn, .pca
	asserterror The number of columns in the TableOfReal ('.ncolminus') should match the length of the eigenvectors of the PCA ('.evectorLength').
	.projection3 = To TableOfReal (project rows): 0

	appendInfoLine:  tab$, tab$, "PCA & TableOfReal: To Configuration"
	selectObject: .tor, .pca
	.configuration1 = To Configuration: 0
	.ncols_c1 = Get number of columns
	assert .ncols_c1 = 2
	selectObject: .tor, .pca
	.configuration2 = To Configuration: 1
	.ncols_c2 = Get number of columns
	assert .ncols_c2 = 1
	selectObject: .minuscolumn, .pca
	asserterror The number of columns in the TableOfReal ('.ncolminus') should match the length of the eigenvectors of the PCA ('.evectorLength').
	.configuration3 = To Configuration: 0

	appendInfoLine:  tab$, tab$, "PCA & TableOfReal: Z-scores"
	selectObject: .tor, .pca
	.zscore1 = To TableOfReal (z-scores): 0
	.ncols_z1 = Get number of columns
	assert .ncols_z1 = 2
	selectObject: .tor, .pca
	.zscore2 = To TableOfReal (z-scores): 1
	.ncols_z2 = Get number of columns
	assert .ncols_z2 = 1
	selectObject: .minuscolumn, .pca
	asserterror The number of columns in the TableOfReal ('.ncolminus') should match the length of the eigenvectors of the PCA ('.evectorLength').
	.zscore3 = To TableOfReal (z-scores): 0

	appendInfoLine:  tab$, tab$, "TableOfReal reconstruction from PCA+ Configuration"
	selectObject: .configuration2, .pca
	.reconstruction = To TableOfReal (reconstruct)
	.columns3 = Create TableOfReal: "3", 5, 3
	.toomanyColumns = To Configuration
	selectObject: .pca, .toomanyColumns
	asserterror The number of columns in the Configuration should not exceed the number of eigenvectors in the PCA.
	.tor2 = To TableOfReal (reconstruct)

	removeObject: .projection2, .projection1, .configuration1, 
	... .configuration2, .minuscolumn, .zscore1, .zscore2, .pca, .tor,
	... .reconstruction,  .columns3, .toomanyColumns
	
	appendInfoLine:  tab$, "test PCA & TableOfReal projections OK"

endproc



