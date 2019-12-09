# test_weaklyConstrainedLinearRegression.praat
# djmw 20191201

# example page 606, table 1 from:
#	J. ten Berge (1991): "A general solution for a class of weakly constrained
# 	linear regression problems, Psychometrika 56, 601-609 ;

appendInfoLine: "test_weaklyConstrainedLinearRegression.praat"
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
appendInfoLine: tab$, inner, "; ", result#

# force ||result|| to be 1.0
alpha = 1e30
delta=1.0
result# = solveWeaklyConstrained# (f##, phi#, alpha, delta)
inner = inner (result#, result#)
appendInfoLine: tab$, "alpha very big, delta=1, norm(result)=", inner
diff = abs(inner - 1.0)
assert diff < 1.0e-14; 'diff'
appendInfoLine: "test_weaklyConstrainedLinearRegression.praat OK"
