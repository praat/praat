# Praat script runAllTests_batch.praat
# Paul Boersma 2020-04-18
#
# This script runs all Praat scripts in its subdirectories.
# This script is to be called from the command line:
#     praat --run runAllTests_batch.praat
#
# Scripts containing `_GUI_` in their names are ignored.
#

#
# Some tests require standard settings for the text input encoding:
# files in UTF-8 format should always be readable.
#
Text writing preferences: "try ASCII, then UTF-16"
if macintosh
	Text reading preferences: "try UTF-8, then MacRoman"
elif windows
	Text reading preferences: "try UTF-8, then Windows Latin-1"
elif unix
	Text reading preferences: "try UTF-8, then ISO Latin-1"
else
	exitScript: "Unknown operating system."
endif

writeInfoLine: "Running all tests..."

files = Create Strings as file list: "files", "*.praat"
numberOfFiles = Get number of strings
for file to numberOfFiles
		selectObject: files
		file$ = Get string: file
		if not index (file$, "runAllTests") and not index (file$, "_GUI_")
			appendInfoLine: "### executing ", file$, ":"
			random_initializeWithSeedUnsafelyButPredictably (5489)
			runScript: file$
			random_initializeSafelyAndUnpredictably()
		endif
	endfor
	removeObject: files
endif

writeInfoLine: "                 ALL PRAAT TESTS WENT OK"
appendInfoLine: ""
line$ [5] = "        #####          #####        #####   #####"
line$ [8] = "        #####          #####        #######"
line$ [1] = "               ######               #####           #####"
line$ [2] = "           ##############           #####         #####"
line$ [4] = "        #####          #####        #####     #####"
line$ [7] = "        #####          #####        #########"
line$ [9] = "        #####          #####        #####"
line$ [3] = "         #####        #####         #####       #####"
line$ [6] = "        #####          #####        ##### #####"
for line from 1 to 9
	appendInfoLine: line$ [line]
endfor
for line from 1 to 8
	appendInfoLine: line$ [9 - line]
endfor
