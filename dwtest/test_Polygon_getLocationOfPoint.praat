# test_Polygon_getLocationOfPoint.praat

eps = 1e-15

# minimal crossing Polygon

px[1] = 0.123
py[1] = 0
px[2] = 0.123
py[2] = 1
px[3] = 1.123
py[3] = 0
px[4] = 1.123
py[4] = 1
np = 4

p = Create simple Polygon... p 'px[1]' 'py[1]' 'px[2]' 'py[2]' 'px[3]' 'py[3]' 'px[4]' 'py[4]'

px[5] = px[1]
py[5] = py[1]

printline ... x left
xm = px[1] - 0.1
for i to 100
	ym = randomUniform(py[1]-1, py[2]+1)
	loc$ = Get location of point... xm ym eps
	assert loc$="O"; 'xm' 'ym'	
endfor

printline ... x right
xm = px[3] + 0.1
for i to 100
	ym = randomUniform(py[1]-1, py[2]+1)
	loc$ = Get location of point... xm ym eps
	assert loc$="O"; 'xm' 'ym'	
endfor

printline ... y top
ym = py[2] + 0.1
for i to 100
	xm = randomUniform(px[1]-1, px[3]+1)
	loc$ = Get location of point... xm ym eps
	assert loc$="O"; 'xm' 'ym'	
endfor

printline ... y bottom
ym = py[1] - 0.1
for i to 100
	xm = randomUniform(px[1]-1, px[3]+1)
	loc$ = Get location of point... xm ym eps
	assert loc$="O"; 'xm' 'ym'	
endfor

printline ... vertices
for i to np
	loc$ = Get location of point... px[i] py[i] eps
	assert loc$ = "V"
endfor

printline ... edges
for i to np
	for j to 100
		f = randomUniform(0,1)
		xm = px[i]+f*(px[i+1]-px[i])
		ym = py[i] +f*(py[i+1]-py[i])
		loc$ = Get location of point... xm ym eps
		assert loc$ = "E";  'xm' 'ym'
	endfor
endfor

printline ... inside
for i to 100
	fx = randomUniform (0,0.5)
	fy = randomUniform (fx, 1-fx)
	xm = px[1]+fx
	ym = py[1]+fy
	loc$ = Get location of point... xm ym eps
	assert loc$ = "I";  'xm' 'ym'
	fx = 1 - fx
	xm = px[1]+fx
	loc$ = Get location of point... xm ym eps
	assert loc$ = "I";  'xm' 'ym'
endfor

printline ... outside
#could be done better
for i to 100
	fx = randomUniform (0,0.5)
	fy = randomUniform (0, fx)
	xm = px[1] + fx
	ym = py[1] + fy
	loc$ = Get location of point... xm ym eps
	assert loc$ = "O";  'xm' 'ym'
endfor

select p
Remove
# now use epsilon ...


