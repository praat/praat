# editorScripts.praat
# Paul Boersma, 5 March 2023

exitScript: "Use only “Run selection”."

# From Objects to editor: without “endeditor”.
#
# Try to run the following twice.
#
# This tests whether a script without “endeditor” cleans up correctly.

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	Zoom: 0.3, 0.7

# From Objects to editor: with an unknown variable error.
#
# Try to run the following twice.
#
# This tests whether a script with an error message about an unknown variable cleans up correctly.

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	asserterror Unknown variable:
	fsdlmgslgml
	Select: 0.3, 0.7
	Zoom: 0.3, 0.7

# From Objects to editor: with an unknown menu command error.
#
# Try to run the following twice.
#
# This tests whether a script with an error message about an unknown menu command cleans up correctly.

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	asserterror Command not available in TextGridEditor.
	Zoompppppppppppppp: 0.3, 0.7

# From Objects to editor: with a command syntax error.
#
# Try to run the following twice.
#
# This tests whether a script with an error message about command syntax cleans up correctly.

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	asserterror Command requires more than the given 1 arguments: argument “To” not given.
	Zoom: 0.3

# From Objects to editor: with “endeditor”.
#
# Try to run the following twice.
#
# This tests whether a script without “endeditor” returns to the Objects window correctly.

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	Zoom: 0.3, 0.7
endeditor
Draw: 0.0, 0.0, "yes", "yes", "yes"

# From Objects to editor: with “Close”.
#
# Try to run the following twice.
#
# This tests whether a script without “Close” returns to the Objects window correctly.

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	Zoom: 0.3, 0.7
	Close
Draw: 0.0, 0.0, "yes", "yes", "yes"

# From Objects to editor: with “pauseScript”.
#
# Try to run the following twice.
#
# During the pause, try to close the window (or remove the Sound).

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	asserterror Cannot continue after pause, because the TextGridEditor has been closed.
	pauseScript: "Close the editor window, then continue."
	Zoom: 0.3, 0.7

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textGrid = To TextGrid: "Mary John Bell", "bell"
selectObject: sound, textGrid
View & Edit
editor: textGrid
	Select: 0.3, 0.7
	asserterror Cannot continue after pause, because the TextGridEditor has been closed.
	pause Close the editor window, then continue.
	Zoom: 0.3, 0.7

# Entirely by hand:

# 1. Create a Sound
# 2. Choose View & Edit
# 3. Choose New editor script: check title
# 4. Type something: check change in title
# 5. Remove the Sound: editor closes, title changes, and Run menu becomes insensitive

# Entirely by hand:

# 1. Create a Sound
# 2. Choose View & Edit
# 3. Choose New editor script: check title
# 4. Type something: check change in title
# 5. Close the editor: title changes, and Run menu becomes insensitive

# Entirely by hand:

# 1. Create a Sound
# 2. Choose View & Edit
# 3. Choose Open editor script: check title
# 4. Type something to make the script window dirty: fdsffg
# 5. Remove the Sound: editor closes, title changes, and Run menu becomes insensitive
# 6. Create a Sound
# 7. Choose View & Edit
# 8. Choose Open editor script: message "Text file XXX is already open."
# 9. Remove the Sound: editor closes, title changes, and Run menu becomes insensitive

# Within editor (open this script in an editor window).
#
# Try to run the following twice.
#
# This should continue to work.

Zoom: 0.3, 0.7

# Within editor (open this script in an editor window).
#
# Try to run the following twice.
#
# The following should give an unknown command error.

asserterror Command not available in SoundEditor.
sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"

# Within editor (open this script in an editor window).
#
# Try to run the following twice.
#
# The following should give an unknown variable error.

asserterror Unknown variable:
fdsafshgjh

# Within editor (open this script in an editor window).
#
# Try to run the following twice.
#
# The following should give a command syntax error.

asserterror Command requires more than the given 1 arguments: argument “To” not given.
Zoom: 0.3

# From editor to Objects (open this script in an editor window).
#
# Try to run the following twice.
#
# This should continue to work.

Select: 0.3, 0.7
Extract selected sound (preserve times)
endeditor
	To Intensity: 100, 0, "yes"
	Info
editor
Select: 0.5, 0.6

# runScript from an editor should pass on the editor
#
# This has been automated in: test/script/runScript_GUI_.praat

# Within editor (open this script in an editor window).
#
# Type some text here to make this script window dirty: hfgghjfdgffevdfdsfgfsaf
# During the pause, try to close the window (or remove the Sound).

Select: 0.3, 0.7
asserterror Cannot continue after pause, because the SoundEditor has been closed.
pauseScript: "Close the editor window, then continue."
Zoom: 0.5, 0.6

Select: 0.3, 0.7
asserterror Cannot continue after pause, because the SoundEditor has been closed.
pause Close the editor window, then continue.
Zoom: 0.5, 0.6
