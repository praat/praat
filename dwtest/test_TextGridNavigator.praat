# test_TextGridNavigator.praat
# djmw 20210211

appendInfoLine: "test_TextGridNavigator.praat"

textgrid = Read from file: "TIMIT_train_dr1_fcjf0_sa1_extended.TextGrid"

numberOfTiers = Get number of tiers
numberOfIntervals# = zero# (numberOfTiers)
for itier to numberOfTiers
	numberOfIntervals# [itier] = Get number of intervals: itier
endfor

@test_topic
@test_topicAndBefore
@test_topicAndBeforeAndAfter
@test_twoNavigationContexts
@test_threeNavigationContexts
@test_timing

removeObject: textgrid

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

	# locate vowels from the set "ix eh ih ux ao ah"
	selectObject: textgrid
	.navigator = To TextGridNavigator (topic only): 1, "ix eh ih ux ao ah", "is equal to", "OR", "Match start to Match end"
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

	Modify Topic match criterion: 1, "is not equal to", "AND"
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = numberOfIntervals# [1] - .numberOfVowels
	Modify Topic match criterion: 1, "is equal to", "OR"
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = .numberOfVowels
	selectObject: .navigator, textgrid
	Replace search tiers
	selectObject: .navigator
	.numberOfMatches = Get number of matches
	assert .numberOfMatches = .numberOfVowels
	removeObject: .navigator
	appendInfoLine: tab$, "test topic OK"
endproc

