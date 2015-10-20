# test_Polygon_simplify.praat

printline test_Polygon_simplify

for j to 10
	m = Create simple Matrix... ps 2 100  randomUniform (0,1)
	p1x = Get value in cell... 1 1
	p1y = Get value in cell... 2 1
	p2x = Get value in cell... 1 10
	p2y = Get value in cell... 2 10
	for i from 2 to 99
		if ( i mod 10) =0
			p1x = Get value in cell... 1 i
			p1y = Get value in cell... 2 i
			p2x = Get value in cell... 1 i+10
			p2y = Get value in cell... 2 i+10
		elsif ( i mod 10) >0
			# 9 points on the edge from p(i) to point(i+10)
			af = randomUniform (0,1)
			xn = p1x +af*(p2x-p1x)
			yn = p1y + af*(p2y-p1y)
			Set value... 1 i xn
			Set value... 2 i yn
		endif
	endfor

	p = To Polygon
	for k to 10
		select p
		p3 = Circular permutation... randomUniform (1, 100)
		p4 = Simplify
		np = Get number of points
		assert np = 11
		plus p3
		Remove
	endfor
	select m
	plus p
	Remove
endfor

printline test_Polygon_simplify OK



