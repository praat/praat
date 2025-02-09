# test_Permutation.praat
# djmw 20050710, 20070820, 20100525, 20100819, 20110418, 20220825, 20250110

appendInfoLine: "test_Permutation.praat"

@testPermutePart
@test_600_12
@rotate
@sequence
@swap
@invertp
@multiply
@jump
@distributionTest: 10, 10000
@countInversions
@testCreateSimple

appendInfoLine: "test_Permutation OK"

procedure multiply: 
	appendInfo: tab$, "Multiply"
	.numberOfPermutations = 8
	.numberOfElements = 5
	for .i to .numberOfPermutations / 2
		.p [2*.i -1] = Create Permutation: "p", .numberOfElements, "no"
		.p [2*.i] = Invert
	endfor
	selectObject: .p [1]
	for .i from 2 to .numberOfPermutations
		plusObject: .p [.i]
	endfor
	.p [.numberOfPermutations + 1] = Multiply
	for .j to .numberOfElements
		.val = Get value: .j
		assert .val = .j; p ['.j']='.val'
		appendInfo: "."
	endfor
	for .i to .numberOfPermutations + 1
		removeObject: .p [.i]
	endfor
	appendInfoLine:  ".OK"
endproc

procedure rotate
	appendInfoLine: tab$, "Rotate"
	for .i to 20
		.p = Create Permutation: "test", .i,  "yes"
	
		appendInfo: tab$, tab$, .i
		for .k to .i
			selectObject: .p
			.pk = Rotate: 0, 0, .k
			.pkk = Get value: .k + 1
			if .pkk > 0
				assert .pkk = 1 ; i='.i', k='.k'
			endif
			.pki =  Rotate: 0, 0, -.k
			for .l to .k
				.pkil = Get value: .l
				assert .pkil = .l
			endfor
			removeObject: .pk, .pki
			appendInfo: "."
		endfor
		appendInfoLine: " OK"
		removeObject: .p
	endfor
endproc

procedure invertp
	appendInfo: tab$, "Invert"
	for .i to 20
		.p = Create Permutation: "test", .i, "yes"
		.pin = Invert
		plusObject: .p
		.pm = Multiply
		for .k to .i
			.pmk = Get value: .k
			assert .pmk = .k ; i='.i', k='.k'
		endfor
		appendInfo: "."
		removeObject: .pin, .p, .pm
	endfor
	appendInfoLine: "OK"
endproc

procedure test_600_12
	appendInfo: tab$, "test_600_12"
	.p = Create Permutation: "600", 600, "yes"
	.pin = Interleave: 0, 0, 12, 1
	.pininv = Invert
	plusObject: .pin
	.pm = Multiply
	for .l to 600
		.pkil = Get value: .l
		assert .pkil = .l; l='.l'
	endfor
	removeObject: .pin, .pininv, .p, .pm
	appendInfoLine: " OK"
endproc  

procedure sequence
	appendInfo: tab$, "Sequences" 
	.n = 4
	.np = 4*3*2 - 1
	p = Create Permutation: "ps", .n, "yes"
	for .j to .np 
		for .i to .j
			Next
		endfor
		for .i to .j
			Previous
		endfor
		for .i to .n
			.val = Get value: .i
			assert .val = .i
		endfor
		appendInfo: "."
	endfor
	removeObject: p
	appendInfoLine: "OK"
endproc
 
procedure swap
	appendInfo: tab$, "Swap positions"
	.p = Create Permutation: "id", 10, "yes"
	for .i to 10
		for .j to 10
			selectObject: .p
			.ps = Copy: "swap"
			Swap positions: .i, .j
			for .k to 10
				selectObject: .p
				.pk = Get value: .k
				selectObject: .ps
				.psk = Get value: .k
				if .k <> .i and .k <> .j
					assert .pk = .psk
				endif
			endfor
			removeObject: .ps
			appendInfo: "."
		endfor
	endfor
	removeObject: .p
	appendInfoLine: "OK"
endproc

