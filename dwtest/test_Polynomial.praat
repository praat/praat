# test_Polynomial.praat
# djmw 20160509

appendInfoLine: "test_Polynomial.praat"
for i to 10
	@test_roots
	@test_products
endfor
appendInfoLine: "test_Polynomial.praat OK"

procedure test_roots
	# random polynomials can behave very wildly. Therefore we cannot
	# be too strict in checking the differences between generated and 
	# measured roots. 
	# Once in a while the precision is not as good as wanted and a notification will be given.
	.eps1 = 2e-6
	.eps2 = 1e-6
	appendInfoLine: tab$, "roots"
	for .i to 20
		.xmin = -2
		.xmax = randomUniform (1, 2)
		.numberOfRoots = randomInteger (2, 20)
		appendInfoLine: tab$, tab$, .i, ": ", .numberOfRoots, " random roots in [",.xmin, ",", .xmax, "]"
		.rootsTable = Create TableOfReal: "r", .numberOfRoots, 1
		Formula: ~ randomUniform (.xmin, .xmax)
		Sort by column: 1, 0
		.roots$ = ""
		for .j to .numberOfRoots
			.root [.j]  = Get value: .j, 1
			.roots$ = .roots$ + string$ (.root [.j]) + if .j == .numberOfRoots then "" else " " fi
		endfor
		.p = Create Polynomial from real zeros:  "p", .xmin, .xmax, .roots$
		# divide by a root
		for .j to .numberOfRoots
			.rootj = .root [.j]
			.remainder = Get remainder after division: .rootj
			.test = abs (.remainder / .rootj)
			assert .test < .eps1; '.remainder' '.rootj' 
		endfor
		.root [0] = .xmin
		.root [.numberOfRoots + 1] = .xmax
		.eps2 = .numberOfRoots * 1e-6

		for .j to .numberOfRoots
			.xmini = randomUniform (.root [.j-1], .root [.j])
			.xmaxi = randomUniform (.root [.j], .root [.j+1])
			.root = Get one real root: .xmini, .xmaxi
			.rootj = .root [.j]
			.dif = abs (.rootj - .root)
			if .dif > .eps2 * abs (.rootj)
				appendInfoLine: "Possible precision problem for ", .numberOfRoots, " roots: ", .roots$
				appendInfoLine: .dif, "(= abs(root-rootFound):", .rootj, "-", .root, ") for ", .j, "th  root)"
			endif
			;assert .dif < .eps2 * abs (.root [.j]); '.root' '.rootj' '.j'
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
		assert abs (number (.coef$) - .coef) < .eps; '.coef' '.coef$'
	endfor
	removeObject: .p, .strings
endproc

