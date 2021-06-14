#
# Some normal numbers.
#
assert 1 / 2 <> undefined
assert 0 / 2 <> undefined
assert sqrt (10) <> undefined

#
# Some infinities.
#
assert 1 / 0 = undefined
assert 1 div 0 = undefined
assert 1 mod 0 = undefined
assert 10 ^ 500 = undefined
assert exp (1000) = undefined

#
# Some infinitesimals.
#
assert 10 ^ -500 = 0
assert exp (-1000) = 0

#
# Some not-a-numbers.
#
assert sqrt (-10) = undefined
assert 0 / 0 = undefined
assert 0 div 0 = undefined
assert 0 mod 0 = undefined

#
# Propagation.
#
assert - undefined = undefined
assert undefined - undefined = undefined
assert sqrt (undefined) = undefined
assert sqrt (- undefined) = undefined
assert undefined * 0 = undefined
assert 0 * undefined = undefined
assert 1 / undefined = undefined
assert 0 ^ undefined = undefined
assert undefined ^ undefined = undefined
assert abs (undefined) = undefined
assert round (undefined) = undefined
assert floor (undefined) = undefined
assert ceiling (undefined) = undefined
assert rectify (undefined) = undefined
assert sin (undefined) = undefined
assert cos (undefined) = undefined
assert tan (undefined) = undefined
assert arcsin (undefined) = undefined
assert arccos (undefined) = undefined
assert arctan (undefined) = undefined
assert sinh (undefined) = undefined
assert cosh (undefined) = undefined
assert tanh (undefined) = undefined
assert arcsinh (undefined) = undefined
assert arccosh (undefined) = undefined
assert arctanh (undefined) = undefined
assert exp (undefined) = undefined
assert log2 (undefined) = undefined
assert ln (undefined) = undefined
assert log10 (undefined) = undefined
assert exp (1e6) = undefined
assert 0 * exp (1e6) = undefined
assert sqrt (-1) = undefined
assert 0 * sqrt (-1) = undefined

#
# Propagation within larger expressions.
#
assert 1 / (1 / 0) = undefined
assert 1 / (0 / 0) = undefined
assert 1 / exp (1000) = undefined   ; not the same as exp (-1000)!
assert 0 ^ (1 / 0) = undefined
assert 0 * (0 / 0) = undefined
assert 0 * (1 / 0) = undefined

#
# Exception to propagation: anything to the power 0 always gives 1 (as in R).
#
assert 0 ^ 0 = 1
;assert undefined ^ 0 = 1
assert undefined ^ 0 = undefined
;assert sqrt (-1) ^ 0 = 1
assert sqrt (-1) ^ 0 = undefined

