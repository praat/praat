# fisherQ
# Paul Boersma, August 27, 2003
# Computes a significance from zero, given a measured F value.
df1 = 2
df2 = 70
f = 33.59
fisherQ = fisherQ (f, df1, df2)
fisherQ$ = fixed$ (fisherQ, 20)
echo fisherQ test: 'fisherQ' 'fisherQ$'
assert fisherQ$ = "0.00000000005932714540"
for i to 10000
   assert fisherQ (randomUniform (3, 4), 1, 100000) <> undefined
endfor

echo invFisherQ
call invFisherQ 2 70 1e-14
call invFisherQ 70 2 1e-14
call invFisherQ 1 100000 1e-8
call invFisherQ 100000 1 1e-11
call invFisherQ 1 1 1e-13
call invFisherQ 100 100 1e-9
procedure invFisherQ df1 df2 precision
   # Known values.
   assert invFisherQ (0, 'df1', 'df2') = undefined
   assert invFisherQ (1, 'df1', 'df2') = 0
   # We should be able to draw a curve of invFisherQ.
   for i from 1 to 1000
      assert abs (fisherQ (invFisherQ (i/1000, df1, df2), 'df1', 'df2') - 'i'/1000) < 'precision'
   endfor
   # Q near 0, i.e. F large: relative precision.
   for power from 4 to 150
      q = 10 ^ -power
      f = invFisherQ (q, df1, df2)
      assert f <> undefined ; 'q' 'df1' 'df2'
      assert abs (fisherQ (f, 'df1', 'df2') - 'q') < 'q'*'precision'*10 ; 'f'
   endfor
   for power from 151 to 309
      q = 10 ^ -power
      f = invFisherQ (q, df1, df2)
      assert f = undefined or abs (fisherQ (f, 'df1', 'df2') - 'q') < 'q'*'precision'*30 ; 'f'
   endfor
   # Accuracy is lost for Q near 1, i.e. F near zero.
   assert abs (fisherQ (invFisherQ (0.9999, df1, df2), 'df1', 'df2') - 0.9999) < 'precision' * 4
   assert abs (fisherQ (invFisherQ (0.99999, df1, df2), 'df1', 'df2') - 0.99999) < 'precision' * 40
   assert abs (fisherQ (invFisherQ (0.999999, df1, df2), 'df1', 'df2') - 0.999999) < 'precision' * 300
   assert abs (fisherQ (invFisherQ (0.9999999, df1, df2), 'df1', 'df2') - 0.9999999) < 'precision' * 600
   assert abs (fisherQ (invFisherQ (0.99999999, df1, df2), 'df1', 'df2') - 0.99999999) < 'precision' * 6000
   assert abs (fisherQ (invFisherQ (0.999999999, df1, df2), 'df1', 'df2') - 0.999999999) < 'precision' * 10000
   #
   # The inverse relationship: q values from measured f values should map back to those f values.
   #
   for power to 100
      f = 10 ^ -power
      q = fisherQ (f, df1, df2)
      assert q=1 or abs (invFisherQ ('q', 'df1', 'df2') - 'f') < precision
   endfor
   for f from 10 to 99
      q = fisherQ (f/100, df1, df2)
      assert q>1-precision or abs (invFisherQ (q, 'df1', 'df2') - 'f'/100) < precision
      assert abs (invFisherQ (fisherQ (f/10, df1, df2), 'df1', 'df2') - 'f'/10) < f/10*precision
      assert abs (invFisherQ (fisherQ (f, df1, df2), 'df1', 'df2') - 'f') < f*precision
      assert abs (invFisherQ (fisherQ (10*f, df1, df2), 'df1', 'df2') - 10*'f') < 10*f*precision
      q = fisherQ (100*f, df1, df2)
      assert q = 0 or abs (invFisherQ ('q', 'df1', 'df2') - 100*'f') < 100*f*precision
      q = fisherQ (1000*f, df1, df2)
      assert q = 0 or abs (invFisherQ ('q', 'df1', 'df2') - 1000*'f') < 1000*f*precision
   endfor
endproc
#
# Things that used to go wrong.
#
assert invFisherQ (0.13, 1, 1e9) <> undefined ; used to exceed 60 iterations
assert invFisherQ (0.159, 2, 70) <> undefined ; used to exceed 60 iterations
#
# Things that still go wrong.
#
; fisherQ (1, 1e19, 1e19) --> negative
#
# Things that newly go wrong.
#
; fisherQ (invFisherQ (0.999999, 1, 100000), 1, 100000) = 1 ; used to be 0.9999973414545418
#
# Things that again go wrong.
#
