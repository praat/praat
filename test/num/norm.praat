#! Praat test script norm.praat
# Paul Boersma, 10 January 2024

writeInfoLine: "norm"

assert norm ({ 3, 4 }, 1) = 7
assert norm ({ 3, -4 }, 1) = 7

assert norm ({ 3, 4 }) = 5
assert norm ({ 3, 4 }, 2) = 5
assert norm ({ -3, 4 }, 2) = 5

allowedImprecision = if windows then 1e-14 else 1e-15 fi

assert abs (norm ({ 3, 3, 2, 1, 1 }, 3) - 4) < allowedImprecision
assert abs (norm ({ -3, 3, 2, -1, 1 }, 3) - 4) < allowedImprecision

assert norm ({ { 3 , -4 } }, 1) = 7
assert norm ({ { 3 }, { -4 } }, 1) = 7
assert norm ({ { -3, 4 } }, 2) = 5
assert norm ({ { -3 }, { 4 } }, 2) = 5
assert abs (norm ({ { -3, 3, 2 }, { -1, 0, 1 } }, 3) - 4) < allowedImprecision
assert abs (norm ({ { -3, 3 }, { 2, -1 }, { 0, 1 } }, 3) - 4) < allowedImprecision

appendInfoLine: "OK"