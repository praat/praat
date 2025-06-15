echo FFT speed:
stopwatch
sound1 = Create Sound from formula... sine mono 0 100 44100
... 1/2 * sin (2 * pi * 377 * x)
spectrum = To Spectrum... yes
sound2 = To Sound
plus sound1
plus spectrum
Remove
t = stopwatch
printline 't:3' seconds