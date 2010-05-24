# DTW_test.praat
# djmw 20100504

printline DTW_test start

stepSize = 0.005
s1 = Create Sound from formula... s1 Mono 0 1 44100 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
mfcc1 = To MFCC... 12 0.015 stepSize 100 100 0
select s1
s2 = Copy... s2
mfcc2 = To MFCC... 12 0.015 stepSize 100 100 0
plus mfcc1
dtw = To DTW... 1 0 0 0 0.056 yes yes 1/2 < slope < 2
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
plus mfcc1
plus mfcc2
plus s1
plus s2
#Remove
printline DTW_test end O.K.
