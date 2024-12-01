# dwtest/runAllTests.praat
# 2024-12-01 writing settings to UTF-8 for Praat 7

#
# Some tests require standard settings for the text input encoding:
# files in UTF-8 format should always be readable.
#
Text writing preferences: "UTF-8"
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

tests = Create Strings as file list: "tests", "test_*.praat"
ntests = Get number of strings
for itest to ntests
	selectObject: tests
	test$ = Get string: itest
	appendInfoLine: test$
	report_before$ = Report memory use
	random_initializeWithSeedUnsafelyButPredictably (5489)
	runScript: test$
	random_initializeSafelyAndUnpredictably()
	@check_memory: report_before$, "   "
endfor

procedure check_memory: .report_before$, .preprint$
	.m$[1] = "Strings: "
	.m$[2] = "Arrays: "
	.m$[3] = "Things: "
	.report_after$ = Report memory use
	appendInfoLine: .preprint$, "Memory:"
	for .i to 3
		.nb = extractNumber (.report_before$, .m$[.i])
		.na = extractNumber (.report_after$, .m$[.i])
		.post$ = if .nb <> .na then " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" else "" endif
		appendInfoLine: .preprint$, .m$[.i], .nb, " ", .na, .post$
	endfor
endproc
removeObject: tests

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
