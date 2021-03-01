# test_TextGridNavigator.praat
# djmw 20210211

appendInfoLine: "test_TextGridNavigator.praat"

textgrid = Read from file: "TIMIT_train_dr1_fcjf0_sa1_extended.TextGrid"

numberOfTiers = Get number of tiers
numberOfIntervals# = zero# (numberOfTiers)
for itier to numberOfTiers
	numberOfIntervals# [itier] = Get number of intervals: itier
endfor

# locate vowels from the set "ix eh ih ux ao ah"
navigationContext1 = Create NavigationContext (topic only): "c",  "ix eh ih ux ao ah", "is equal to", "OR"
selectObject: textgrid, navigationContext1
tierNavigator1 = To TextGridTierNavigator: 1, "Match start to Match end"

# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" before them
navigationContext2 = Create NavigationContext: "cv",  "ix eh ih ux ao ah", "is equal to", "OR",
	... "sh hv jh k s q r w y", "is equal to", "OR", "", "is equal to", "AND", "before", "no"
selectObject: textgrid, navigationContext2
tierNavigator2 = To TextGridTierNavigator: 1, "Topic start to Topic end"

# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" 
# before them and one of "hv q sh" after them
navigationContext3 = Create NavigationContext: "cv",  "ix eh ih ux ao ah", "is equal to", "OR",
	... "sh hv jh k s q r w y", "is equal to", "OR", "hv q sh", "is equal to", "OR", "before and after", "no"
selectObject: textgrid, navigationContext3
tierNavigator3 = To TextGridTierNavigator: 1, "Topic start to Topic end"

# locate words from the set "suit wash water year"
navigationContext4 = Create NavigationContext (topic only): "words", "had suit wash water year", "is equal to", "OR"
selectObject: textgrid, navigationContext4
tierNavigator4 = To TextGridTierNavigator: 3, "Topic start to Topic end"

# locate  "N " (on tier 4)
navigationContext5 = Create NavigationContext (topic only): "nouns", "N", "is equal to", "OR"
selectObject: textgrid, navigationContext5
tierNavigator5 = To TextGridTierNavigator: 4, "Topic start to Topic end"

@test_topic
@test_topicAndBefore
@test_topicAndBeforeAndAfter
@test_twoNavigationContexts
@test_threeNavigationContexts

removeObject: textgrid, navigationContext1, navigationContext2, 
	... navigationContext3, navigationContext4, navigationContext5,
	... tierNavigator1, tierNavigator2, tierNavigator3, tierNavigator4,
	... tierNavigator5

appendInfoLine: "test_TextGridNavigator.praat OK"

procedure test_topic
	appendInfoLine: tab$, "test topic"

	.index1# = {3, 5, 8, 11, 15, 21, 23, 25, 29, 32, 35 }
	.numberOfVowels = size (.index1#)
	.startTime1# = zero# (.numberOfVowels)
	.endTime1# = zero# (.numberOfVowels)
	selectObject: textgrid
	for .i to .numberOfVowels
		.startTime1# [.i] = Get start time of interval: 1, .index1# [.i]
		.endTime1# [.i] = Get end time of interval: 1, .index1# [.i]
	endfor

	selectObject: tierNavigator1
	.navigator = To TextGridNavigator
	Find first
	for .i to .numberOfVowels
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]
		.startTime = Get start time: 1, "topic"
		assert .startTime = .startTime1# [.i]; '.index' '.startTime'
		.endTime = Get end time: 1, "topic"
		assert .endTime = .endTime1# [.i]; '.index' '.endTime'
		Find next
	endfor

	Modify Topic criterion: 1, "is not equal to", "AND"
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = numberOfIntervals# [1] - .numberOfVowels
	.navigationContext = Extract navigation context: 1
	Modify Topic criterion: "is equal to", "OR"
	selectObject: .navigator, .navigationContext
	Replace navigation context: 1
	selectObject: .navigator
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = .numberOfVowels
	selectObject: .navigator, textgrid
	Replace TextGrid tiers
	selectObject: .navigator
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = .numberOfVowels
	removeObject: .navigator, .navigationContext
	appendInfoLine: tab$, "test topic OK"
