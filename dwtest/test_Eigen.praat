# test_Eigen.praat
# djmw 20161116, 20180829

appendInfoLine: "test_Eigen.praat"

@testInterface
eps = 1e-7

for i to 10
	@testDiagonal: i
endfor

@test2by2
@test3by3
@testgeneralSquare

procedure testInterface
	for .i to 5
		.numberOfColumns = randomInteger (3, 12)
		.tableofreal = Create TableOfReal: "t", 100, .numberOfColumns
		Formula: ~ randomGauss (0, 1)
		.pca = To PCA
		.eigen = Extract Eigen
		.numberOfEigenvalues = Get number of eigenvalues
		assert .numberOfEigenvalues == .numberOfColumns
		.dimension = Get eigenvector dimension
		assert .dimension == .numberOfColumns
		for .j to .numberOfEigenvalues
			.eigenvalue [.j] = Get eigenvalue: .j
		endfor
		for .j to .numberOfEigenvalues
			.sump = Get eigenvalue: .j
			for .k from .j to .numberOfEigenvalues
				.sum = Get sum of eigenvalues: .j, .k
				assert .sum >= .eigenvalue [.j]
			endfor
		endfor

		for .j to .numberOfEigenvalues
			for .k from .j to .dimension
				.val[.j,.k] = Get eigenvector element: .j, .k
			endfor
		endfor
		for .j to .numberOfEigenvalues
			for .k to .dimension
				.val[.k] = Get eigenvector element: .j, .k
			endfor
			Invert eigenvector: .j
			for .k to .dimension
				.valk = Get eigenvector element: .j, .k
				assert .valk == - .val[.k]
			endfor	
		endfor
		removeObject: .tableofreal, .pca, .eigen
	endfor
endproc

procedure assertApproximatelyEqual: .val1, .val2, .eps, .comment$
	.diff = abs (.val1 -.val2)
	.tekst$ = .comment$ + " " + string$ (.val1) + ", " + string$ (.val2)
	if .val1 == 0
		assert .diff < .eps; '.tekst$'
	else
		.reldif =  .diff / abs(.val1)
		assert .reldif < .eps ; '.tekst$'
	endif
endproc

procedure test2by2
	.dim = 2
	.mat = Create simple Matrix: "2x2s", .dim, .dim, "1"
	Set value: 1, 1, 2
	Set value: 2, 2, 2
	.eigenvalues# = {3, 1}
# 20180829 clumsy because we cannot yet do mat##={{},{}}
	.eigenvec1# = {1/sqrt (2), 1/sqrt (2)}
	.eigenvec2# = {-1/sqrt (2), 1/sqrt (2)}
	.eigenvectors## = zero## (.dim, .dim)
	for .j to .dim
		.eigenvectors## [1, .j] = .eigenvec1# [.j]
		.eigenvectors## [2, .j] = .eigenvec2# [.j]
	endfor
	.eigen = To Eigen
	appendInfoLine: tab$, "2x2 symmetrical"
	@testeigen: .eigen, .dim, .eigenvalues#, .eigenvectors##
	removeObject: .mat, .eigen
endproc

procedure testeigen: .eigen, .dim, .eigenvalues#, .eigenvectors##
	selectObject: .eigen
	.numberOfEigenvalues = Get number of eigenvalues
	assert .numberOfEigenvalues == .dim
	for .i to .dim
		.eval = Get eigenvalue: .i
		.comment$ = string$ (.dim) + " eigenvalue" + string$ (.i)
		@assertApproximatelyEqual: .eval, .eigenvalues# [.i], eps, .comment$
		for .j to .dim
			.evecj = Get eigenvector element: .i, .j
			.comment$ = "eigenvector[" + string$ (.i) + "] [" +string$ (.j) + "]"
			.val = .eigenvectors## [.i,.j]
			@assertApproximatelyEqual: .evecj, .val, eps, .comment$
		endfor
	endfor
endproc

