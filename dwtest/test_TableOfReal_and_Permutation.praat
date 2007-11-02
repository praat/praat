# test_TableOfReal_and_Permutation.praat
# djmw 20050713, 20070829

nrows = 30
Create TableOfReal... test nrows 2
testtab = selected ("TableOfReal")
Formula... row*col
labels$ = "aabbccddeeffgghhiijjk"


printline test: TableOfReal_and_Permutation.praat
for itry to 10
  select testtab
  for i to nrows
    randomIndex = randomInteger (1,20)
    label$ = mid$ (labels$, randomIndex, 1)
    Set row label (index)... i 'label$'
  endfor
  To Permutation (sort row labels)
  pid = selected ("Permutation")
  plus testtab
  Permute rows
  testtabp = selected ("TableOfReal")
  label$ = Get row label... 1
  for i from 2 to nrows  
    lab$ = Get row label... i
    assert lab$ >= label$ ; i='i' 'lab$' > 'label$'
    label$ = lab$
  endfor
  select pid
  Invert
  pidi = selected ("Permutation")
  plus testtabp
  Permute rows
  testtabpi = selected ("TableOfReal")
  Formula... self - TableOfReal_test[]
  stdev = Get column stdev (index)... 1
  assert stdev = 0
  select pid
  plus testtabp
  plus pidi
  plus testtabpi
  Remove
  print .
endfor
printline o.k.
select testtab
Remove



