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
ic = selected ("Configuration")
is = selected ("Salience")

select c[1]
plus ic
p3 = To Procrustes... no
plus ic
cp3 = To Configuration
plus c[1]
p4 = To Procrustes... no
call check_if_identity_transform  p4

select dr
plus ic
plus is
plus p3
plus p4
plus cp3
for i to 6
	plus c[i]
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