procedure test3by3
	.dim = 3
	.mat = Create simple Matrix: "3x3s", .dim, .dim, "0"
	Set value: 1, 1, 2
	Set value: 2, 2, 3
	Set value: 2, 3, 4
	Set value: 3, 2, 4
	Set value: 3, 3, 9
	.eigenvalues# = {11, 2 , 1}
	.eigenvec1# = {0, 1/sqrt (5), 2/sqrt (5)}
	.eigenvec2# = {1, 0, 0}
	.eigenvec3# = {0, -2/sqrt (5), 1/sqrt (5)}
	.eigenvectors## = zero## (.dim, .dim)
	for .j to .dim
		.eigenvectors## [1, .j] = .eigenvec1# [.j]
		.eigenvectors## [2, .j] = .eigenvec2# [.j]
		.eigenvectors## [3, .j] = .eigenvec3# [.j]
	endfor
	.eigen = To Eigen
	appendInfoLine: tab$, "3x3 symmetrical"
	@testeigen: .eigen, .dim, .eigenvalues#, .eigenvectors##
	removeObject: .mat, .eigen
endproc

procedure diagonalData: .dim
	.name$ = string$(.dim) + "x" + string$ (.dim)
	.mat = Create simple Matrix: .name$, .dim, .dim, "0"
	.eigenvalues# = zero# (.dim)
	.eigenvectors## = zero## (.dim, .dim)
	for .i to .dim
		.val = .dim - .i + 1
		Set value: .i, .i, .val
		.eigenvalues# [.i] = .val
		.eigenvectors## [.i,.i] = 1
	endfor
endproc

procedure testDiagonal: .dim
	@diagonalData: .dim
	.matname$ = selected$ ("Matrix")
	.eigen = To Eigen
	appendInfoLine: tab$, .matname$ +" diagonal"
	@testeigen: .eigen, .dim, diagonalData.eigenvalues#, diagonalData.eigenvectors##
	removeObject: .eigen, diagonalData.mat
endproc

procedure testgeneralSquare
	.dim = 3
	.name$ = "3x3square"
	.mat = Create simple Matrix: .name$, .dim, .dim, "0"
	Set value: 1, 2, 1
	Set value: 2, 3, 1
	Set value: 3, 1, 1
	.given_re# = {1, -1/2, -1/2}
	.given_im# = {0, 0.5*sqrt(3), -0.5*sqrt(3)}
	Eigen (complex)
	.eigenvectors = selected ("Matrix", 1)
	.eigenvalues = selected ("Matrix", 2)
	selectObject: .eigenvalues
	.nrow = Get number of rows
	assert .nrow = 3
	# lite version of equality: check for occurrence
	# the eigenvalues of a real square matrix are not "sorted". We only know that complex conjugate eigenvalues occur
	# have the one with positive imaginary part first.
	.eval_re# = {object [.eigenvalues, 1, 1], object [.eigenvalues, 2, 1],object [.eigenvalues, 3, 1]}
	.eval_im# = {object [.eigenvalues, 1, 2], object [.eigenvalues, 2, 2],object [.eigenvalues, 3, 2]}
	if .eval_re# [1] > 1-eps and .eval_re# [1] < 1+eps
		assert .eval_re# [2] / .given_re# [2] > 1-eps and .eval_re# [2] / .given_re# [2] < 1+eps
		assert .eval_re# [3] / .given_re# [3] > 1-eps and .eval_re# [3] / .given_re# [3] < 1+eps	
	else
		assert .eval_re# [1] / .given_re# [2] > 1-eps and .eval_re# [1] / .given_re# [2] < 1+eps
		assert .eval_re# [2] / .given_re# [3] > 1-eps and .eval_re# [2] / .given_re# [3] < 1+eps	
		assert .eval_re# [3] > 1-eps and .eval_re# [3] < 1+eps and .eval_im# [3] == 0		
	endif
	selectObject: .eigenvectors
	.ncol = Get number of columns
	assert .ncol = 6	
	removeObject: .mat, .eigenvectors, .eigenvalues
endproc

appendInfoLine: "test_Eigen.praat OK"	

