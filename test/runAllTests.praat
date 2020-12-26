# Praat script runAllTests.praat
# Paul Boersma 2020-12-26
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

folders$# = folders$# (".")
for folder to size (folders$#)
	folder$ = folders$# [folder]
	if folder$ <> "manually" and folder$ <> "speed"
		files$# = files$# (folder$ + "/*.praat")
		for file to size (files$#)
			file$ = files$# [file]
			path$ = folder$ + "/" + file$
			appendInfoLine: "### executing ", path$, ":"
			runScript: path$
		endfor
	endif
endfor

folders1$# = folders$# (".")
for folder1 to size (folders1$#)
	folder1$ = folders1$# [folder1]
	if folder1$ <> "manually" and folder1$ <> "speed"
		folders2$# = folders1$# (folder1$ + "/*")
		for folder2 to size (folders2$#)
			folder2$ = folders2$# [folder2]
			files$# = files$# (folder1$ + "/" + folder2$ + "/*.praat")
			for file to size (files$#)
				file$ = files$# [file]
				path$ = folder1$ + "/" + folder2$ + "/" + file$
				appendInfoLine: "### executing ", path$, ":"
				runScript: path$
			endfor
		endfor
	endif
endfor

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

