# test_Permutation.praat
# djmw 20050710, 20070820

call test_600_12
call rotate
call invert
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
  Create Permutation... 600 600 y
  p = selected ("Permutation")
  Interleave... 0 0 12 1
  pin = selected ("Permutation")
  Invert
  pininv = selected ("Permutation")
  plus pin
  Multiply
  pm = selected ("Permutation")
  for l to 600
      pkil = Get value... l
      assert pkil = l; l='l'
  endfor
  printline o.k.
endproc  
