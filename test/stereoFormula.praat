echo Stereo formulas
# Paul Boersma, January 30, 2007
Create Sound from formula... test Stereo 0 1.5 44100 randomUniform (-1, 1)
a = Get value at sample number... Average 100
printline 'a'
b = Get value at sample number... Left 100
printline 'b'
c = Get value at sample number... Right 100
printline 'c'
d = Sound_test [100]
printline 'd'
dd = Sound_test [0, 100]
printline 'dd'
ee = Sound_test [1, 100]
printline 'ee'
f = Sound_test [2, 100]
printline 'f'
assert a = d
assert b = ee
assert c = f
assert d = dd
a = Get value at time... Average 0.5 Linear
printline 'a'
b = Get value at time... Left 0.5 Linear
printline 'b'
c = Get value at time... Right 0.5 Linear
printline 'c'
d = Sound_test (0.5)
printline 'd'
dd = Sound_test (0.5, 0)
printline 'dd'
ee = Sound_test (0.5, 1)
printline 'ee'
f = Sound_test (0.5, 2)
printline 'f'
assert abs (a - d) < 1e-7
assert abs (b - ee) < 1e-7
assert abs (c - f) < 1e-7
assert d = dd
Remove
printline OK
