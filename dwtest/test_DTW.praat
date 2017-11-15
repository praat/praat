# test_DTW.praat
# djmw 20100504, 20120223

printline DTW_test start

stepSize = 0.005
s1 = Create Sound from formula... s1 Mono 0 1 44100 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
s2 = Copy... s2
plus s1
dtw = To DTW... 0.015 stepSize 0.1 1/2 < slope < 2
printline 'tab$' Slope constraint
nx = Get maximum consecutive steps... X
assert nx <= 2
ny =  Get maximum consecutive steps... Y
assert ny <= 2
for i to 20
	t = i * 0.04
	ty = Get y time... t
	assert (ty - stepSize) < t and t < (ty + stepSize)
	tx = Get x time... t
	assert (tx - stepSize) < t and t < (tx + stepSize)
endfor

select dtw
printline 'tab$' To Polygon...
pol = To Polygon... 0.1 1/2 < slope < 2
Remove
select dtw
pol = To Polygon... 0.1 1/3 < slope < 3
Remove
select dtw
pol = To Polygon... 0.1 2/3 < slope < 3/2
Remove
printline 'tab$' To Matrix
select dtw
mat = To Matrix (distances)
Remove
select dtw
To Matrix (cum. distances)... 0.05 2/3 < slope < 3/2
Remove
select dtw
plus s1
plus s2
Remove
printline test_DTW end O.K.
