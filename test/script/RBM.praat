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

#
# Train first layer.
#
input1# = zero# (numberOfInputNodes)
output1# = zero# (numberOfMiddleNodes)
inbias1# = zero# (numberOfInputNodes)
outbias1# = zero# (numberOfMiddleNodes)
weight1## = zero## (numberOfInputNodes, numberOfMiddleNodes)
inrec1# = zero# (numberOfInputNodes)
outrec1# = zero# (numberOfMiddleNodes)
for idatum to numberOfPatterns
	vowel = randomInteger (1, numberOfVowels)
	formant = randomGauss (mean# [vowel], sigma)
	input1# ~ 5 * exp (-0.5/sigma^2 * (col - formant) ^ 2) - 0.5
	#
	# Spread up, with Bernoulli sampling.
	#
	output1# = sigmoid# (outbias1# + mul# (input1#, weight1##))
	output1# = randomBernoulli# (output1#)
	#
	# Spread down.
	#
	inrec1# = inbias1# + mul# (weight1##, output1#)   ; Gaussian
	#
	# Spread up.
	#
	outrec1# = sigmoid# (outbias1# + mul# (inrec1#, weight1##))
	#
	# Update.
	#
	inbias1# += learningRate * (input1# - inrec1#)
	outbias1# += learningRate * (output1# - outrec1#)
	weight1## += learningRate * (outer## (input1#, output1#) - outer## (inrec1#, outrec1#))
endfor

#
# Train second layer.
#
input2# = zero# (numberOfMiddleNodes)
output2# = zero# (numberOfOutputNodes)
inbias2# = zero# (numberOfMiddleNodes)
outbias2# = zero# (numberOfOutputNodes)
weight2## = zero## (numberOfMiddleNodes, numberOfOutputNodes)
inrec2# = zero# (numberOfMiddleNodes)
outrec2# = zero# (numberOfOutputNodes)
for idatum to numberOfPatterns
	vowel = randomInteger (1, numberOfVowels)
	formant = randomGauss (mean# [vowel], sigma)
	input1# ~ 5 * exp (-0.5/sigma^2 * (col - formant) ^ 2) - 0.5
	#
	# Spread up through first layer, with Bernoulli sampling.
	#
	output1# = sigmoid# (outbias1# + mul# (input1#, weight1##))
	output1# = randomBernoulli# (output1#)
	;; output1# owned and target
	;; output1# by reference (and target)
	;; randomBernoulli# into output1# (because no use of output1# further on)
	;; assignment is no-op
	#
	# Copy output of first layer to input of second layer.
	#
	input2# = output1#
	#
	# Spread up through second layer, with Bernoulli sampling.
	#
	output2# = sigmoid# (outbias2# + mul# (input2#, weight2##))
	output2# = randomBernoulli# (output2#)
	#
	# Spread down.
	#
	inrec2# = sigmoid# (inbias2# + mul# (weight2##, output2#))
	; inrec2# owned and target TODO
	; inbias2# by reference OK
	; weight2## by reference OK
	; output2# by reference OK
	; mul# ref and ref into target (size matches, and inrec# does not occur further on) TODO
	; + ref into target TODO
	; sigmoid# into target TODO

	#
	# Spread up.
	#
	outrec2# = sigmoid# (outbias2# + mul# (inrec2#, weight2##))
	#
	# Update.
	#
	inbias2# += learningRate * (input2# - inrec2#)
	outbias2# += learningRate * (output2# - outrec2#)
	weight2## += learningRate * (outer## (input2#, output2#) - outer## (inrec2#, outrec2#))
endfor

appendInfoLine: "Trained in ", stopwatch, " seconds"

numberOfTestPatterns = 15
Erase all
Font size: 10
for itest to numberOfTestPatterns
	appendInfoLine: "Test pattern #", itest, ":"
	vowel = randomInteger (1, numberOfVowels)
	formant = randomGauss (mean# [vowel], sigma)
	input1# ~ 5 * exp (-0.5 * ((col - formant) / sigma) ^ 2) - 0.5
	#
	# Draw input.
	#
	Select outer viewport: 0, 3, (itest - 1) * 0.6, (itest - 1) * 0.6 + 1.0
	Create simple Matrix: "input", 1, numberOfInputNodes, ~ 5 * exp (-0.5 * ((col - formant) / sigma) ^ 2) - 0.5
	stdev = Get standard deviation: 0, 0, 0, 0
	appendInfoLine: "   Energy in input layer: ", stdev
	Draw rows: 0, 0, 0, 0, -5, 5
	Remove
	#
	# Spread up through first layer, without Bernoulli sampling.
	#
	output1# = sigmoid# (outbias1# + mul# (input1#, weight1##))
	appendInfoLine: "   Energy in middle layer: ", stdev (output1#)
	#
	# Copy output of first layer to input of second layer.
	#
	input2# = output1#
	#
	# Spread up through second layer, without Bernoulli sampling.
	#
	output2# = sigmoid# (outbias2# + mul# (input2#, weight2##))
	appendInfoLine: "   Energy in output layer: ", stdev (output2#)
	#
	# Spread down through second layer.
	#
	inrec2# = sigmoid# (inbias2# + mul# (weight2##, output2#))
	appendInfoLine: "   Energy in middle layer: ", stdev (inrec2#)
	#
	# Spread down through first layer.
	#
	inrec1# = inbias1# + mul# (weight1##, inrec2#)   ; Gaussian
	#
	# Draw reflection.
	#
	Select outer viewport: 3, 6, (itest - 1) * 0.6, (itest - 1) * 0.6 + 1.0
	Create simple Matrix: "reflection", 1, numberOfInputNodes, "inrec1# [col]"
	Draw rows: 0, 0, 0, 0, -5, 5
	Remove
endfor
