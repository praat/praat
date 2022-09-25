# test_Sound_to_Spectrum_resampled.praat
# djmw 20220408

appendInfoLine: "test_Sound_to_Spectrum_resampled.praat"

tone = Create Sound as pure tone: "3000", 1, 0, 0.02531645569620253, 44100, 3000, 0.2, 0.01, 0.01
dft = To Spectrum: "no"
cog_dft = Get centre of gravity: 2.0
selectObject: tone
resampled = To Spectrum (resampled): 30
cog_resampled = Get centre of gravity: 2.0
assert abs (cog_dft - cog_resampled)  / cog_dft < 1e-6
removeObject: tone, dft, resampled

appendInfoLine: "test_Sound_to_Spectrum_resampled.praat OK"