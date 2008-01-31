# LPC_test.praat
# djmw 20080122

f1 = 500
b1 = 50
f2 = 1500
b2 = 100
f3 = 2500
b3 = 150
f4 = 3500
b4 = 200
f5 = 4500
b5 = 300

debug = 0

printline --- LPC_test.praat : test LPC analyses on artificial vowel -----

pt = Create PitchTier... empty 0 0.5
Add point... 0 150
source = To Sound (pulse train)... 44100 1 0.05 2000 no
ft = Create FormantTier... empty 0 0.5
Add point... 0  'f1' 'b1' 'f2' 'b2' 'f3' 'b3' 'f4' 'b4' 'f5' 'b5'
plus source
soundpre = Filter

sound = Resample... 10000 50

select pt
plus source
plus ft
plus soundpre
#Remove

for imethod to 4
  select sound
  if imethod = 1
    method$ = "auto"
    lpc = To LPC (autocorrelation)... 10 0.025 0.005 50
  elsif imethod = 2
    method$ = "covar"
    lpc = To LPC (covariance)... 10 0.025 0.005 50
  elsif imethod = 3
    method$ = "burg"
    lpc = To LPC (burg)... 10 0.025 0.005 50
  elsif imethod = 4
    method$ = "marple"
    lpc = To LPC (marple)... 10 0.025 0.005 50 1e-06 1e-06
  endif
  call get_formants 'lpc' 'method$'
  deltaf1 = deltaf
  deltab1 = deltab
  assert 'deltaf1' < 0.07 and 'deltab1' < 3.2; ('method$')

  select lpc
  plus sound
  lpcr = To LPC (robust)... 0.025 50 1.5 5 1e-06 no
  call get_formants lpcr 'method$':robust
  deltaf2 = deltaf
  deltab2 = deltab
  assert 'deltaf2' < 0.005 and 'deltab2' < 0.2; ('method$':robust)
  select lpc
  plus lpcr
  Remove
endfor

select sound
Remove

printline --- test LPC analyses on artificial vowel END -----

procedure get_formants .lpc .method$
  select .lpc
  .f = To Formant (keep all)
  deltaf = 0
  deltab = 0
  for .nf to 4
    .f'.nf' = Get quantile... .nf 0 0 Hertz 0.5
    .b'.nf' = Get quantile of bandwidth... .nf 0 0 Hertz 0.5
    if .nf <= 4
      deltaf += abs(f'.nf'- .f'.nf') / f'.nf'
      deltab += abs(b'.nf'- .b'.nf') / b'.nf'
    endif
  endfor
  deltaf /= 4
  deltab /= 4
  printline deltaf = 'deltaf:4' deltab = 'deltab:4' '.f1:0' '.b1:0' '.f2:0' '.b2:0' '.f3:0' '.b3:0' '.f4:0' '.b4:0' '.f5:0' '.b5:0' ('.method$')
  select .f
  Remove
endproc

