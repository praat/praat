#! Praat script TextGridWindow.praat
# Paul Boersma 2022-05-24

exitScript: "Please run this script only in parts."

#
# Text-cursor-based label splitting.
#
Create Sound from formula: "sineWithNoise", 1, 0.0, 1.0, 44100, ~1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
To TextGrid: "one two", ""
Set interval text: 1, 1, "abcd"
selectObject: "Sound sineWithNoise", "TextGrid sineWithNoise"
View & Edit
#
# If:
#    you put the text cursor between "ab" and "cd" in the text field,
#    then click in the boundary insertion circle on tier 1
# Then:
#    a boundary should appear on tier 1,
#    with "ab" in the interval on its left,
#    and "cd" in the interval on its right
#
# (last checked 2022-05-24 for Mac, Windows, Linux)
#
# If:
#    you select the cd interval,
#    then put the time cursor somewhere in the "cd" interval,
#    then click in the boundary insertion circle on tier 2
# Then:
#    a boundary should appear on tier 2,
#    with nothing in the interval on its left,
#    and nothing in the interval on its right
#
# (last checked 2022-05-24 for Mac, Windows, Linux)
#
# If:
#    you put "hallo" in the first interval on tier 2,
#    then you select the "ab" interval on tier 1,
#    then put the time cursor somewhere in the "ab" interval,
#    then click in the boundary insertion circle on tier 2
# Then:
#    a boundary should appear on tier 2,
#    with "hallo" in the interval on its left,
#    and nothing in the interval on its right
#
# (last checked 2022-05-24 for Mac, Windows, Linux)
#
removeObject: "Sound sineWithNoise", "TextGrid sineWithNoise"
