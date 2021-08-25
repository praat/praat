# updateScrollBar_GUI_.praat
# Paul Boersma 2021-08-25

#
# Scenario:
#
# Given two open Sound windows for the same Sound object,
# when you Cut in one of them, the cut also appears in the other.
# Praat should ungroup the first window, because it no longer
# fits into a group with any other open windows.
# For the same reason, Praat should ungroup also the other Sound window.
#

sound = Create Sound from formula: "sineWithNoise", 1, 0.0, 1.0,
... 44100, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
manip = To Manipulation: 0.01, 75.0, 600.0
View & Edit
selectObject: sound
View & Edit
editor: sound
	Select: 0.6, 0.7
endeditor
View & Edit
editor: sound
	Cut
endeditor
#
# Now, cutting should have ungrouped the other sound window.
#
selectObject: manip
View & Edit

removeObject: sound, manip
