# Praat script runAllTests_batch.praat
# Paul Boersma 2020-05-08
#
# This script runs all Praat scripts in its subdirectories.

executable$ = "~/builds/mac_products/Configuration64/Praat.app/Contents/MacOS/Praat"

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
			if not index (file$, "_GUI_.praat")
				path$ = directory$ + "/" + file$
				appendInfoLine: "### executing ", path$, ":"
				runSystem: executable$, " --run """, path$, """"
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
				if not index (file$, "_GUI_.praat")
					path$ = directory1$ + "/" + directory2$ + "/" + file$
					appendInfoLine: "### executing ", path$, ":"
					runSystem: executable$, " --run """, path$, """"
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
