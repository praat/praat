# test/fon/Sound_to_Pitch.praat
# Paul Boersma, 18 April 2025
# check on rounding errors in parabolic interpolation

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,1)"
Formula: ~ if x < 0.1 or x > 0.9 then 0 else self fi
Subtract mean
pitch = To Pitch (raw autocorrelation): 0.01, 75, 600, 15, "no", 0.03, 0.45, 0.01, 0.35, 0.14
removeObject: sound, pitch
