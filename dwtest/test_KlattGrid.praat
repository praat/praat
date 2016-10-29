# test_KlattGrid.praat
# djmw 20151020

printline test_KlattGrid.praat

t = Create Table with column names... t 25 tier min max nf
Set string value... 1 tier pitch
Set string value... 2 tier voicing amplitude
Set string value... 3 tier flutter
Set string value... 4 tier power1
Set string value... 5 tier power2
Set string value... 6 tier open phase
Set string value... 7 tier collision phase
Set string value... 8 tier double pulsing
Set string value... 9 tier spectral tilt
Set string value... 10 tier aspiration amplitude
Set string value... 11 tier breathiness amplitude
Set numeric value... 1 min 80
Set numeric value... 2 min 0
Set numeric value... 3 min 0
Set numeric value... 4 min 1
Set numeric value... 5 min 2
Set numeric value... 6 min 0
Set numeric value... 7 min 0
Set numeric value... 8 min 0
Set numeric value... 9 min 0
Set numeric value... 10 min 0
Set numeric value... 11 min 0
Set numeric value... 1 max 400
Set numeric value... 2 max 93
Set numeric value... 3 max 1
Set numeric value... 4 max 4
Set numeric value... 5 max 5
Set numeric value... 6 max 0.9
Set numeric value... 7 max 0.05
Set numeric value... 8 max 1
Set numeric value... 9 max 40
Set numeric value... 10 max 90
Set numeric value... 11 max 90

Set string value... 12 tier oral formant frequency
Set string value... 13 tier nasal formant frequency
Set string value... 14 tier nasal antiformant frequency
Set string value... 15 tier tracheal formant frequency
Set string value... 16 tier tracheal antiformant frequency
Set string value... 17 tier delta formant frequency
Set string value... 18 tier frication formant frequency

Set numeric value... 12 min 100
Set numeric value... 13 min 100
Set numeric value... 14 min 0
Set numeric value... 15 min 200
Set numeric value... 16 min 100
Set numeric value... 17 min 0
Set numeric value... 18 min 100

Set numeric value... 12 max 6000
Set numeric value... 13 max 6000
Set numeric value... 14 max 6000
Set numeric value... 15 max 2000
Set numeric value... 16 max 1000
Set numeric value... 17 max 1000
Set numeric value... 18 max 8000

Set string value... 19 tier oral formant bandwidth
Set string value... 20 tier nasal formant bandwidth
Set string value... 21 tier nasal antiformant bandwidth
Set string value... 22 tier tracheal formant bandwidth
Set string value... 23 tier tracheal antiformant bandwidth
Set string value... 24 tier delta formant bandwidth
Set string value... 25 tier frication formant bandwidth

nof = 8
nnf = 3
nnaf = 4
ntf = 4
ntaf = 2
ndf = 2
nff = 6

Set numeric value... 12 nf nof
Set numeric value... 13 nf nnf
Set numeric value... 14 nf nnaf
Set numeric value... 15 nf ntf
Set numeric value... 16 nf ntaf
Set numeric value... 17 nf ndf
Set numeric value... 18 nf nff

for i from 19 to 25
	bwmin = Object_'t'[i-7, "min"]
	bwmax = Object_'t'[i-7, "max"]
	nf = Object_'t'[i-7, "nf"]
	Set numeric value... i min bwmin/10
	Set numeric value... i max bwmax/10
	Set numeric value... i nf nf
endfor

nPhonationTiers = 11

tmin = 0
tmax = 1
kg = Create KlattGrid... kg tmin tmax nof nnf nnaf nff ntf ntaf ndf
ntimes = 5
dt = 0.0001

for i to nPhonationTiers
	select kg
	tier$ = Object_'t'$[i,"tier"]
	vmin =  Object_'t'[i,"min"]
	vmax =  Object_'t'[i,"max"]
	for itime to ntimes
		time[itime] = randomUniform (tmin, tmax)
		val[itime] = randomUniform (vmin, vmax)
		Add 'tier$' point... time[itime] val[itime]
	endfor

	for itime to ntimes
		val = Get 'tier$' at time... time[itime]
		assert val = val[itime]
	endfor

	Remove 'tier$' points... tmin tmax

	tier = Extract 'tier$' tier
	Remove
	printline 'tab$'... 'tier$'
endfor

for i from nPhonationTiers+1 to 25
	select kg
	tier$ = Object_'t'$ [i, "tier"]
	vmin =  Object_'t' [i, "min"]
	vmax =  Object_'t' [i, "max"]
	nf = Object_'t' [i, "nf"]
	print 'tab$'... 'tier$'
	for ifor from 1 to nf
		select kg
		for itime to ntimes
			time[itime] = randomUniform (tmin, tmax)
			val[itime] = randomUniform (vmin, vmax)
			Add 'tier$' point... ifor time[itime] val[itime]
		endfor

		for itime to ntimes
			val = Get 'tier$' at time... ifor time[itime]
			assert val = val[itime]
		endfor

		Remove 'tier$' points... ifor tmin tmax
		grid$ = replace$ (tier$, " frequency", "", 1)
		grid$ = replace$ (grid$, " bandwidth", "", 1)
		grid = Extract 'grid$' grid
		print  'tab$''ifor'
		Remove
	endfor
	print 'newline$'
endfor

removeObject: kg, t

printline test_KlattGrid.praat OK