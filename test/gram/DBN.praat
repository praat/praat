writeInfoLine: "Training two RBM's with a three-peaked distribution"
stopwatch

numberOfInputNodes = 30
numberOfMiddleNodes = 50
numberOfOutputNodes = 20
numberOfVowels = 3
mean# = { 8, 16, 23 }
sigma = 1.8
numberOfPatterns = 10000
learningRate = 0.001

my.DeepBeliefNetwork = Create DeepBeliefNetwork: "my", { numberOfInputNodes, numberOfMiddleNodes, numberOfOutputNodes }, 0

my.PatternList = Create PatternList: "patterns", numberOfInputNodes, numberOfPatterns
vowel# = zero# (numberOfPatterns)
vowel# ~ randomInteger (1, numberOfVowels)
formant# = zero# (numberOfPatterns)
formant# ~ randomGauss (mean# [vowel# [col]], sigma)
Formula: ~ 5 * exp (-0.5/sigma^2 * (col - formant# [row]) ^ 2) - 0.5
selectObject: my.DeepBeliefNetwork, my.PatternList
Learn: learningRate

appendInfoLine: "Trained in ", stopwatch, " seconds"

removeObject: my.PatternList

numberOfTestPatterns = 15
Erase all
Font size: 10
for itest to numberOfTestPatterns
	appendInfoLine: "Test pattern #", itest, ":"
	vowel = randomInteger (1, numberOfVowels)
	formant = randomGauss (mean# [vowel], sigma)
	#
	# Draw input.
	#
	Select outer viewport: 0, 3, (itest - 1) * 0.6, (itest - 1) * 0.6 + 1.0
	input.Matrix = Create simple Matrix: "input", 1, numberOfInputNodes, ~ 5 * exp (-0.5 * ((col - formant) / sigma) ^ 2) - 0.5
	stdev = Get standard deviation: 0, 0, 0, 0
	appendInfoLine: "   Energy in input layer: ", stdev
	Draw rows: 0, 0, 0, 0, -5, 5
	input.PatternList = To PatternList: 1
	#
	# Spread up and down.
	#
	selectObject: my.DeepBeliefNetwork, input.PatternList
	Apply to input: 1
	selectObject: my.DeepBeliefNetwork
	Spread up: "deterministic"
	Spread down: "deterministic"
	#
	# Draw reflection.
	#
	Select outer viewport: 3, 6, (itest - 1) * 0.6, (itest - 1) * 0.6 + 1.0
	reflection.Matrix = Extract input activities
	Draw rows: 0, 0, 0, 0, -5, 5

	removeObject: input.Matrix, input.PatternList, reflection.Matrix
endfor
