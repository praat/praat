# test_biharmonicInterpolation.praat
# djmw 20170915

appendInfoLine: "test_biharmonicInterpolation.praat"
power = 2
noise_stdev = 0.01
xmin = -2
xmax = 2
ymin = -2
ymax = 2
nx = 200
ny = 200

numberOfPoints = 100
tor = Create TableOfReal: "xyz", numberOfPoints, 3

for irow to numberOfPoints
	x = randomUniform (xmin, xmax)
	y = randomUniform (ymin, ymax)
	z = x^power + y^power + randomGauss (0, noise_stdev)
	Set value: irow, 1, x
	Set value: irow, 2, y
	Set value: irow, 3, z
endfor

mat = Interpolate (rectangular grid): xmin, xmax, nx, ymin, ymax, ny, "Biharmonic"
dx = Get column distance
y1 = Get y of row: 1
dy = Get row distance
x1 = Get x of column: 1
sumSquared = 0.0
for irow to ny
	y = y1 + (irow -1) * dy
	for icol to nx
		x = x1 + (icol - 1) * dx
		z = 	x^power + y^power ; model
		zi = object [mat, irow, icol]; the interpolated data
		distSquared = (z - zi)^2	
		sumSquared += distSquared
	endfor
endfor

stdev = sumSquared / (nx * ny - 1)

appendInfoLine: tab$, "Number of points: ", numberOfPoints
appendInfoLine: tab$, "Model: z(x,y) = x^", power, "+y^", power, 
	... "+randomGauss(0,", noise_stdev, ")"
appendInfoLine: tab$, "Grid: [-2,2] x [-2,2]"
appendInfoLine: tab$, "Number of interpolated points:", nx * ny
appendInfoLine: tab$, "Stdev=", stdev
assert stdev < noise_stdev
removeObject: mat, tor

appendInfoLine: "test_biharmonicInterpolation.praat OK"

