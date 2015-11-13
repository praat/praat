# test_MDS.praat

printline test MDS

dr = Create letter R example... 0

c[1] = To Configuration (monotone mds)... 2 "Primary approach" 1e-05 50 1
select dr
c[2] = To Configuration (i-spline mds)... 2 1 1 1e-05 50 1
select dr
c[3] = To Configuration (interval mds)... 2 1e-05 50 1
select dr
c[4] = To Configuration (ratio mds)... 2 1e-05 50 1
select dr
c[5] = To Configuration (absolute mds)... 2 1e-05 50 1
select dr
c[6] = To Configuration (kruskal)... 2 2 "Primary approach" Formula1 1e-05 50 1

selectObject: dr, c[1]
c[6+1]= To Configuration (monotone mds): "Primary approach", 1e-7, 50, 1

selectObject: dr, c[2]
c[6+2] = To Configuration (i-spline mds): 1, 1, 1e-07, 50, 1
selectObject: dr, c[3]
c[6+3] = To Configuration (interval mds): 1e-05, 50, 1
selectObject: dr, c[4]
c[6+4] = To Configuration (ratio mds): 1e-05, 50, 1
selectObject: dr, c[5]
c[6+5] = To Configuration (absolute mds): 1e-05, 50, 1
selectObject: dr, c[6]
c[6+6] = To Configuration (kruskal): "Primary approach", "Formula1", 1e-05, 50, 1


printline ... comparing configurations
for i from 2 to  6
	select c[1]
	plus c[i]
	p1 = To Procrustes... no
	plus c[i]
	ct[i] = To Configuration
	plus c[1]
	p2 = To Procrustes... no
	call check_if_identity_transform  p2
	plus p1
	Remove
endfor
ct[1] = c[1]
printline .... INDSCAL
select c[1]
for i from 2 to 6
	plus ct[i]
endfor
To Distance
for i to 6
	dist[i] = selected ("Distance", i)
endfor

To Configuration (indscal)... 2 yes 1e-5 100 1
ic[1] = selected ("Configuration")
is[1] = selected ("Salience")

selectObject: ic[1]
for i to 6
	plusObject: dist[i]
endfor

To Configuration (indscal): "yes", 1e-5, 100
ic[2] = selected ("Configuration")
is[2] = selected ("Salience")

select c[1]
plus ic[1]
p3 = To Procrustes... no
plus ic[1]
cp3 = To Configuration
plus c[1]
p4 = To Procrustes... no
call check_if_identity_transform  p4

select dr
plusObject: ic[1], ic[2]
plusObject: is[1], is[2]
plus p3
plus p4
plus cp3
for i to 6
	plus c[i]
	plus c[i+6]
	plus ct[i]
	plus dist[i]
endfor
Remove
printline test MDS ok

procedure check_if_identity_transform  .p
	select .p
	.scale = Get scale
	assert .scale > 1 - 1e-4
	for .j to 2
		tj = Get translation element... .j
		assert abs(tj) < 1e-6
		for .k to 2
			tjk = Get transformation element... .j .k
			if .j = .k
				assert tjk > 1 - 1e-4
			else
				assert abs(tjk) < 1e-6
			endif
		endfor
	endfor
endproc

