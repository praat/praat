# test_TextGridNavigator.praat
# djmw 20210211

appendInfoLine: "test_TextGridNavigator.praat"

textgrid = Read from file: "TIMIT_train_dr1_fcjf0_sa1_extended.TextGrid"

navigationContext1 = Create NavigationContext: "c",  "vowels", "ix eh ih ux ao ah", "is equal to",
	... "lc", "", "is equal to",
	... "rc", "", "is equal to", "no before and no after", "no"
navigationContext2 = Create NavigationContext: "cv",  "vowels", "ix eh ih ux ao ah", "is equal to",
	... "lc", "sh hv jh k s q r w y", "is equal to",
	... "rc", "", "is equal to", "before", "no"
navigationContext3 = Create NavigationContext: "cv",  "vowels", "ix eh ih ux ao ah", "is equal to",
	... "lc", "sh hv jh k s q r w y", "is equal to",
	... "rc", "hv q sh", "is equal to", "before and after", "no"
navigationContext4 = Create NavigationContext: "nouns", "noun", "suit wash water year", "is equal to",
	... "lc", "", "is equal to", "rc", "", "is equal to", "no before and no after", "no"

@test_onlyVowels
@test_vowelsAndLeftContext
@test_vowelsLeftAndRightContext
@test_twoNavigationContexts

removeObject: textgrid, navigationContext1, navigationContext2, navigationContext3, navigationContext4

appendInfoLine: "test_TextGridNavigator.praat OK"

procedure test_onlyVowels
	appendInfoLine: tab$, "test only vowels NavigationContext"
	selectObject: textgrid, navigationContext1
	.navigator = To TextGridNavigator: 1

	.index# = {3, 5, 8, 11, 15, 21, 23, 25, 29, 32, 35 }
	Locate first
	for .i to size (.index#)
		.index = Get index: 1, "topic"
		assert .index = .index# [.i]
		Locate next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test only vowels NavigationContext OK"
endproc

procedure test_vowelsAndLeftContext
	appendInfoLine: tab$, "test  vowels + left context"

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
	appendInfoLine: tab$, "test  vowels + left context  NavigationContext OK"
endproc

procedure test_vowelsLeftAndRightContext
	appendInfoLine: tab$, "test  left context + vowel + right context"

	selectObject: textgrid, navigationContext3
	.navigator = To TextGridNavigator: 1
	.index# = {3, 15,  25}
	.index_lc# = {2, 14, 24}
	.index_rc# = {4, 16, 26}
	.label_lc$# = {"sh", "s", "w" }
	.label_rc$# = {"hv", "q", "sh"}
	.combi$# = {"sh ix hv", "s ux q", "w ao sh" }

	Locate first
	for .i to size (.index#)
		.index = Get index: 1, "topic"
		assert .index = .index# [.i]; '.index'
		.label$ = Get label: 1, "before"
		assert .label$ = .label_lc$# [.i]; '.index' '.label$'
		.label$ = Get label: 1, "after"
		assert .label$ = .label_rc$# [.i]; '.index' '.label$'
		Locate next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test  left context + vowel + right context OK"
endproc

procedure test_twoNavigationContexts
	appendInfoLine: tab$, "test two navigation contexts"

	# combine  NavigationContext's  on tier 1 and tier 3
	selectObject: textgrid, navigationContext2
	.navigator = To TextGridNavigator: 1
	selectObject: .navigator, navigationContext4
	Add navigation context: 3, "overlaps before and after"
	selectObject: .navigator

	.index1# = {15, 25, 29, 35}
	.index1_lc# = {14, 24, 28, 34}
	.label1$# = {"ux", "ao", "ao", "ih"}
	.index3# = {6, 9, 11, 13}
	.label3$# = {"suit", "wash", "water", "year"}

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
		Locate next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test two navigation contexts OK"
endproc



