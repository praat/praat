# test_angle_between_planes
# djmw 20041021, 20070820 

appendInfoLine: "test_angle_between_planes.praat"
appendInfoLine: tab$, "200 times: angle=randomUniform(0.001,90), numberOfRows = randomInteger (10,1000)"
for i to 1;200
	 hoek = randomUniform (0.001, 90)
 	 nrow = randomInteger (10,1000)
 	 @test: hoek, nrow, 3
endfor

appendInfoLine: "test_angle_between_planes.praat OK"

procedure test: degrees, nrow, ncol,
	idt1 = Create TableOfReal: "t", nrow, ncol
	Formula: "if col=1 then randomGauss(0,10) else self endif"
	Formula: "if col=2 then randomGauss(0,5) else self endif"
	Formula: "if col=3 then randomGauss(0,2.5) else self endif"
 	# Centering is very important
	Centre columns
	c = To Configuration
	Rotate (pc)
	idt2 = To TableOfReal
	pc1 = To PCA
	selectObject: c
 	# The rotation is in the pc-coordinates frame
	Rotate: 1, 3, degrees
	idt3 =  To TableOfReal
	pc2 = To PCA
	selectObject: pc1, pc2
	degrees_found = Get angle between pc1-pc2 planes
	diff = abs (degrees_found-degrees)
	appendInfoLine: tab$, tab$, fixed$ (degrees, 4), " ", fixed$ (diff, 4), " (", nrow, ")"
 	;assert abs(degrees_found-degrees) < 2e-6; 'nrow' 'degrees' 'degrees_found'
	# printline 'nrow' 'degrees' 'degrees_found'

 	removeObject: idt1, idt2, idt3, pc1, pc2, c
endproc


