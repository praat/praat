# KlattGrid_test.praat
# djmw 20081208


printline ===========    KlattGrid test start

call test_get_add_remove_extract_replace
call test_deltaFormants


printline ===========    KlattGrid test end

procedure test_get_add_remove_extract_replace
  numberOfNormalFormants = 6
  numberOfNasalFormants = 1
  numberOfTrachealFormants = 1
  numberOfFricationFormants = 5
  numberOfNasalAntiFormants = 1
  numberOfTrachealAntiFormants = 1
  numberOfDeltaFormants = 1
  tmin = 0
  tmax = 1

  printline ===========    KlattGrid test_get_add_remove_extract_replace start

  kg = Create KlattGrid... kg tmin tmax numberOfNormalFormants numberOfNasalFormants numberOfNasalAntiFormants 
    ... numberOfFricationFormants numberOfTrachealFormants numberOfTrachealAntiFormants numberOfDeltaFormants

  for .itime to 20
    call setget_1 "pitch" 130
    call setget_1 "voicing amplitude" 90
    call setget_1 "flutter" 0.5
    call setget_1 "power1" 2
    call setget_1 "power2" 3
    call setget_1 "open phase" 0.5
    call setget_1 "collision phase" 0.035
    call setget_1 "double pulsing" 0.4
    call setget_1 "spectral tilt" 20
    call setget_1 "aspiration amplitude" 90
    call setget_1 "breathiness amplitude" 90

    call setget_1 "frication bypass" 20
    call setget_1 "frication amplitude" 50
  
    call setget_formants Normal
    call setget_formants Nasal
    call setget_formants Tracheal
    call setget_formants Frication
    call setget_antiformants Nasal
    call setget_antiformants Tracheal

    call setget_deltaformants

  endfor

  select kg
  Remove
  printline ===========    KlattGrid test_get_add_remove_extract_replace succesful
endproc

procedure setget_1 .var$ .value
  select kg
  .time = randomUniform (tmin,tmax)
  Add '.var$' point... .time .value
  .val = Get '.var$' at time... .time
  .vt = Extract '.var$' tier
  select kg
  Remove '.var$' points between... tmin tmax
  .val1 = Get '.var$' at time... .time
  assert .val1 = undefined; Add '.var$' at time... '.time' '.value'
  
  plus .vt
  Replace '.var$' tier
  select kg
  .val1 = Get '.var$' at time... .time
  assert .val1 =.val; Add '.var$' at time... '.time' '.value'
  select .vt
  Remove
endproc

procedure setget_2p .var$ .ifor .time .value
  select kg
  Add delta '.var$' point... .ifor .time .value
  .val = Get delta '.var$' at time... .ifor .time
  assert .val = .value; Add delta '.var$' point... .ifor .time .value
  .fg = Extract delta formant grid
  select kg
  Remove delta '.var$' points between... .ifor tmin tmax
  .val1 =  Get delta '.var$' at time... .ifor .time
  assert .val1 = undefined; Get delta '.var$' at time... .ifor .time
  plus .fg
  Replace delta formant grid
  select kg
  .val1 =  Get delta '.var$' at time... .ifor .time
  assert .val1 =.val; Get delta '.var$' at time... .ifor .time
  select .fg
  Remove
endproc

procedure setget_3 .var$ .choice$ .ifor .time .value
  select kg
  Add '.var$' point... "'.choice$'" .ifor .time .value
  .val = Get '.var$' at time... "'.choice$'" .ifor .time
  assert .val = .value; Add '.var$' point... "'.choice$'" .ifor .time .value
  .fg = Extract formant grid... '.choice$'
  select kg
  Remove '.var$' points between... "'.choice$'" .ifor tmin tmax
  .val1 =  Get '.var$' at time... "'.choice$'" .ifor .time
  assert .val1 = undefined; Get '.var$' at time... "'.choice$'" .ifor .time
  plus .fg
  Replace formant grid... '.choice$'
  select kg
  .val1 =  Get '.var$' at time... "'.choice$'" .ifor .time
  assert .val1 =.val; Get '.var$' at time... "'.choice$'" .ifor .time
  select .fg
  Remove
endproc

procedure setget_3a .var$ .choice$ .ifor .time .value
  select kg
  Add '.var$' point... "'.choice$'" .ifor .time .value
  .val = Get '.var$' at time... "'.choice$'" .ifor .time
  assert .val = .value; Add '.var$' point... "'.choice$'" .ifor .time .value
  .tier = Extract amplitude tier... "'.choice$'" .ifor
  select kg
  Remove '.var$' points between... "'.choice$'" .ifor tmin tmax
  .val1 =  Get '.var$' at time... "'.choice$'" .ifor .time
  assert .val1 = undefined; Get '.var$' at time... "'.choice$'" .ifor .time
  plus .tier
  Replace amplitude tier... "'.choice$'" .ifor
  select kg
  .val1 =  Get '.var$' at time... "'.choice$'" .ifor .time
  assert .val1 =.val; Get '.var$' at time... "'.choice$'" .ifor .time
  select .tier
  Remove
endproc

procedure setget_formants .type$
  for .i to numberOf'.type$'Formants
    .time = randomUniform (tmin, tmax)
    .f = (2*.i-1) * randomUniform (450, 550)
    .b = .f /10
    .a = randomUniform (70, 90)
    call setget_3 "formant" "'.type$' formant" .i .time .f
    call setget_3 "bandwidth" "'.type$' formant" .i .time .b
    call setget_3a "amplitude" "'.type$' formant" .i .time .a
  endfor
endproc


procedure setget_deltaformants
  for .i to numberOfDeltaFormants
    .time = randomUniform (tmin, tmax)
    .f = randomUniform (50, 60)
    .b = .f /10
    call setget_2p "formant" .i .time .f
    call setget_2p "bandwidth" .i .time .b
  endfor
endproc

procedure setget_antiformants .type$
  for .i to numberOf'.type$'AntiFormants
    .time = randomUniform (tmin, tmax)
    .f = (2*.i-1) * randomUniform (450, 550)
    .b = .f /10
    call setget_3 "formant" "'.type$' antiformant" .i .time .f
    call setget_3 "bandwidth" "'.type$' antiformant" .i .time .b
  endfor
endproc

procedure test_deltaFormants
  .kg = Create KlattGrid... kg 0 1 6 1 1 6 1 1 1
  Add pitch point... 0.5 100
  Add voicing amplitude point... 0.5 90
  Add bandwidth point... "Normal formant" 1 0.5 50
  Add delta formant point... 1 0.5 500
  Remove formant points between... "Normal formant" 1 0 2
  Add formant point... "Normal formant" 1 0 400
  Add formant point... "Normal formant" 1 1 600
  Add formant point... "Normal formant" 1 0.003553 400
  Add formant point... "Normal formant" 1 0.002 300
  Add formant point... "Normal formant" 1 0.0112 430
  .sound = To Sound... 44100 Cascade 1 5 1 0 1 0 1 0 1 0 1 5 yes yes no
  Remove
  select .kg
  To formant grid (open phases)... 0.1
endproc


