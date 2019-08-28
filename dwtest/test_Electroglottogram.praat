# test_Electroglottogram.praat
# djmw 20190402

appendInfoLine : "test_Electroglottogram.praat"

pitchFloor = 50
pitchCeiling = 500
closingThreshold = 0.30
silenceThreshold = 0.03
sound = Read from file: "s_egg_test.wav"
egg = Extract Electroglottogram: 1
intervalTier = To IntervalTier: pitchFloor, pitchCeiling, closingThreshold, silenceThreshold
selectObject: egg
degg = To Electroglottogram (derivative): 5000, 50
selectObject: egg
To AmplitudeTier (levels): pitchFloor, pitchCeiling, closingThreshold, "yes", "yes"
peaks = selected ("AmplitudeTier", 1)
valleys = selected ("AmplitudeTier", 2)
levels = selected ("AmplitudeTier", 3)
selectObject: sound, egg, degg
combined = Combine to multi-channel

removeObject: levels, valleys, peaks, degg, intervalTier, egg, sound, combined

appendInfoLine : "test_Electroglottogram.praat OK"
