# test_Polynomial.praat
# djmw 20160509

printline test_Polynomial

@test_roots
@test_products

printline test_Polynomial OK

procedure test_roots
	# random polynomials can behave very wildly. Therefor we are not 
	# too strictly in checking the differences between generated and 
	#measured roots
	.eps1 = 1e-6
	.eps2 = 1e-6
	printline ...Roots
	for .i to 20
		.numberOfRoots = randomInteger (2, 10)
		.rootsTable = Create TableOfReal: "r", .numberOfRoots, 1
		.xmin = randomUniform (-2, 0)
		.xmax = randomUniform (0, 2)
		Formula: ~ randomUniform (.xmin, .xmax)
		Sort by column: 1, 0
		.roots$ = ""
		for .j to .numberOfRoots
			.roots[.j]  = Get value: .j, 1
			.roots$ = .roots$ + string$ (.roots[.j]) + if .j == .numberOfRoots then "" else " " fi
		endfor
		.p = Create Polynomial from real zeros:  "p", .xmin, .xmax, .roots$
		# divide by a root
		for .j to .numberOfRoots
			.rootsj = .roots[.j]
			.remainder = Get remainder: .rootsj
			.test = abs (.remainder /.rootsj)
			assert .test < .eps1; '.remainder' '.rootsj' 
		endfor
		.roots [0] = .xmin
		.roots [.numberOfRoots + 1] = .xmax
		for .j to .numberOfRoots
			.xmini = randomUniform (.roots[.j-1], .roots[.j])
			.xmaxi = randomUniform (.roots[.j], .roots[.j+1])
			.root = Get one real root: .xmini, .xmaxi
			.rootsj = .roots[.j]
			.dif = abs (.roots[.j] - .root)
			assert .dif < .eps2 * abs (.roots[.j]); '.root' '.rootsj' .j
		endfor
		removeObject: .p, .rootsTable
	endfor
endproc

procedure test_products
	.eps = 1e-15
	.p = Create Polynomial from product terms: "p", -3, 3, "1 2 -1 -2"
	.coefs$ = "1 0 -1 0 0 0 -1 0 1"
	.strings = Create Strings as tokens: .coefs$
	.ntokens = Get number of strings
	for .i to .ntokens
		selectObject: .strings
		.coef$ = Get string: .i
		selectObject: .p
		.coef = Get coefficient: .i
		assert abs (number (.coef$) - .coef) < .eps; '.coef'
	endfor
	removeObject: .p, .strings
endproc

