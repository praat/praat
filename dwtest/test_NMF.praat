# test_NMF.praat
# djmw 20190312

appendInfoLine: "test_NMF.praat"
for i to 10
	ncol = 10
	nrow = randomInteger (1, 100)
	napprox = randomInteger(1,5)
	mat = Create simple Matrix: "xy", nrow, ncol, "randomUniform (1,10)"
	nmf = To NMF: napprox, 40, 1e-09, 1e-09, "RandomUniform"
	reconstruction = To Matrix
	removeObject: mat, nmf, reconstruction
	appendInfoLine: tab$, nrow, "x", ncol, ", aprox = ",  napprox
endfor
appendInfoLine: "test_NMF.praat OK"
