#! Praat test script min.praat
# Paul Boersma, July 26, 2003

echo min

assert min (5, 6) = 5
assert min (-7.8) = -7.8
assert min (3, 4, 5, 7, 2, 10) = 2
assert min (3, 4, 5, 7, 12, 10) = 3
assert min (3, 4, 5, 7, 12, 1) = 1

assert imin (5, 6) = 1
assert imin (-7.8) = 1
assert imin (3, 4, 5, 7, 2, 10) = 5
assert imin (3, 4, 5, 7, 12, 10) = 1
assert imin (3, 4, 5, 7, 12, 1) = 6

assert max (5, 6) = 6
assert max (-7.8) = -7.8
assert max (3, 4, 5, 7, 2, 10) = 10
assert max (3, 4, 5, 7, 12, 10) = 12
assert max (13, 4, 5, 7, 12, 10) = 13

assert imax (5, 6) = 2
assert imax (-7.8) = 1
assert imax (3, 4, 5, 7, 2, 10) = 6
assert imax (3, 4, 5, 7, 12, 10) = 5
assert imax (13, 4, 5, 7, 12, 10) = 1

assert min (undefined, 3) = undefined
assert imin (undefined, 3) = undefined
assert max (undefined, 3) = undefined
assert imax (undefined, 3) = undefined

assert min (3, undefined) = undefined
assert imin (3, undefined) = undefined
assert max (3, undefined) = undefined
assert imax (3, undefined) = undefined

printline OK