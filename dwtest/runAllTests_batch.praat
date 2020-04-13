# Praat script runAllTests_batch.praat
# Paul Boersma 2020-04-13
#
# This script runs all Praat scripts in its subdirectories.
# This script is to be called from the command line:
#     praat --run runAllTests.praat
#
# Scripts containing `_GUI_` in their names are ignored.

if macintosh
	executable$ = "~/builds/mac_products/Configuration64/Praat.app/Contents/MacOS/Praat --no-pref-files"
else
	executable$ = "../praat --no-pref-files"
endif

writeInfoLine: "Running all tests..."

files = Create Strings as file list: "files", "*.praat"
numberOfFiles = Get number of strings
for file to numberOfFiles
		selectObject: files
		file$ = Get string: file
		if not index (file$, "runAllTests") and not index (file$, "_GUI_")
			appendInfoLine: "### executing ", file$, ":"
			runSystem: executable$, " --run """, file$, """"
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
