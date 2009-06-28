# ba-da-continuum.praat
# djmw 20090506

# de KlattGrid versie

nsounds = 10
t1 = 0.05
t2 = t1+0.05
tm = 0.3

kg = Create KlattGrid... kg 0 tm 9 1 1 6  1 1 1

Add pitch point... 0 150

Add pitch point... tm 100

Add voicing amplitude point... t1 65
Add voicing amplitude point... t2 80
Add open phase point... t1 0.7
Add collision phase point... t1 0.05

#Add aspiration amplitude point... t1 30
#Add aspiration amplitude point... t2 40
 
Add oral formant frequency point... 1 t1 100
Add oral formant bandwidth point... 1 t1 50
Add oral formant frequency point... 1 t2 800

Add oral formant bandwidth point... 2 t1 100
Add delta formant bandwidth point... 1 t1 100

for iformant from 3 to 9
  frequency = (2 * iformant - 1) * 500
  Add oral formant frequency point... iformant t2 frequency
  Add oral formant bandwidth point... iformant t1 frequency 
  Add oral formant bandwidth point... iformant t2 frequency / 15
endfor

for isound to nsounds
  select kg
  Remove oral formant frequency points... 2 0 tm
  f2 = 100 + (2900 / (nsounds - 1)) * (isound - 1)
  Add oral formant frequency point... 2 t1 f2
  Add oral formant frequency point... 2 t2 1200
  To Sound
  Fade out... All tm -0.005 y
  Rename... bada'isound'
endfor

# cleanup

select kg
#Remove

# old style according manual Source-filter synthesis 3. does not work nbeacause of error in script

procedure ba_da_old_style
  tm = 0.4
  t1 = 0.05
  pt = Create PitchTier... f0 0 tm
  Add point... 0 150
  Add point... tm 100
  pp = To PointProcess
  Remove points between... 0 t1
  Remove points between... tm-0.05 tm
  s = To Sound (phonation)... 44100 0.6 0.05 0.7 0.03 3.0 4.0

  intens = Create IntensityTier... intens 0 tm
  Add point... t1 60
  Add point... t1+0.05 80

  plus s
  source = Multiply

  for i to 10
    f2_locus = 500+(2500/9)*(i-1)
    fg = Create FormantGrid... filter 0 tm 9 0.001 1000 0 100
    Remove formant points between... 1 0 tm
    Add formant point... 1 t1 100
    Add bandwidth point... 1 t1 50
    Add formant point... 2 t1 f2_locus
    Add bandwidth point... 2 t1 100
    plus source
    Filter
    Rename... old_bada'i'
    select fg
    Remove
  endfor

  select source
  plus s
  plus intens
  plus pp
  plus pt
  Remove
endproc  



