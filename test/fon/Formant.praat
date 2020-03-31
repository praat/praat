# test/fon/Formant.praat
# Paul Boersma 2020-03-31

tone = Create Sound as pure tone: "tone", 1, 0, 0.4, 44100, 440, 0.2, 0.01, 0.01
spectrum = To Spectrum: "yes"
excitation = To Excitation: 0.1

smooth = To Formant: 20
Save as binary file: "kanweg.Formant"
copy = Copy: "copy"   ; check invariant
Remove

selectObject: spectrum
Formula: ~ 0
peaks = To Formant (peaks): 1000
Save as binary file: "kanweg.Formant"
copy = Copy: "copy"   ; check invariant
Remove

removeObject: peaks, smooth, excitation, spectrum, tone

tone = Create Sound as pure tone: "tone", 1, 0, 10, 44100, 531, 0.2, 0.01, 0.01
formant = To Formant (burg): 0, 5, 5500, 0.025, 50
selectObject: tone
pulses = To PointProcess (periodic, peaks): 75, 600, "yes", "no"
plusObject: formant
tier = To FormantTier
Save as binary file: "kanweg.FormantTier"
removeObject: tier, pulses, formant, tone