endproc

procedure test_topicAndBefore
	appendInfoLine: tab$, "test  topic + before"

	selectObject: tierNavigator2
	.navigator = To TextGridNavigator
	.index1# = {3, 5, 8, 15, 21, 23, 25, 29, 35 }
	.label1$# = {"sh", "hv", "jh", "s", "r" , "s", "w", "w", "y" }
	.combi1$# = {"sh |ix", "hv | eh", "jh | ih", "s | ux", "r | ix", "s | ix", "w | ao", "w | ao", "y | ih" }

	.time = 0.0
	for .i to size (.index1#)
		Find next after time: .time
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.label$ = Get label: 1, "before"
		assert .label$ = .label1$# [.i]; '.index' '.label$'
		.time = Get end time: 1, "before"
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test  topic + before OK"
endproc

procedure test_topicAndBeforeAndAfter
	appendInfoLine: tab$, "test  before + topic + after"

	selectObject: tierNavigator3
	.navigator = To TextGridNavigator
	.index1# = {3, 15,  25}
	.index_before1# = {2, 14, 24}
	.index_after1# = {4, 16, 26}
	.label_before1$# = {"sh", "s", "w" }
	.label_after1$# = {"hv", "q", "sh"}

	Find first
	for .i to size (.index1#)
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.index_before = Get index: 1, "before"
		assert .index_before = .index_before1# [.i]
		.label$ = Get label: 1, "before"
		assert .label$ = .label_before1$# [.i]; '.index' '.label$'
		.index_after = Get index: 1, "after"
		assert .index_after = .index_after1# [.i]
		.label$ = Get label: 1, "after"
		assert .label$ = .label_after1$# [.i]; '.index' '.label$'
		Find next
	endfor
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = size (.index1#)
	.numberOfTopicMatches = Get number of Topic matches: 1
	assert .numberOfTopicMatches = 11
	.numberOfBeforeMatches = Get number of Before matches: 1
	assert .numberOfBeforeMatches = 12
	.numberOfAfterMatches = Get number of After matches: 1
	assert .numberOfAfterMatches = 4
	
	removeObject: .navigator
	appendInfoLine: tab$, "test  before + topic + after OK"
endproc

procedure test_twoNavigationContexts
	appendInfoLine: tab$, "test two navigation contexts"

	.index1# = {5, 15, 25, 29, 35}
	.numberOfMatches = size (.index1#)
	.index1_before# = {4, 14, 24, 28, 34}
	.label1$# = {"eh", "ux", "ao", "ao", "ih"}
	.index3# = {3, 6, 9, 11, 13}
	# only vowels in these words should match
	.label3$# = {"had", "suit", "wash", "water", "year"}
	.startTime3# = zero# (.numberOfMatches)
	.endTime3# = zero# (.numberOfMatches)
	selectObject: textgrid
	for .i to .numberOfMatches
		.startTime3# [.i] = Get start time of interval: 3, .index3# [.i]
		.endTime3# [.i] = Get end time of interval: 3, .index3# [.i]
	endfor
	appendInfoLine: tab$,tab$, "From small to large"
	# combine  NavigationContext's  on tier 1 and tier 3
	selectObject: textgrid, navigationContext2
	.navigator = To TextGridNavigator: 1, "Topic start to Topic end"
	selectObject: .navigator, tierNavigator4
	Add TextGridTierNavigator: "overlaps before and after"
	selectObject: .navigator
	Find first
	for .i to .numberOfMatches
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.index_before = Get index: 1, "before"
		assert .index_before = .index1_before# [.i]
		.label$ = Get label: 1, "topic"
		assert .label$ = .label1$# [.i]
		.indexc2 = Get index: 3, "topic"
		assert .indexc2 = .index3#[.i]; '.indexc2' '.index3# [.i]'
		.label2$ = Get label: 3, "topic"
		assert .label2$ = .label3$# [.i]
		.startTime = Get start time: 3, "topic"
		assert .startTime = .startTime3# [.i]; '.indexc2' '.startTime'
		.endTime = Get end time: 3, "topic"
		assert .endTime = .endTime3# [.i]; '.indexc2' '.endTime'
		Find next
	endfor
	appendInfoLine: tab$,tab$, "From small to large OK"
	appendInfoLine: tab$,tab$, "From large to small"
	selectObject: tierNavigator4
	.navigator2 = To TextGridNavigator
	Rename: "LARGE_to_small"
	selectObject: .navigator2, tierNavigator2
	Add TextGridTierNavigator: "is inside"
	selectObject: .navigator
	Find first
	for .i to .numberOfMatches
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.index_before = Get index: 1, "before"
		assert .index_before = .index1_before# [.i]
		.label$ = Get label: 1, "topic"
		assert .label$ = .label1$# [.i]
		.indexc2 = Get index: 3, "topic"
		assert .indexc2 = .index3#[.i]; '.indexc2' '.index3# [.i]'
		.label2$ = Get label: 3, "topic"
		assert .label2$ = .label3$# [.i]
		.startTime = Get start time: 3, "topic"
		assert .startTime = .startTime3# [.i]; '.indexc2' '.startTime'
		.endTime = Get end time: 3, "topic"
		assert .endTime = .endTime3# [.i]; '.indexc2' '.endTime'
		Find next
	endfor
	
	removeObject: .navigator, .navigator2
	appendInfoLine: tab$,tab$, "From large to small OK"
	appendInfoLine: tab$, "test two navigation contexts OK"
endproc

procedure test_threeNavigationContexts
	appendInfoLine: tab$, "test three navigation contexts"

	.index1# = {15, 29, 35}
	.numberOfMatches = size (.index1#)
	.index1_before# = {14, 28, 34}
	.label1$# = { "ux", "ao", "ih"}
	.index3# = {6, 11, 13}
	.indext4# = {5, 9, 11}
	.label3$# = {"suit", "water", "year"}
	.startTime3# = zero# (.numberOfMatches)
	.endTime3# = zero# (.numberOfMatches)
	.startTimet4# = zero# (.numberOfMatches)
	.endTimet4# = zero# (.numberOfMatches)
	selectObject: textgrid
	for .i to .numberOfMatches
		.startTime3# [.i] = Get start time of interval: 3, .index3# [.i]
		.endTime3# [.i] = Get end time of interval: 3, .index3# [.i]
		.startTimet4# [.i] = Get start time of interval: 4, .indext4# [.i]
		.endTimet4# [.i] = Get end time of interval: 4, .indext4# [.i]
	endfor

	# combine  NavigationContext's  on tier 1, tier 3 and tier 4
	selectObject: textgrid, navigationContext2
	.navigator = To TextGridNavigator: 1,  "Match start to Match end"
	selectObject: .navigator, tierNavigator4
	Add TextGridTierNavigator: "overlaps before and after"
	selectObject: .navigator, tierNavigator5
	Add TextGridTierNavigator: "overlaps before and after"
	selectObject: .navigator

	Find first
	for .i to .numberOfMatches
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.index_before = Get index: 1, "before"
		assert .index_before = .index1_before# [.i]
		.label$ = Get label: 1, "topic"
		assert .label$ = .label1$# [.i]
		.indexc2 = Get index: 3, "topic"
		assert .indexc2 = .index3#[.i]; '.indexc2' '.index3# [.i]'
		.label2$ = Get label: 3, "topic"
		assert .label2$ = .label3$# [.i]
		.startTime = Get start time: 3, "topic"
		assert .startTime = .startTime3# [.i]; '.indexc2' '.startTime'
		.endTime = Get end time: 3, "topic"
		assert .endTime = .endTime3# [.i]; '.indexc2' '.endTime'
		.indexc3 = Get index: 4, "topic"
		assert .indexc3 = .indext4#[.i]; '.indexc3' '.indext4# [.i]'
		.startTime = Get start time: 4, "topic"
		assert .startTime = .startTimet4# [.i]; '.indexc3' '.startTime'
		.endTime = Get end time: 4, "topic"
		assert .endTime = .endTimet4# [.i]; '.indexc3' '.endTime'
		Find next
	endfor
	removeObject: .navigator
	appendInfoLine: tab$, "test three navigation contexts OK"
endproc


