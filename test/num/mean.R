n = 1e6
big = 1
sequence = seq (1, 7)
accurateMean = mean (sequence)
accurateStdev = sd (sequence) * sqrt (6 / 7) / sqrt (1 - 1 / 7 / n)
for (power in seq (1, 20)) {
	big = big * 10
	a = rep (big + sequence, n)
	cat (power, mean (a) - big - accurateMean, sd (a) - accurateStdev, '\n')
}
