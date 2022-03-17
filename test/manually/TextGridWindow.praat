#! Praat script TextGridWindow.praat
# Paul Boersma 2022-02-19

exitScript: "Please run this script only in parts."

#
# Text-cursor-based label splitting.
#
Create Sound from formula: "sineWithNoise", 1, 0.0, 1.0, 44100, ~1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
To TextGrid: "single", ""
Set interval text: 1, 1, "abcd"
selectObject: "Sound sineWithNoise", "TextGrid sineWithNoise"
View & Edit
#
# If:
#    you put the text cursor between "ab" and "cd" in the text field,
#    then click on the boundary insertion circle
# Then:
#    a boundary should appear,
#    with "ab" in the interval on its left,
#    and "cd" in the interval on its right
#
# (last checked 2022-02-19 for Mac, Windows, Linux)
#
removeObject: "Sound sineWithNoise", "TextGrid sineWithNoise"