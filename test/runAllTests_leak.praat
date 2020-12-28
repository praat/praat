# Praat script runAllTests_leak.praat
# Paul Boersma 2020-12-28
#
# This script runs all Praat scripts in its subdirectories twice,
# and reports its suspicions of memory leaks.
# To see whether a leak actually exists, run the suspected script
# multiple times from the script window, each time followed by `Report memory use`.

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

writeInfoLine: "Running all tests twice..."

# Warm up.
strings_before = 0
tensors_before  = 0
things_before  = 0
other_before   = 0
strings_after = 0
tensors_after  = 0
things_after  = 0
other_after   = 0

leakReport$ = ""

procedure doScript: .path$
	runScript: .path$
	Erase all
	writeInfo ()

	memoryReport$ = Report memory use
	memoryReport$ = Report memory use
	strings_before = extractNumber (memoryReport$, "Strings:")
	arrays_before  = extractNumber (memoryReport$, "Tensors:")
	things_before  = extractNumber (memoryReport$, "Things:")
	other_before   = extractNumber (memoryReport$, "Other:")

	runScript: .path$
	Erase all
	writeInfo ()

	memoryReport$ = Report memory use
	strings_after = extractNumber (memoryReport$, "Strings:")
	arrays_after  = extractNumber (memoryReport$, "Tensors:")
	things_after  = extractNumber (memoryReport$, "Things:")
	other_after   = extractNumber (memoryReport$, "Other:")

	if strings_after <> strings_before or arrays_after <> arrays_before or
	... things_after <> things_before or other_after <> other_before
		appendInfoLine ()
		leakReport$ = leakReport$ + "Leaking in " + .path$ + ":" + newline$
		leakReport$ = leakReport$ + "   Strings: " + string$ (strings_before) + " " + string$ (strings_after) + newline$
		leakReport$ = leakReport$ + "   Tensors: " + string$ (tensors_before) + " " + string$ (tensors_after) + newline$
		leakReport$ = leakReport$ + "   Things: " + string$ (things_before) + " " + string$ (things_after) + newline$
		leakReport$ = leakReport$ + "   Other: " + string$ (other_before) + " " + string$ (other_after) + newline$
	endif
endproc

topPath$ = "."
topFolderNames$# = folderNames$# (topPath$)
for topFolder to size (topFolderNames$#)
	topFolderName$ = topFolderNames$# [topFolder]
	if topFolderName$ <> "manually" and topFolderName$ <> "speed"
		topFolderPath$ = topPath$ + "/" + topFolderName$
		@runFilesInFolder: topFolderPath$
		subfolderNames$# = folderNames$# (topFolderPath$ + "/*")
		for subfolder to size (subfolderNames$#)
			subFolderPath$ = topFolderPath$ + "/" + subfolderNames$# [subfolder]
			@runFilesInFolder: subFolderPath$
		endfor
	endif
endfor

procedure runFilesInFolder: .folderPath$
	.fileNames$# = fileNames$# (.folderPath$ + "/*.praat")
	for .file to size (.fileNames$#)
		.filePath$ = .folderPath$ + "/" + .fileNames$# [.file]
		appendInfoLine: "### executing ", .filePath$, ":"
		random_initializeWithSeedUnsafelyButPredictably (5489)
		@doScript: .filePath$
		random_initializeSafelyAndUnpredictably()
	endfor
endproc

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
appendInfoLine: leakReport$
