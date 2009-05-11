# ba-da-continuum.praat
# djmw 20090506

# de KlattGrid versie

t1 = 0.05
tm = 0.4
kg = Create KlattGrid... kg 0 tm 9 1 1 6  1 1 1
Add pitch point... 0 150
Add pitch point... tm 100

Add voicing amplitude point... t1 60
Add voicing amplitude point... t1+0.03 80
Add open phase point... t1 0.7
Add collision phase point... t1 0.05

Add oral formant frequency point... 1 t1 100
Add oral formant bandwidth point... 1 t1 50
Add oral formant bandwidth point... 2 t1 100

for iformant from 3 to 9
  Add oral formant frequency point... 1 t1 iformant*1000
  Add oral formant bandwidth point... 1 t1 iformant*100  
endfor

for ivowel to 10
  select kg
  Remove oral formant frequency points... 2 0 tm
  f2_locus = 500 + (2500/9)*(ivowel-1) 
  Add oral formant frequency point... 2 t1 f2_locus
  To Sound
  Rename... bada'ivowel'
endfor

# cleanup

select kg
Remove

# old style volgens manual Source-filter synthesis 3. The ba-da continuum 

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
  



