echo arithmetic
# Paul Boersma, January 27, 2006

assert 1 + 1 = 2
assert 1 + undefined = undefined
assert undefined + 4 = undefined
assert undefined + undefined = undefined

assert 5 * 6 = 30
assert 5 * undefined = undefined
assert undefined * 6 = undefined
assert undefined * undefined = undefined

assert 30 / 5 = 6
assert 5 / undefined = undefined
assert undefined / 6 = undefined
assert undefined / undefined = undefined
assert 30 / 0 = undefined
assert 30 div 0 = undefined
assert 30 mod 0 = undefined

assert 19 - 7 = 12
assert 19 - undefined = undefined
assert undefined - 7 = undefined
assert undefined - undefined = undefined

# From the manual (Formulas 2. Operators):

assert --6 = 6
assert 2^6 = 64
assert 2^-6 = 0.015625
assert -(1+1) ^6 = -64
assert 4^3^2 = 262144
assert 4^3 ^ 2 = 262144
assert (4 ^ 3) ^ 2 = 4096
assert (-2)^6 = 64
assert -(2^6) = -64
assert -(2)^6 = -64
assert 1/4*5 = 1.25
assert 1 / 4*5 = 1.25
assert 1 / (4*5) = 0.05
assert 3 * 2 ^ 4 = 48
assert 3*2 ^ 4 = 48
assert (3 * 2) ^ 4 = 1296
assert 54 div 5 = 10
assert 54 mod 5 = 4
assert 54.3 div 5.1 = 10
assert abs (54.3 mod 5.1 - 3.3) < 1e-12
assert -54 div 5 = -11
assert -(54 div 5) = -10
assert -(54 mod 5) = -4
assert 3 * 18 div 5 = 10
assert 3 * (18 div 5) = 9
assert 3 * 18 mod 5 = 4
assert 3 * (18 mod 5) = 9
assert 54 div 5 * 3 = 30
assert 54 div (5 * 3) = 3
assert 54 mod 5 * 3 = 12
assert 54 mod (5 * 3) = 9
assert 3 - 8 + 7 = 2
assert 3 - (8 + 7) = -12
assert 3 + 8 * 7 = 59
assert (3 + 8) * 7 = 77
assert 3 + - (2 ^ 4) = -13
assert 3 + 5 / 2 + 3 = 8.5
assert (3 + 5) / (2 + 3) = 1.6

assert 1 + 1 = 2
assert 1 + 1 < 3
assert 1 + 1 <= 2
assert 1 + 1 <= 3
assert 1 + 1 > 1
assert 1 + 1 >= 2
assert 1 + 1 >= 1

assert (5 + 6 = 10) = 0
assert (5 + 6 = 11) = 1
assert (5 + 6 <> 10) = 1
assert (5 + 6 <> 11) = 0
assert (5 + 6 < 10) = 0
assert (5 + 6 < 11) = 0
assert (5 + 6 > 10) = 1
assert (5 + 6 > 11) = 0
assert (5 + 6 <= 10) = 0
assert (5 + 6 <= 11) = 1
assert (5 + 6 >= 10) = 1
assert (5 + 6 >= 11) = 1
assert (not 5 + 6 = 10) = 1
assert (not 5 + 6 = 11) = 0
x = 7
assert (x > 5 and x < 10) = 1
assert (not x <= 5 and not x >= 10) = 1
assert (not (x <= 5 or x >= 10)) = 1
assert ("hallo" = "hallo") = 1
assert ("hallo" = "hello") = 0
assert ("hallo" <> "hallo") = 0
assert ("hallo" <> "hello") = 1
assert ("hallo" < "hallo") = 0
assert ("hallo" < "hello") = 1
assert ("hello" < "hallo") = 0
assert ("hallo" > "hallo") = 0
assert ("hallo" > "hello") = 0
assert ("hello" > "hallo") = 1
assert ("hallo" <= "hallo") = 1
assert ("hallo" <= "hello") = 1
assert ("hello" <= "hallo") = 0
assert ("hallo" >= "hallo") = 1
assert ("hallo" >= "hello") = 0
assert ("hello" >= "hallo") = 1
assert "hallo" + "dag" = "hallodag"
assert "hallo" + "dag" <> "hellodag"
assert "hallo" - "dag" = "hallo"
assert "hallodag" - "dag" = "hallo"

assert left$ ("hallo", 3) = "hal"

printline OK