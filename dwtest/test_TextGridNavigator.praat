# test_TextGridNavigator.praat
# djmw 20210211

appendInfoLine: "test_TextGridNavigator.praat"

textgrid = Read from file: "TIMIT_train_dr1_fcjf0_sa1_extended.TextGrid"

# locate vowels from the set "ix eh ih ux ao ah"
navigationContext1 = Create NavigationContext: "c",  "vowels", "ix eh ih ux ao ah", "is equal to",
	... "before", "", "is equal to",
	... "after", "", "is equal to", "no before and no after", "no"

# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" before them
navigationContext2 = Create NavigationContext: "cv",  "vowels", "ix eh ih ux ao ah", "is equal to",
	... "before", "sh hv jh k s q r w y", "is equal to",
	... "after", "", "is equal to", "before", "no"

# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" 
# before them and one of "hv q sh" after them
navigationContext3 = Create NavigationContext: "cv",  "vowels", "ix eh ih ux ao ah", "is equal to",
	... "before", "sh hv jh k s q r w y", "is equal to",
	... "after", "hv q sh", "is equal to", "before and after", "no"

# locate words from the set "suit wash water year"
navigationContext4 = Create NavigationContext: "nouns", "noun", "suit wash water year", "is equal to",
	... "before", "", "is equal to", "after", "", "is equal to", "no before and no after", "no"

@test_onlyVowels
@test_vowelsAndBefore
@test_vowelsAndBeforeAndAfter
@test_twoNavigationContexts

removeObject: textgrid, navigationContext1, navigationContext2, navigationContext3, navigationContext4

appendInfoLine: "test_TextGridNavigator.praat OK"

procedure test_onlyVowels
	appendInfoLine: tab$, "test only vowels NavigationContext"

	.index# = {3, 5, 8, 11, 15, 21, 23, 25, 29, 32, 35 }
	.numberOfVowels = size (.index#)
	.startTime# = zero# (.numberOfVowels)
	.endTime# = zero# (.numberOfVowels)
	selectObject: textgrid
	for .i to .numberOfVowels
		.startTime# [.i] = Get start time of interval: 1, .index# [.i]
		.endTime# [.i] = Get end time of interval: 1, .index# [.i]
	endfor

	selectObject: textgrid, navigationContext1
	.navigator = To TextGridNavigator: 1
	Locate first
	for .i to .numberOfVowels
		.index = Get index: 1, "topic"
		assert .index = .index# [.i]
		.startTime = Get start time: 1, "topic"
		assert .startTime = .startTime# [.i]; '.index' '.startTime'
		.endTime = Get end time: 1, "topic"
		assert .endTime = .endTime# [.i]; '.index' '.endTime'
		Locate next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test only vowels NavigationContext OK"
endproc

procedure test_vowelsAndBefore
	appendInfoLine: tab$, "test  vowels + before"

	selectObject: textgrid, navigationContext2
	.navigator = To TextGridNavigator: 1
	.index# = {3, 5, 8, 15, 21, 23, 25, 29, 35 }
	.label$# = {"sh", "hv", "jh", "s", "r" , "s", "w", "w", "y" }
	.combi$# = {"sh |ix", "hv | eh", "jh | ih", "s | ux", "r | ix", "s | ix", "w | ao", "w | ao", "y | ih" }

	.time = 0.0
	for .i to size (.index#)
		Locate next after time: .time
		.index = Get index: 1, "topic"
		assert .index = .index# [.i]; '.index'
		.label$ = Get label: 1, "before"
		assert .label$ = .label$# [.i]; '.index' '.label$'
		.time = Get end time: 1, "before"
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test  vowels + before OK"
endproc

procedure test_vowelsAndBeforeAndAfter
	appendInfoLine: tab$, "test  before + vowel + after"

	selectObject: textgrid, navigationContext3
	.navigator = To TextGridNavigator: 1
	.index# = {3, 15,  25}
	.index_before# = {2, 14, 24}
	.index_after# = {4, 16, 26}
	.label_before$# = {"sh", "s", "w" }
	.label_after$# = {"hv", "q", "sh"}

	Locate first
	for .i to size (.index#)
		.index = Get index: 1, "topic"
		assert .index = .index# [.i]; '.index'
		.index_before = Get index: 1, "before"
		assert .index_before = .index_before# [.i]
		.label$ = Get label: 1, "before"
		assert .label$ = .label_before$# [.i]; '.index' '.label$'
		.index_after = Get index: 1, "after"
		assert .index_after = .index_after# [.i]
		.label$ = Get label: 1, "after"
		assert .label$ = .label_after$# [.i]; '.index' '.label$'
		Locate next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test  before + vowel + after OK"
endproc

procedure test_twoNavigationContexts
	appendInfoLine: tab$, "test two navigation contexts"

	.index1# = {15, 25, 29, 35}
	.numberOfs = size (.index1#)
	.index1_lc# = {14, 24, 28, 34}
	.label1$# = {"ux", "ao", "ao", "ih"}
	.index3# = {6, 9, 11, 13}
	# only vowels in these words should match
	.label3$# = {"suit", "wash", "water", "year"}
	.startTime3# = zero# (4)
	.endTime3# = zero# (4)
	selectObject: textgrid
	for .i to 4
		.startTime3# [.i] = Get start time of interval: 3, .index3# [.i]
		.endTime3# [.i] = Get end time of interval: 3, .index3# [.i]
	endfor

	# combine  NavigationContext's  on tier 1 and tier 3
	selectObject: textgrid, navigationContext2
	.navigator = To TextGridNavigator: 1
	selectObject: .navigator, navigationContext4
	Add navigation context: 3, "overlaps before and after"
	selectObject: .navigator
	Locate first
	for .i to size (.index1#)
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.index_lc = Get index: 1, "before"
		assert .index_lc = .index1_lc# [.i]
		.label$ = Get label: 1, "topic"
		assert .label$ = .label1$# [.i]
		.indexc2 = Get index: 2, "topic"
		assert .indexc2 = .index3#[.i]; '.indexc2' '.index3# [.i]'
		.label2$ = Get label: 2, "topic"
		assert .label2$ = .label3$# [.i]
		.startTime = Get start time: 2, "topic"
		assert .startTime = .startTime3# [.i]; '.indexc2' '.startTime'
		.endTime = Get end time: 2, "topic"
		assert .endTime = .endTime3# [.i]; '.indexc2' '.endTime'
		Locate next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test two navigation contexts OK"
endproc



