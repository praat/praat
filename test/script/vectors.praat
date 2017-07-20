a# = { 1, 4, 9, 16, 25 }
assert sum (a#) = 55
assert mean (a#) = 11
assert abs (mean (a#) - sum (a#) / 5) < 1e-14
assert abs (stdev (a#) - 9.669539802906858) < 1e-14
assert abs (stdev (a#) - sqrt (sumOver (i to 5, (a# [i] - mean (a#)) ^ 2) / 4)) < 1e-14
assert abs (center (a#) - 4.090909090909091) < 1e-14
assert abs (center (a#) - sumOver (i to 5, i * a# [i]) / sum (a#)) < 1e-14
