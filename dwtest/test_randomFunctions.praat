# test_randomFunctions.praat
# djmw 20121211, 20191227


appendInfoLine: "test_randomFunctions.praat"

Erase all
Font size: 10
@test_randomBiniomial: 1/6, 100, 1, 1
@test_randomBiniomial: 1/10, 50, 2, 0
@test_randomGamma: 3, 0.02, 3
@test_randomGamma: 5, 0.02, 4
sleep: 2	; some time to look at the drawing

appendInfoLine:  "test_randomFunctions.praat OK"

procedure test_randomBiniomial: .p, .n, .ipicture, .info
	appendInfoLine: tab$,  "randomBinomial (", .p, ",", .n, ")"
	.np = .n * .p
	.numberOfRuns = 100000
	if .info <> 0
		appendInfoLine:   tab$, tab$, .numberOfRuns, " runs with randomBinomial (", .p, ",", .n, ")"
		appendInfoLine:   tab$, tab$, "We expect values between 0 and ", .n, " with a mean near n * p = ", .np
		appendInfoLine:   tab$, tab$, "Bin 1 counts number of times randomBinomial return 0, bin 2 number of times randomBinomial returns 1,  etc."
		appendInfoLine:   tab$, tab$, "The probability q of the values in bin i equals binomialP (p, i - 1 , n) - binomialP (p, i - 2 , n)"
		appendInfoLine:   tab$, tab$,  "In bin i we expect numberOfRuns * q (=", .numberOfRuns, " * q  values"
	endif
	.mat = Create simple Matrix: "m", .numberOfRuns, 1, "randomBinomial (.p, .n)"
	Select outer viewport: 0, 6, (.ipicture -1) * 3, .ipicture * 3
	.numberOfBins = 40
	Draw distribution: 0, 0, 0, 0, 0, .numberOfBins, .numberOfBins, 0, 0, "yes"
	One mark bottom: .np, "no", "yes", "yes", fixed$ (.np, 2)
	Text top: "no", "randomBinomial (1/6, 100)"
	.info$ = Picture info
	.ymax = extractNumber (.info$, "Axis top:")
	Text: 0.65 * .numberOfBins, "left", 0.75 * .ymax, "Half", "Top expected at x = " + fixed$ (.np, 2)
	.table = Create TableOfReal: "t", 3, .n+1
	for .i to .numberOfRuns
		.nb = randomBinomial (.p, .n)
		.val = Get value: 1, .nb+1
		.val += 1
		Set value: 1, .nb+1, .val
	endfor

	for .i from 2 to .numberOfBins
		.val = Get value: 1, .i
		.q = binomialP (.p, .i - 1 , .n) - binomialP (.p, .i - 2 , .n)
		.expect = round (.numberOfRuns * .q)
		.diff = .val - .expect
		Set value: 2, .i,  .expect
		Set value: 3, .i, .diff
		if .info <> 0
			appendInfoLine: tab$, tab$, "bin = ", .i, ", diff = ", .diff, "  (expected = ", .expect,  " q = ", .q
		endif
	endfor

	removeObject: .mat, .table
endproc

procedure test_randomGamma: .alpha, .beta, .ipicture
	appendInfoLine: tab$,  "randomGamma (", .alpha, ",", .beta, ")"
	.nrow = 1000
	.ncol = 100
	.gamma = Create simple Matrix: "gamma", .nrow, .ncol, "randomGamma (.alpha, .beta)"
	Select outer viewport: 0, 6, (.ipicture -1) * 3, .ipicture * 3
	.numberOfBins = 100
	.maximumValue = 750
	Draw distribution: 0, 0, 0, 0, 0, .maximumValue, .numberOfBins, 0, 0, "yes"
	.xOfMaximum = (.alpha - 1) / .beta
	One mark bottom: .xOfMaximum, "yes", "yes", "yes", ""
	.numberOfCells = .nrow * .ncol
	Draw function: 0, .maximumValue, 1000, "(.maximumValue/.numberOfBins)*
	... .numberOfCells*exp(-lnGamma(.alpha))*.beta^.alpha *x^(.alpha-1)*exp(-.beta*x)"
	.topText$ = "randomGamma (" + string$( .alpha) + ", " + string$ (.beta) +  ")"
	Text top: "no", .topText$
	.info$ = Picture info
	.ymax = extractNumber (.info$, "Axis top:")
	Text: .maximumValue / 2, "left", 0.75 * .ymax, "Half", "Top expected at x = " + string$ (.xOfMaximum)
	removeObject: .gamma
endproc

