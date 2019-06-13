# Praat script runAllPraatTests.praat
# Paul Boersma 2018-08-30
#
# This script runs all Praat scripts in its subdirectories.

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

memoryReport$ = Report memory use
strings_before = extractNumber (memoryReport$, "Strings:")
arrays_before  = extractNumber (memoryReport$, "Arrays:")
things_before  = extractNumber (memoryReport$, "Things:")
other_before   = extractNumber (memoryReport$, "Other:")

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
			path$ = directory$ + "/" + file$
			appendInfoLine: "### executing ", path$, ":"
			runScript: path$
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
				path$ = directory1$ + "/" + directory2$ + "/" + file$
				appendInfoLine: "### executing ", path$, ":"
				runScript: path$
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

memoryReport$ = Report memory use
strings_after = extractNumber (memoryReport$, "Strings:")
arrays_after  = extractNumber (memoryReport$, "Arrays:")
things_after  = extractNumber (memoryReport$, "Things:")
other_after   = extractNumber (memoryReport$, "Other:")

appendInfoLine ()
appendInfoLine: "Leaking:"
appendInfoLine: "   Strings: ", strings_after - strings_before
appendInfoLine: "   Arrays: ", arrays_after - arrays_before
appendInfoLine: "   Things: ", things_after - things_before
appendInfoLine: "   Other: ", other_after - other_before

