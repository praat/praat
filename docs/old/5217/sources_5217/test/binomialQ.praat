echo binomialQ
assert fixed$ (invBinomialP (0.025, 211, 1000), 6) = "0.237622"
assert fixed$ (invBinomialQ (0.025, 211, 1000), 6) = "0.186092"
assert abs (binomialP (invBinomialP (0.025, 211, 1000), 211, 1000) - 0.025) < 1e-14
assert abs (binomialQ (invBinomialQ (0.025, 211, 1000), 211, 1000) - 0.025) < 1e-15
printline OK