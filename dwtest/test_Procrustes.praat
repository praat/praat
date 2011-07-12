# test_Procrustes.praat
# djmw 20040117, 20070820

printline ----- test_Procrustes.praat -----
 
call test_orthogional_procrustes_gvl_12_4_3
call test_procrustes_bg_19_4
call test_procrustes_random_configurations 12
printline
printline ----- succeeded: test_Procrustes.praat (end) -----

procedure test_procrustes_bg_19_4
  # example Borg&Groenen section 19.4
  printline   test example in Borg & Groenen section 19.4
  nr = 4 
  nc = 2
  s = 0.5
  t1 = 1
  t2 = 2
  alpha = 30
  eps = 2.3e-16
  
  Create Configuration... X nr nc 0
  Set value... 1 1 1
  Set value... 1 2 2
  Set value... 2 1 -1
  Set value... 2 2 2
  Set value... 3 1 -1
  Set value... 3 2 -2
  Set value... 4 1 1
  Set value... 4 2 -2
  
  for i to 4
    Set row label (index)... i x_'i'
  endfor
  
  Copy... Y
  ! Horizontal reflection
  Invert dimension... 1
  Rotate... 1 2 alpha
  
  Formula...  0.5*self + (if col=1 then t1 else t2 fi)
  
  plus Configuration X
  To Procrustes... 0
  Rename... X_Y
  plus Configuration Y
  To Configuration
  Rename... Z
  Formula... self-Configuration_X[]
  for i to 4
    for j to 2
      assert Configuration_Z[i,j] < nr*nc*eps; Configuration_Z['i','j'] < 'nr'*'nc'*'eps'
    endfor
  endfor
  
  select Procrustes X_Y
  Invert
  Rename... X_Yi
  for i to 2
    tp = Get translation element... i
    assert t'i' - tp < nr*nc*eps; translation['i']
  endfor
  sp = Get scale
  assert s - tp < nr*nc*eps; scale
  
  select Configuration X
  plus Configuration Y
  plus Configuration Z
  plus Procrustes X_Y
  plus Procrustes X_Yi
  Remove
endproc

procedure test_procrustes_random_configurations numconf
  printline   'numconf' randomly generated configurations of dimension 2^1 x 2 .. 2^12 x 2
  nr = 2
  print     Configuration:
  for k to numconf
    print  'k'
    Create Configuration... X nr nc randomUniform(-1,1)
    Copy... Y
    Invert dimension... 1
    alpha = randomUniform (0,90)
    Rotate... 1 2 alpha
    t1 = randomUniform (0,2)
    t2 = randomUniform (0,2)
    scale = randomUniform(0.5,2)
    Formula...  scale*self + (if col=1 then t1 else t2 fi)
  
    plus Configuration X
    To Procrustes... 0
    Rename... X_Y
    plus Configuration Y
    To Configuration
    Rename... Z
    Formula... self-Configuration_X[]
    for i to nr
      for j to 2
        assert Configuration_Z[i,j] < nr*nc*eps; Configuration_Z['i','j'] < 'nr'*'nc'*'eps'
      endfor
    endfor
  
    select Procrustes X_Y
    Invert
    Rename... X_Yi
    # no need to test the translations, they need not be equal (see BG page 347)
    sp = Get scale
    assert abs(scale - sp) < nr*nc*eps; scale
  
#   printline o.k.: nr='nr', nc='nc', alpha='alpha' degrees, scale='scale', t=('t1', 't2')
    nr *= 2
    select Configuration X
    plus Configuration Y
    plus Configuration Z
    plus Procrustes X_Y
    plus Procrustes X_Yi
    Remove
  endfor
endproc

procedure test_orthogional_procrustes_gvl_12_4_3
  printline   test orthognal Procrustes transform (example 12.4.1 Golub & van Loan)
  Create Configuration... a 4 2 0
  Set value... 1 1 1
  Set value... 2 1 3
  Set value... 3 1 5
  Set value... 4 1 7
  Set value... 1 2 2
  Set value... 2 2 4
  Set value... 3 2 6
  Set value... 4 2 8

  Create Configuration... b 4 2 0
  Set value... 1 1 1.2
  Set value... 2 1 2.9
  Set value... 3 1 5.2
  Set value... 4 1 6.8
  Set value... 1 2 2.1
  Set value... 2 2 4.3
  Set value... 3 2 6.1
  Set value... 4 2 8.1

  plus Configuration a
  To Procrustes... y

  t2 = Get translation element... 2
  assert t2 = 0
  s = Get scale
  assert s = 1; 

  eps = 5e-5
  r11 = Get transformation element... 1 1
  r22 = Get transformation element... 2 2
  assert r11-r22 < eps
  assert r11-0.9999 < eps
  r12 = Get transformation element... 1 2
  r21 = Get transformation element... 2 1
  assert r12+0.0126 < eps
  assert r21-0.0126 < eps

  plus Configuration a
  plus Configuration b
  Remove
endproc
