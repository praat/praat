sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
ceps = To PowerCepstrogram: 60, 0.002, 5000, 50
cpps = Get CPPS: "yes", 0.02, 0.0005, 60, 330, 0.05, "Parabolic", 0.001, 0, "Exponential decay", "Robust"
assert 4.0 < cpps and cpps < 4.9 ; 'cpps'
removeObject: sound, ceps
