# test_Sound_voiceActivity.praat
# djmw 20210317
# ppgb 20240104 typo

random_initializeWithSeedUnsafelyButPredictably (0.5)
noise = Create Sound from formula: "noise", 1, 0, 1, 44100, ~ randomGauss(0,1)
random_initializeSafelyAndUnpredictably ()
textgrid = nowarn To TextGrid (voice activity): 0.0, 0.3, 0.1, 70, 6000, -10, -35, 0.1, 0.1, "silent", "sounding"
numberOfIntervals = Get number of intervals: 1
assert numberOfIntervals = 1
label$ = Get label of interval: 1, 1
assert label$ = "silent"

sound = Read from file: "Example_vorbis_audio.ogg"
textgrid2 = To TextGrid (voice activity): 0.0, 0.3, 0.1, 70, 6000, -10, -35, 0.1, 0.1, "silent", "sounding"
numberOfIntervals = Get number of intervals: 1
assert numberOfIntervals = 1
label$ = Get label of interval: 1, 1
assert label$ = "sounding"

selectObject: sound
spectrogram = To Spectrogram: 0.02, 6000, 0.01, 20, "Hanning (sine-squared)"
matrix = Get long-term spectral flatness: 0.3, 0.1, 70, 6000


removeObject: noise, textgrid, matrix, spectrogram, sound, textgrid2

