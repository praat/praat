#! Praat test script norm.praat
# Paul Boersma 20200419

writeInfoLine: "norm"

assert norm ({ 3, 4 }, 1) = 7
assert norm ({ 3, -4 }, 1) = 7

assert norm ({ 3, 4 }) = 5
assert norm ({ 3, 4 }, 2) = 5
assert norm ({ -3, 4 }, 2) = 5

assert abs (norm ({ 3, 3, 2, 1, 1 }, 3) - 4) < 1e-15
assert abs (norm ({ -3, 3, 2, -1, 1 }, 3) - 4) < 1e-15

assert norm ({ { 3 , -4 } }, 1) = 7
assert norm ({ { 3 }, { -4 } }, 1) = 7
assert norm ({ { -3, 4 } }, 2) = 5
assert norm ({ { -3 }, { 4 } }, 2) = 5
assert abs (norm ({ { -3, 3, 2 }, { -1, 0, 1 } }, 3) - 4) < 1e-15
assert abs (norm ({ { -3, 3 }, { 2, -1 }, { 0, 1 } }, 3) - 4) < 1e-15

appendInfoLine: "OK"