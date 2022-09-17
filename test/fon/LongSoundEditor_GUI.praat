# ==============
# a script which replicates the crash in Praat 6.2.15.
## --------
## Assertion failed in the file "SoundArea.h" at line 105:
##  our soundOrLongSound() && our soundOrLongSound() -> ny > 0
## --------
# (NOTE1) The contents of (Long)Sound and TextGrid are irrelevant here.
# The point is that you cannnot "View & Edit" TextGrid and LongSound Objects.
# (NOTE2) you can obtain the same result if you
# (1) comment out the last line (i.e., View & Edit),
# (2) run the script, and
# (3) press "View & Edit" manually.

lsID = Open long sound file: "examples/example.wav"
tgID = To TextGrid: "Mary John bell", "bell"
selectObject: lsID, tgID
View & Edit
Remove
# ==============
