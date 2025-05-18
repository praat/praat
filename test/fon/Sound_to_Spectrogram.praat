# test/fon/Sound_to_Spectrogram.praat
# Paul Boersma, 18 May 2025

Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"

asserterror Your sound is too short:'newline$'it should be at least as long as two window lengths.
To Spectrogram: 5, 5000, 0.002, 20, "Gaussian"

asserterror No frequency bins in spectrogram analysis.
To Spectrogram: 0.005, 1, 0.002, 20, "Gaussian"

Remove
