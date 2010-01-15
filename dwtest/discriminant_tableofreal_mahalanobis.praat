# discriminant_tableofreal_mahalanobis.praat
# djmw 20100106

# This scripts calculates the amount of overlap between two normal distributions
# both distributions have sigma = 1
# mu1 = 0 and mu2 varies from 1 to 5

n = 20000

t0 = Create TableOfReal... t0 n 1
for i to n/2
  Set row label (index)... i 1
endfor
for i from n/2+1 to n
  Set row label (index)... i 2
endfor

for mu to 5
  select t0
  Formula... randomGauss ((if row < n/2 then 0 else mu fi), 1)
  d = To Discriminant
  for j to 2
    select d
    plus t0
    t'j' = To TableOfReal (mahalanobis)... 'j' n
  endfor
  tt = Copy... tt
  for nsigma to 5
    Formula... Object_'t1'[row,col] < nsigma and Object_'t2'[row,col] < nsigma
    nr = 0
    for kr to n
      if Object_'tt'[kr,1] = 1
        nr += 1
      endif
    endfor
    z12 = 0 + nsigma
    z21 = mu - nsigma
    expect = 0
    if z21 < z12
       # we have overlap
       expect = (gaussQ (z21) - gaussQ (z12)) * n
    endif
    diff = nr - expect
    printline For d='mu' and 'nsigma'-sigma: 'nr' from 'n' overlap; diff = 'diff:1' (expect = 'expect:1') 
  endfor
  select d
  plus t1
  plus t2
  plus tt
  Remove
endfor