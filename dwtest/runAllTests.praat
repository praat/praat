# run_all_tests.praat

tests = Create Strings as file list: "tests", "test_*.praat"
ntests = Get number of strings
for itest to ntests
	selectObject: tests
	test$ = Get string: itest
	appendInfoLine: test$
	report_before$ = Report memory use
	runScript: test$
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
