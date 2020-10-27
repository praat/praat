my.Sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
my.Harmonicity = To Harmonicity (cc): 0.01, 75.0, 0.1, 1.0
mean = Get mean: 0.0, 0.005
writeInfoLine: mean
removeObject: my.Sound, my.Harmonicity

