# test_Electroglottogram.praat
# djmw 20190402

appendInfoLine : "test_Electroglottogram.praat"

sound = Read from file: "s_egg_test.wav"
egg = Extract Electroglottogram: 1
degg = To DElectroglottogram: 


appendInfoLine : "test_Electroglottogram.praat OK"
