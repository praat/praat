echo sqrt
# Paul Boersma, July 6, 2003
assert sqrt (-100) = undefined
assert sqrt (0) = 0
assert sqrt (1) = 1
assert sqrt (4) = 2
assert sqrt (9) = 3
assert sqrt (123456789^2) = 123456789
assert abs (sqrt (2) ^ 2 - 2) < 1e-15

printline exp
# Paul Boersma, September 13, 2015
assert exp (-10000) = 0
assert exp (0) = 1
assert abs (exp (1) - e) < 1e-15
assert abs (1 - exp (100) / e ^ 100) < 1e-13
assert abs (1 - exp (100) * exp (-100)) < 1e-15
assert abs (1 - e ^ 100 * e ^ -100) < 1e-14

printline pow
# Paul Boersma, August 27, 2003
assert 0^7 = 0
assert 7^0 = 1
;assert 0^0 = 1 ; special choice

printline ln
# Paul Boersma, July 6, 2003
assert ln (-100) = undefined
assert ln (0) = undefined
assert ln (1) = 0
assert abs (ln (exp (100)) - 100) < 1e-13
assert abs (ln (exp (10)) - 10) < 1e-17
assert abs (ln (exp (1)) - 1) < 1e-17
assert abs (ln (exp (0.1)) - 0.1) < 1e-16
assert abs (ln (exp (0.01)) - 0.01) < 1e-15

printline lnGamma
# Paul Boersma, July 13, 2003
assert lnGamma (-100) = undefined
assert lnGamma (0) = undefined
assert lnGamma (1) = 0
assert abs (lnGamma (e^-100) - 100) < 1e-13
assert abs (e^lnGamma(4) - 6) < 1e-13
assert abs (exp(lnGamma(4)) - 6) < 1e-13
factorial = 1
for i to 170
   factorial *= i
   assert abs (1 - e^lnGamma('i' + 1) / factorial) < 1e-12
endfor
for i from -100 to 2
   x = 1.23456789 * 10^i
   gamma = exp (lnGamma (x))
   gamma2 = exp (lnGamma (x + 1))
   gamma3 = x * gamma
   ;printline 'gamma2' 'gamma3'
   assert abs (1 - gamma2 / gamma3) < 1e-13
endfor

printline OK