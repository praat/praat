sound = Read from file: "../fon/logicalVersusPhysical.Sound"
lpc = noprogress To LPC (burg): 16, 0.025, 0.005, 50.0
numberOfFrames = Get number of frames
assert numberOfFrames = 191   ; 'numberOfFrames'
removeObject: sound, lpc

