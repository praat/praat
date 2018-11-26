writeInfoLine: "Training a DBN with a three-peaked distribution"
stopwatch

structure# = { 30, 20 }
numberOfVowels = 3
mean# = { 8, 16, 23 }
sigma = 1.8
numberOfPatterns = 3000
learningRate = 0.001

my.Net = Create Net as deep belief network: "my", structure#, 0

my.PatternList = Create PatternList: "patterns", structure# [1], numberOfPatterns
formant# = zero# (numberOfPatterns)
formant# ~ randomGauss (mean# [randomInteger (1, numberOfVowels)], sigma)
Formula: ~ 5 * exp (-0.5/sigma^2 * (col - formant# [row]) ^ 2) - 0.5

selectObject: my.Net, my.PatternList
;Learn by layer: learningRate
Learn: learningRate

appendInfoLine: "Trained in ", stopwatch, " seconds"

for ilayer to size (structure#) - 1
	selectObject: my.Net
	weight## = Get weights: ilayer
	appendInfoLine: weight##, newline$
endfor

numberOfTestPatterns = 15
Erase all
Font size: 10
for itest to numberOfTestPatterns
	appendInfoLine: "Test pattern #", itest, ":"
	patternNumber = randomInteger (1, numberOfPatterns)
	#
	# Draw input.
	#
	Select outer viewport: 0, 3, (itest - 1) * 0.6, (itest - 1) * 0.6 + 1.0
	selectObject: my.PatternList
	Draw: patternNumber, 0.0, 0.0, -5.0, 5.0, "no"
	#
	# Spread up and down.
	#
	selectObject: my.Net, my.PatternList
	Apply to input: patternNumber
	selectObject: my.Net
	Spread up: "deterministic"
	Spread down: "deterministic"
	#
	# Draw reflection.
	#
	Select outer viewport: 3, 6, (itest - 1) * 0.6, (itest - 1) * 0.6 + 1.0
	reflection.Matrix = Extract input activities
	stdev = Get standard deviation: 0, 0, 0, 0
	appendInfoLine: "   Energy in reflection: ", stdev
	Draw rows: 0, 0, 0, 0, -5, 5
	Remove
endfor
removeObject: my.PatternList, my.Net
