# test_Electroglottogram.praat
# djmw 20190402

appendInfoLine : "test_Electroglottogram.praat"

pitchFloor = 50
pitchCeiling = 500
closingThreshold = 0.30
silenceThreshold = 0.03
sound = Read from file: "s_egg_test.wav"
egg = Extract Electroglottogram: 1, "no"
textgrid = To TextGrid (closed glottis): pitchFloor, pitchCeiling, closingThreshold, silenceThreshold
selectObject: egg
degg = Derivative: 5000, 50, 0.99
selectObject: egg
To AmplitudeTier (levels): pitchFloor, pitchCeiling, closingThreshold, "yes", "yes"
peaks = selected ("AmplitudeTier", 1)
valleys = selected ("AmplitudeTier", 2)
levels = selected ("AmplitudeTier", 3)

removeObject: levels, valleys, peaks, degg, textgrid, egg, sound

appendInfoLine : "test_Electroglottogram.praat OK"
