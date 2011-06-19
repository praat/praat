# test_PCA.praat
# djmw 20110525

printline test_PCA

# 5 points,
#  	p1, p2,p3 on a line throug the origin with an angle of pi/6 with variance 6
#	p4, p2, p5 orthogonal with variance 2
#
tol = 1e-12
npoints = 5
t = Create TableOfReal... t npoints 2
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
pca = To PCA
printline ... eigenvalues
ss1 = Get eigenvalue... 1
ss2 = Get eigenvalue... 2
assert abs(ss1 - 6/(npoints-1)) < tol
assert abs(ss2 - 2/(npoints-1)) < tol

printline ... fraction variance accounted for
fvaf = Get fraction variance accounted for... 1 1
assert abs(fvaf - 6/8)< tol

printline ... centroid at (0,)

printline ...eigenvectors othogonal
ev11 = Get eigenvector element... 1 1
ev12 = Get eigenvector element... 1 2
ev21 = Get eigenvector element... 2 1
ev22 = Get eigenvector element... 2 2
inprod = ev11*ev21+ev12*ev22

assert abs(inprod) < tol

plus t
Remove
printline test_PCA OK

