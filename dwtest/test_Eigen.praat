# test_Eigen.praat
# djmw 20161116

appendInfoLine: "test_Eigen.praat"

@testInterface

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
	
appendInfoLine: "test_Eigen.praat OK"	

