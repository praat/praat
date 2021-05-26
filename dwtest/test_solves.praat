# test_solves.praat
# djmw 20191201, 20210526

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
	appendInfoLine: tab$, "Example fromR with library (nnls)"
	# next example is from R with library
	# set.seed(1)
	# n <- 8; p <- 4
	# x <- matrix(rnorm(n * p), nrow = n)
	# y <- x %*% matrix(rep(c(1, -1), length.out = p), ncol = 1) + rnorm(n)
	# library(nnls)
	# mod1 <- nnls(x, y)
	# mod1$x 
	a## = { {-0.6264538,  0.57578135, -0.01619026,  0.61982575},
	... {0.1836433, -0.30538839,  0.94383621, -0.05612874},
	... { -0.8356286,  1.51178117,  0.82122120, -0.15579551},
	... {  1.5952808,  0.38984324,  0.59390132, -1.47075238},
	... { 0.3295078, -0.62124058,  0.91897737, -0.47815006},
	... { -0.8204684, -2.21469989, 0.78213630,  0.41794156},
	... { 0.4874291,  1.12493092,  0.07456498,  1.35867955},
	... { 0.7383247, -0.04493361, -1.98935170, -0.10278773}}
	y# = { -1.4505796, 1.4351916, -2.7474526, 2.8550967, 1.9535858, 1.6991128, -0.8215911, -0.3401299}
	solution# = {1.4907686, 0.0000000, 0.8906014, 0.0000000}
	result# = solveNonnegative# (a##, y#,  100, 1e-17, 1)
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