#	Show a bug in Praat introduced after Praat version 6.0.40
#
#	J J A Pacilly, 20-aug-2019, for Paul Boersma

id1 = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
id2 = Copy: "sineWithNoise"
selectObject: id1, id2
id = Concatenate with overlap: 0.01

formant = To Formant (burg): 0, 1, 2000, 0.025, 50
removeObject: id1, id2, id, formant