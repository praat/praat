# KlattSlope.praat
# Paul Boersma, 25 November 2010

# This script tests whether a sound created with the Klatt synthesizer
# is similar to a sound created with "To Sound (phonation)".

openPhase = 0.7
collisionPhase = 0.03;hertzToErb(300)/35;0.03
power = 3.0
f1 = 250
b1 = 100
f2 = 3200
b2 = 100
duration = 0.8
f0start = 150
f0end = 110
;bandwidthFormula$ = "100"
bandwidthFormula$ = "(formant-0.5)*1000/10"

numberOfFormants = 20

pitchTier = Create PitchTier... a 0 duration
Add point... 0 f0start
Add point... duration f0end
pulses = To PointProcess
sound = To Sound (phonation)... 44100 1.0 0.05 openPhase collisionPhase power power+1.0
;sound = To Sound (pulse train)... 44100 1.0 0.05 2000
Filter with one formant (in-line)... f1 b1
Filter with one formant (in-line)... f2 b2
for formant from 3 to numberOfFormants
	Filter with one formant (in-line)... (formant-0.5)*1000 'bandwidthFormula$'
endfor
spectrum = To Spectrum... yes
slope1 = Get band density difference... 0 1000 2000 4000
echo 'slope1:6' dB
removeObject: pitchTier, pulses, sound, spectrum

klatt = Create KlattGrid... a 0 duration numberOfFormants 0 0 0 0 0 0
Add voicing amplitude point... 0 90.0
Add pitch point... 0 f0start
Add pitch point... duration f0end
Add power1 point... 0 power
Add power2 point... 0 power+1.0
Add open phase point... 0 openPhase
Add collision phase point... 0 collisionPhase
Add oral formant frequency point... 1 0 f1
Add oral formant bandwidth point... 1 0 b1
Add oral formant frequency point... 2 0 f2
Add oral formant bandwidth point... 2 0 b2
for formant from 3 to numberOfFormants
	Add oral formant frequency point... formant 0 (formant-0.5)*1000
	Add oral formant bandwidth point... formant 0 'bandwidthFormula$'
endfor
;To Sound (special)... 0 0 44100 yes yes yes yes yes yes "Powers in tiers" yes yes yes Cascade 1 numberOfFormants 1 1 0 0 0 0 0 0 0 0 0 0 0 0 yes
To Sound
To Spectrum... yes
slope2 = Get band density difference... 0 1000 2000 4000
printline 'slope2:6' dB
removeObject: klatt, "Sound a", "Spectrum a"
