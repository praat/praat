# run_all_tests.praat

tests = Create Strings as file list... tests test_*.praat
ntests = Get number of strings
for itest to ntests
	select tests
	test$ = Get string... itest
	printline 'test$'
	execute 'test$'
	endif
endfor