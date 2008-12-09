# KlattGrid_test.praat
# djmw 20081208

numberOfNormalFormants = 6
numberOfNasalFormants = 1
numberOfTrachealFormants = 1
numberOfFricationFormants = 5
numberOfDeltaFormants = 1
tmin = 0
tmax = 1

kg = Create KlattGrid... kg tmin tmax numberOfNormalFormants numberOfNasalFormants numberOfTrachealFormants numberOfFricationFormants numberOfDeltaFormants

for .itime to 20
  tmid = randomUniform (tmin, tmax)
  call setget_1 "pitch" tmid 130
  call setget_1 "flutter" tmid 0.5
  call setget_1 "open phase" tmid 0.5
  call setget_1 "collision phase" tmid 0.035
  call setget_1 "spectral tilt" tmid 20

  call setget_2p "delta formant" 1 tmid 50
  call setget_2p "delta bandwidth" 1 tmid 50

  call setget_1 "frication bypass" tmid 20
  call setget_1 "frication amplitude" tmid 50

  call setget_2 "phonation amplitude" tmid 70 Voicing
  call setget_2 "phonation amplitude" tmid 70 Aspiration
  call setget_2 "phonation amplitude" tmid 70 Breathyness

  call setget_2p "flow function" "Power1" tmid 2
  call setget_2p "flow function" "Power2" tmid 3
  
  call setget_formants "Normal" tmid
  call setget_formants "Nasal" tmid
  call setget_formants "Tracheal" tmid
  call setget_formants "Frication" tmid
  call setget_antiformants "Nasal" tmid
  call setget_antiformants "Tracheal" tmid
endfor

procedure setget_1 .var$ .time .value
  select kg
  Add '.var$' point... .time .value
  .val = Get '.var$' at time... .time
  assert .val = .value; Add '.var$' at time... '.time' '.value'
  Remove '.var$' points between... tmin tmax
endproc

procedure setget_2 .var$ .time .value .choice1$
  select kg
  Add '.var$' point... .time .value '.choice1$'
  .val = Get '.var$' at time... .time '.choice1$'
  assert .val = .value; Add '.var$' at time... '.time' '.value' '.choice1$'
  Remove '.var$' points between... tmin tmax '.choice1$'
endproc

procedure setget_2p .var$ .choice1$ .time .value
  select kg
  Add '.var$' point... '.choice1$' .time .value
  .val = Get '.var$' at time... '.choice1$' .time
  assert .val = .value; Add '.var$' at time... '.choice1$ '.time' '.value'
  Remove '.var$' points between... '.choice1$' tmin tmax
endproc

procedure setget_3 .var$ .choice1 .time .value .choice2$
  select kg
  Add '.var$' point... '.choice1' .time .value '.choice2$'
  .val = Get '.var$' at time... '.choice1' .time '.choice2$'
  assert .val = .value; Add '.var$' at time... '.choice1' '.time' '.value'
  Remove '.var$' points between... '.choice1' tmin tmax '.choice2$'
endproc

procedure setget_formants .type$ .time
  for .i to numberOf'.type$'Formants
    .f = (2*.i-1)*500
    .b = .f /10
    .a = 90 / .i^2
    call setget_3 "formant" .i .time .f '.type$' formant
    call setget_3 "bandwidth" .i .time .b '.type$' formant
    call setget_3 "amplitude" .i .time .a '.type$' formant
  endfor
endproc

procedure setget_antiformants .type$ .time
  for .i to numberOf'.type$'Formants
    .f = (2*.i-1)*500
    .b = .f /10
    call setget_3 "formant" .i .time .f '.type$' formant
    call setget_3 "bandwidth" .i .time .b '.type$' formant
  endfor
endproc