procedure jump
	appendInfo: tab$, "Jump"
	.p1 = Create Permutation: "id", 10, "yes"
	Jump: 3, 1
	.p# = List values
	assert .p# [1] = 1
	assert .p# [2] = 4
	assert .p# [3] = 7
	assert .p# [4] = 10
	assert .p# [5] = 2
	assert .p# [6] = 5
	assert .p# [7] = 8
	assert .p# [8] = 3
	assert .p# [9] = 6
	assert .p# [10] = 9
	appendInfo: "." 
	.p2 = Create Permutation: "id", 10, "yes"
	Jump: 3, 2
	.p# = List values
	assert .p# [1] = 2
	assert .p# [2] = 5
	assert .p# [3] = 8
	assert .p# [4] = 3
	assert .p# [5] = 6
	assert .p# [6] = 9
	assert .p# [7] = 1
	assert .p# [8] = 4
	assert .p# [9] = 7
	assert .p# [10] = 10
	removeObject: .p1, .p2
	appendInfoLine: ".OK"
endproc

procedure distributionTest: .size, .numberOfRepetitions
	appendInfoLine: tab$, "distributionTest"
	.permutation = Create Permutation: "p", .size, "yes"
	.distribution## = zero## (.size, .size)
	for .iperm to .numberOfRepetitions
		Permute randomly (in-place): 0, 0
		.values# = List values
		for .ipos to .size
			.distribution## [.ipos, .values# [.ipos]] += 1
		endfor
	endfor
	# the table is created but not used. Just in case you want to have a look at the distribution
	# of the numbers over the index positions.
	.tor = Create TableOfReal: "distribution", .size, .size
	for .irow to .size
		Set row label (index): .irow, string$ (.irow)
		Set column label (index): .irow, "#" + string$ (.irow)
		for .icol to .size
			Set value: .irow, .icol, .distribution## [.irow, .icol]
		endfor
	endfor
	.expected = .numberOfRepetitions / .size
	.diff## = .distribution## - .expected
	.chiSq = sum (.diff## * .diff##) / .expected^2
	.df = (.size - 1)^2 ; last value in each row and whole last row are completely determined
	.p = chiSquareQ (.chiSq, .df)
	assert .p > 0.999
	appendInfoLine: tab$, tab$, "p = ", .p, " for chiSquare = ",
	...  .chiSq, " with ", .df, " degrees of freedom."
	removeObject: .permutation, .tor
	appendInfoLine: tab$, tab$, "Realized## - expected (=", fixed$ (.expected, 0), "):"
	for .irow to .size
		appendInfo: tab$, tab$, tab$, .diff## [.irow, 1]
		for .icol from 2 to .size
			appendInfo: " ", .diff## [.irow, .icol]
		endfor
		appendInfoLine: ""
	endfor
	appendInfoLine: tab$, "distributionTest OK"
endproc
 
procedure testPermutePart
	appendInfo: tab$, "Permute part"
	.p10 = Create Permutation: "p", 10, "yes"
	.v10# = List values
	.p5 = Create Permutation: "p", 5, "no"
	.v5# = List values
	selectObject: .p10, .p5
	.p1 = Permute part: 1
	.v1# = List values
	for i to 5
		assert .v1# [i] == .v5# [i]
		assert .v1# [5 + i] = 5 + i
	endfor
	selectObject: .p10, .p5
	.p2 = Permute part: 6
	.v2# = List values
	for i to 5
		assert .v2# [i] == i
		assert .v2# [5 + i] = 5 + .v5# [i]
	endfor
	removeObject: .p2, .p1, .p5, .p10
	appendInfoLine: " OK"
endproc

procedure countInversions
	appendInfoLine: tab$, "Count inversions"
	appendInfo: tab$, tab$, "Counting"
	.size = 20
	for .i from 2 to .size
		.p = Create Permutation: "p", .i, "yes"
		for .repetions to max (5, min (10, .size * (.size - 1) / 2))
			Permute randomly (in-place): 0, 0
			.inversions = Get number of inversions
			.inversionsc = 0
			.values# = List values
			for .k to .i - 1
				.valk = .values# [.k]
				for .j from .k + 1 to .i
					if .valk > .values# [.j]
						.inversionsc += 1
					endif
				endfor
			endfor
			assert .inversions = .inversionsc
		endfor
		removeObject: .p
	endfor
	appendInfoLine: "  OK"

	.p = Create Permutation: "p", 10, "yes"
	# 1,2,3,4,5,6,7,8,9,10 -> 0
	.pi = Interleave: 0, 0, 5, 0
	# 1,6,2,7,3,8,4,9,5,10 -> 10
	.numberOfInversions = Get number of inversions
	assert .numberOfInversions = 10

	appendInfo: tab$, tab$, "List all inversions"
	.inversions_known## = {{6,2}, {6,3}, {6,4}, {6,5}, {7,3}, {7,4}, {7,5}, {8,4}, {8,5}, {9,5}}
	.inversions## = List all inversions
	assert numberOfRows (.inversions##) = .numberOfInversions
	for .irow to .numberOfInversions
		.known = .inversions_known## [.irow, 1]
		.test = .inversions## [.irow, 1]
		assert .known = .test ; col1 '.known' '.test'
		.known = .inversions_known## [.irow, 2]
		.test = .inversions## [.irow, 2]
		assert .known = .test ; col2 '.known' '.test'
	endfor
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "List random inversions"
	.nrandom = 2
	.randomInversions## = List random inversions: .nrandom
	@checkInversions: .randomInversions##, .inversions_known##;

	.nrandom = .numberOfInversions + 5
	.randomInversions## = List random inversions: .nrandom
	@checkInversions: .randomInversions##, .inversions_known##;
	appendInfoLine: " OK"

	removeObject: .pi, .p
	appendInfoLine: tab$, "Count inversions OK"
endproc

procedure checkInversions: .r##, .ref##
	for .irowr to numberOfRows (.r##)
		.found = 0
		.irow = 1
		repeat
			if .r## [.irowr, 1] = .ref## [.irow, 1] and .r## [.irowr, 2] = .ref## [.irow, 2]
				.found = 1
			endif
			.irow += 1
		until .found or .irow > numberOfRows (.ref##)
		assert .found; '.irowr'
	endfor
endproc

procedure testCreateSimple
	appendInfo: tab$ ,"testCreateSimple"
	.p1 = Create simple Permutation: "p1", {1, 2}
	asserterror There should be at least one element in a Permutation.
	.p2 = Create simple Permutation: "p2", {}
	asserterror All numbers from 1 to 3 should occur exactly once, e.g. the value 1 occurs 2 times.
	.p3 = Create simple Permutation: "p3",  {3, 1, 1}
	asserterror Your maximum number (4) should not be larger than the number of elements you supplied (3).
	.p4 = Create simple Permutation: "p4",  {3, 1, 4}
	asserterror Your minimum number should be 1 (it is 0).
	.p5 = Create simple Permutation: "p5",  {0, 1, 2}
	removeObject: .p1
	appendInfoLine: " OK"
endproc

procedure multiplyP: .a, .b
	assert .a < .b
	selectObject: .a, .b
	.a$ = selected$ ("Permutation", 1)
	.b$ = selected$ ("Permutation", 2)
	.r = Multiply
	.newName$ = .a$ + "_x_" + .b$
	Rename: .newName$
	.numberOfInversions =  Get number of inversions
	appendInfoLine: .numberOfInversions, " inversions for: ", .newName$
endproc

procedure checkDifferenceBetweenTwoPermutations
	.b = Create simple Permutation: "b", {2, 6, 7, 10, 5, 1, 4, 9, 3, 8}
	.e = Create simple Permutation: "e", {10, 9, 7, 8, 6, 5, 4, 2, 3, 1}
	.nie = Get number of inversions
	.ei = Invert
	Rename: "ei"
	selectObject: .b
	.nib = Get number of inversions
	appendInfoLine: "We want ", .nie - .nib, " inversions"
	.bi = Invert
	Rename: "bi"
	selectObject: .b
	.bc = Copy: "b"
	selectObject: .e
	.ec = Copy: "e"
	selectObject: .ei
	.eic = Copy: "ei"
	@multiplyP: .b, .e
	@multiplyP: .e, .bc
	@multiplyP: .b, .ei
	@multiplyP: .ei, .bc
	@multiplyP: .ei, .bi
	@multiplyP: .e, .bi
	@multiplyP: .bi, .ec
	@multiplyP: .bi, .eic
	
	removeObject: .b, .e, .bi, .ei, .b_x_ei, .ei_x_b, .ei_x_bi
endproc
