echo Stereo formulas
# Paul Boersma, February 1, 2008
Create Sound from formula... test Stereo 0 1.5 44100 randomUniform (-1, 1)
a = Get value at sample number... Average 100
printline 'a'
b = Get value at sample number... Left 100
printline 'b'
c = Get value at sample number... Right 100
printline 'c'
d = Sound_test [100]
d2 = object ["Sound test", 100]
printline 'd'   'd2'
dd = Sound_test [0, 100]
dd2 = object ["Sound test", 0, 100]
printline 'dd'   'dd2'
ee = Sound_test [1, 100]
ee2 = object ["Sound test", 1, 100]
printline 'ee'   'ee2'
f = Sound_test [2, 100]
f2 = object ["Sound test", 2, 100]
printline 'f'   'f2'
assert a = d
assert b = ee
assert c = f
assert d = dd
assert a = d2
assert b = ee2
assert c = f2
assert d = dd2
a = Get value at time... Average 0.5 Linear
printline 'a'
b = Get value at time... Left 0.5 Linear
printline 'b'
leftAmplitude = Get value at sample number: 1, 22050
rightAmplitude = Get value at sample number: 1, 22051
average = 0.5 * (leftAmplitude + rightAmplitude)
appendInfoLine: "by hand: ", average
c = Get value at time... Right 0.5 Linear
printline 'c'
d = Sound_test (0.5)
d2 = object ("Sound test", 0.5)
printline 'd'   'd2'
dd = Sound_test (0.5, 0)
dd2 = object ("Sound test", 0.5, 0)
printline 'dd'   'dd2'
ee = Sound_test (0.5, 1)
ee2 = object ("Sound test", 0.5, 1)
printline 'ee'   'ee2'
f = Sound_test (0.5, 2)
f2 = object ("Sound test", 0.5, 2)
printline 'f'   'f2'
assert abs (a - d) < 1e-12
assert abs (b - ee) < 1e-12
assert abs (c - f) < 1e-12
assert abs (d - dd) < 1e-12
assert abs (a - d2) < 1e-12
assert abs (b - ee2) < 1e-12
assert abs (c - f2) < 1e-12
assert abs (d - dd2) < 1e-12
Remove
printline OK
