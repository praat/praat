# Praat script runAllTests_batch.praat
# Paul Boersma 2020-04-16
#
# This script runs all Praat scripts in its subdirectories.
# This script is to be called from the command line:
#     praat --run runAllTests_batch.praat
#
# The subdirectories `manually` and `speed` are ignored,
# and scripts containing `_GUI_` in their names are ignored.
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

directories = Create Strings as directory list: "directories", "."
numberOfDirectories = Get number of strings
for directory to numberOfDirectories
	selectObject: directories
	directory$ = Get string: directory
	if directory$ <> "manually" and directory$ <> "speed"
		files = Create Strings as file list: "files", directory$ + "/*.praat"
		numberOfFiles = Get number of strings
		for file to numberOfFiles
			selectObject: files
			file$ = Get string: file
			if not index (file$, "_GUI_")
				path$ = directory$ + "/" + file$
				appendInfoLine: "### executing ", path$, ":"
				runScript: path$
			endif
		endfor
		removeObject: files
	endif
endfor
removeObject: directories

directories1 = Create Strings as directory list: "directories1", "."
numberOfDirectories1 = Get number of strings
for directory1 to numberOfDirectories1
	selectObject: directories1
	directory1$ = Get string: directory1
	if directory1$ <> "manually" and directory$ <> "speed"
		directories2 = Create Strings as directory list: "directories2", directory1$ + "/*"
		numberOfDirectories2 = Get number of strings
		for directory2 to numberOfDirectories2
			selectObject: directories2
			directory2$ = Get string: directory2
			files = Create Strings as file list: "files", directory1$ + "/" + directory2$ + "/*.praat"
			numberOfFiles = Get number of strings
			for file to numberOfFiles
				selectObject: files
				file$ = Get string: file
				if not index (file$, "_GUI_")
					path$ = directory1$ + "/" + directory2$ + "/" + file$
					appendInfoLine: "### executing ", path$, ":"
					runScript: path$
				endif
			endfor
			removeObject: files
		endfor
		removeObject: directories2
	endif
endfor
removeObject: directories1

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
