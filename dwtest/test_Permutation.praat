# test_Permutation.praat
# djmw 20050710, 20070820, 20100525

call test_600_12
call rotate
call invert
call sequence
procedure rotate
  printline Rotate
  for i to 20
    Create Permutation... test i y
    p = selected ("Permutation")
  
    print 'i'
    for k to i
      select p
      Rotate... 0 0 k
      pk = selected ("Permutation")
      pkk = Get value... k+1
      if pkk > 0
        assert pkk = 1 ; i='i', k='k'
      endif
      Rotate... 0 0 -k
      pki = selected ("Permutation")
      for l to k
        pkil = Get value... l
        assert pkil = l
      endfor
      select pk
      plus pki
      Remove
      print .
    endfor
    printline o.k.
    select p
    Remove
  endfor
endproc

procedure invert
  printline Invert
  for i to 20
    Create Permutation... test i n
    p = selected ("Permutation")
    Invert
    pin = selected ("Permutation")
    plus p
    Multiply
    pm = selected ("Permutation")
    for k to i
      pmk = Get value... k
      assert pmk = k ; i='i', k='k'
    endfor
    print .
    select pm
    plus pin
    plus p
    Remove
  endfor
  printline o.k.
endproc

procedure test_600_12
  printline test_600_12
  .p = Create Permutation... 600 600 y
  .pin = Interleave... 0 0 12 1
  .pininv = Invert
  plus .pin
  .pm = Multiply
  for l to 600
      pkil = Get value... l
      assert pkil = l; l='l'
  endfor
  plus .pin
  plus .pininv
  plus .p
  Remove
  printline o.k.
endproc  

procedure sequence
  print Sequences 
  .n = 4
  .np = 4*3*2
  Create Permutation... ps .n y
  for .j to .np 
    for .i to .j
      Next
    endfor
    for .i to .j
      Previous
    endfor
    for .i to .n
      .val = Get value... .i
      assert .val = .i
    endfor
    print .
  endfor
  Remove
  print  o.k. 
endproc