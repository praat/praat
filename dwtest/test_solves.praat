# test_solves.praat
# djmw 20191201

appendInfoLine: "test_solves.praat"

@test_weakly_example
@test_weakly: 10
@test_nonnegative
appendInfoLine: "test_solves.praat OK"

procedure test_weakly_example
	appendInfoLine: tab$, "solveWeaklyConstrained# (f##, phi#, alpha, delta)"
	appendInfoLine: tab$, "Example J. ten Berge (1991), page 606"
	f## = 	{{ 4,  1,  0.5}, 
	...		 { 4, -1, -0.5},
	...		{-4,  1, -0.5},
	...		{-4, -1,  0.5},
	...		{ 2,  0,     0},
	...		{-2,  0,     0} }

	phi# = {-1, -1, -1, -1, -1, 1 }
	alpha = 6
	delta = 2 / 3
	result# = solveWeaklyConstrained# (f##, phi#, alpha, delta)
	inner = inner (result#, result#)

	diff = abs (inner - 7/12) 
	assert diff < 1.0e-14 ; 'diff'
	diff = abs (result#[1] - (- 4/71))
	assert diff < 1.0e-14; 'diff'
	diff = abs (result#[2] - 0) 
	assert abs (result#[2]) < 1.0e-14; 'diff'
	diff = abs (result#[3] - sqrt (35095/60492))
	assert diff < 1.0e-14; 'diff'
	appendInfoLine: tab$, tab$, inner, "; ", result#
	appendInfoLine: tab$, "Force norm to be one alpha = 1e30, delta=1.0"
	alpha = 1e30
	delta=1.0
	result# = solveWeaklyConstrained# (f##, phi#, alpha, delta)
	inner = inner (result#, result#)
	appendInfoLine: tab$, tab$, "norm(result)=", inner
	diff = abs (inner - 1.0)
	assert diff < 1.0e-14; 'diff'
endproc

procedure test_nonnegative
	appendInfoLine: tab$ , "solveNonnegative# (m##, v#, maximumnumberOfIterations, tolerance, info)"
	a## = {{-4, 2, 2}, {2,4, 2}, {1,1,1},{2,-1,3}}
	y# = {1,2,1,3}
	; octave says:
	; A = [-4 2 2; 2 4 2; 1 1 1; 2 -1 3]
	; b = [1; 2; 1; 3]
	; x =  lsqnonneg (A, b)
	; x =[0.17687; 0.0000; 0.85941]
	; 
	solution# = {0.17687, 0.0, 0.85941}
	result# = solveNonnegative# (a##, y#,  100, 1e-17, 2)
	;appendInfoLine: result#
	dif = norm (result# - solution#)
	assert dif < 1e-5
	a## = {{-4, 2, 2}, {2,4, 2}, {1,1,1},{2,-1,3}}
	y# = {1,2,1,3}
	result3# = solveNonnegative# (a##, y#,  3, 1e-17, 2)
	;appendInfoLine: result#
	result# = solveNonnegative# (a##, y#,  result3#, 100, 1e-17, 1)
	;appendInfoLine: result#
	dif = norm (result# - solution#)
	assert dif < 1e-6
endproc

procedure test_weakly: .numberOfTries
	appendInfoLine: tab$ , "Random size matrices, randomUniform (-1,1) data"
	for .itry to .numberOfTries
		.nrow = randomInteger (2, 100)
		.ncol = randomInteger (2, .nrow)
		.a## = randomUniform## (.nrow, .ncol, -1, 1)
		.b# = randomUniform# (.nrow, -1,1)
		.alpha = randomUniform (0, 10)
		.delta = randomUniform (0, 1)
		result# = solveWeaklyConstrained# (.a##, .b#,  .alpha, .delta)
		appendInfoLine: tab$, tab$, .nrow, "x", .ncol, " alpha=", .alpha, "; delta=", .delta
	endfor
endproc