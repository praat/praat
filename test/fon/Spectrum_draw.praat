# test/fon/Spectrum_draw.praat
# Paul Boersma 2017-08-28

# There used to be a bug that caused Praat to crash with an assert message
# if you drew a Spectrum that contained undefined values.

sound = Create Sound as pure tone: "tone", 1, 0, 0.4, 44100, 440, 0.2, 0.01, 0.01
spec = To Spectrum: "yes"
Formula: ~ self / 0
Draw: 0, 0, 0, 0, "yes"

removeObject: sound, spec