procedure test_topicAndBefore
	appendInfoLine: tab$, "test  topic + before"

	# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" before them
	selectObject: textgrid
	.navigator = To TextGridNavigator: 1, "ix eh ih ux ao ah", "is equal to", "OR",
	... "sh hv jh k s q r w y", "is equal to", "OR", "", "is equal to", "AND", "before", "no", "Topic start to Topic end"

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

	# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" 
	# before them and one of "hv q sh" after them
	selectObject: textgrid
	.navigator = To TextGridNavigator: 1, "ix eh ih ux ao ah", "is equal to", "OR",
	... "sh hv jh k s q r w y", "is equal to", "OR", 
	... "hv q sh", "is equal to", "OR", 
	... "before and after", "no", "Topic start to Topic end"
	Modify Topic match criterion: 1, "is equal to", "OR"
	Modify Before match criterion: 1, "is equal to", "OR"
	Modify After match criterion: 1, "is equal to", "OR"
	Modify combination criterion: 1, "before and after", "no"
	Modify match domain: 1, "Topic start to Topic end"
	Modify Before range: 1, 1, 1
	Modify After range: 1, 1, 1
	.index1# = {3, 15,  25}
	.index_before1# = {2, 14, 24}
	.index_after1# = {4, 16, 26}
	.beforeLabels$# = {"sh", "s", "w" }
	.afterLabels$# = {"hv", "q", "sh"}

	Find first
	for .i to size (.index1#)
		.index = Get index: 1, "topic"
		assert .index = .index1# [.i]; '.index'
		.index_before = Get index: 1, "before"
		assert .index_before = .index_before1# [.i]
		.label$ = Get label: 1, "before"
		assert .label$ = .beforeLabels$# [.i]; '.index' '.label$'
		.index_after = Get index: 1, "after"
		assert .index_after = .index_after1# [.i]
		.label$ = Get label: 1, "after"
		assert .label$ = .afterLabels$# [.i]; '.index' '.label$'
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

	appendInfoLine: tab$, tab$, "Check lists of labels, times, domains"

	.labels$# = List labels: "before"
	assert size (.labels$#) = size (.index1#)
	for .i to size (.labels$#)
		assert .labels$# [.i] = .beforeLabels$# [.i]
	endfor
	.labels$# = List labels: "after"
	assert size (.labels$#) = size (.index1#)
	for .i to size (.labels$#)
		assert .labels$# [.i] = .afterLabels$# [.i]
	endfor
	topicIndices# = List indices: "topic"
	beforeIndices# = List indices: "before"
	afterIndices# = List indices: "after"
	topicStartTimes# = List start times: "topic"
	topicEndTimes# = List end times: "topic"
	beforeStartTimes# = List start times: "before"
	beforeEndTimes# = List end times: "before"
	afterStartTimes# = List start times: "after"
	afterEndTimes# = List end times: "after"
	domainstt## = List domains: "Topic start to Topic end"
	assert numberOfRows (domainstt## ) == size (.index1#)
	domainsmm## = List domains: "Match start to Match end"
	assert numberOfRows (domainsmm## ) == size (.index1#)
	for .index to numberOfRows (domainstt## )
		selectObject: textgrid
		topicStartTime = Get start time of interval: 1, topicIndices# [.index]
		topicEndTime = Get end time of interval: 1, topicIndices# [.index]
		assert topicStartTime = topicStartTimes# [.index]
		assert topicEndTime = topicEndTimes# [.index]

		beforeStartTime = Get start time of interval: 1, beforeIndices# [.index]
		beforeEndTime = Get end time of interval: 1, beforeIndices# [.index]
		assert beforeStartTime = beforeStartTimes# [.index]
		assert beforeEndTime = beforeEndTimes# [.index]

		afterStartTime = Get start time of interval: 1, afterIndices# [.index]
		afterEndTime = Get end time of interval: 1, afterIndices# [.index]
		assert afterStartTime = afterStartTimes# [.index]
		assert afterEndTime = afterEndTimes# [.index]

		assert topicStartTime = domainstt## [.index, 1]
		assert topicEndTime = domainstt## [.index, 2]
		assert beforeStartTime = domainsmm## [.index, 1]
		assert afterEndTime = domainsmm## [.index, 2]
	endfor
	appendInfoLine: tab$, tab$, "Check lists of labels, times, domains OK"
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

	# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" before them
	selectObject: textgrid
	.navigator = To TextGridNavigator: 1, "ix eh ih ux ao ah", "is equal to", "OR",
		... "sh hv jh k s q r w y", "is equal to", "OR", "", "is equal to", "AND", "before", "no", "Topic start to Topic end"
	Rename: "small_LARGE"
	# locate words from the set "suit wash water year" on tier 3
	selectObject: .navigator, textgrid
	Add search tier (topic only): 3, "had suit wash water year", "is equal to", "OR", "Topic start to Topic end", 
		... "overlaps before and after"
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

	selectObject: textgrid
	.navigator2 = To TextGridNavigator (topic only): 3, "had suit wash water year", "is equal to", "OR", "Topic start to Topic end"
	Rename: "LARGE_small"
	selectObject: .navigator2, textgrid
	Add search tier: 1, "ix eh ih ux ao ah", "is equal to", "OR", "sh hv jh k s q r w y", "is equal to", "OR",
		 ... "", "is equal to", "AND", "before", "no", "Topic start to Topic end", "is inside"
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

	# locate vowels from the set "ix eh ih ux ao ah" that also have one of "sh hv jh k s q r w y" before them
	selectObject: textgrid
	.navigator = To TextGridNavigator: 1, "ix eh ih ux ao ah", "is equal to", "OR",
	... "sh hv jh k s q r w y", "is equal to", "OR", "", "is equal to", "AND", "before", "no", "Topic start to Topic end"
	selectObject: .navigator, textgrid
	Add search tier (topic only): 3, "had suit wash water year", "is equal to", "OR", "Topic start to Topic end", 
		... "overlaps before and after"
	Add search tier (topic only): 4, "N", "is equal to", "OR", "Topic start to Topic end", "overlaps before and after"
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

procedure test_timing
	stopwatch
	.numberOfCreations = 1000
	for .i to .numberOfCreations
		selectObject: textgrid
		.navigator = To TextGridNavigator: 1, "ix eh ih ux ao ah", "is equal to", "OR",
		... "sh hv jh k s q r w y", "is equal to", "OR", "", "is equal to", "AND", "before", "no", "Topic start to Topic end"
		selectObject: .navigator, textgrid
		Add search tier (topic only): 3, "had suit wash water year", "is equal to", "OR", "Topic start to Topic end", 
		... "overlaps before and after"
		Add search tier (topic only): 4, "N", "is equal to", "OR", "Topic start to Topic end", "overlaps before and after"
		selectObject: .navigator
		domains## = List domains: "Topic start to Topic end"
		if .i < .numberOfCreations
			removeObject: .navigator
		endif
	endfor
	time1 = stopwatch
	.numberOfMatches = 10000
	for .i to .numberOfMatches 
		domains## = List domains: "Topic start to Topic end"
	endfor
	removeObject: .navigator
	time2 = stopwatch
	appendInfoLine: tab$, .numberOfCreations, " navigator creations + matches (3 tiers) took ", fixed$ (time1, 3), " s."
	appendInfoLine:  tab$, .numberOfMatches, " navigator matches (3 tiers) took ", fixed$ (time2, 3), " s."
endproc

