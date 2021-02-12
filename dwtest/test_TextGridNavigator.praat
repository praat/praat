# test_TextGridNavigator.praat
# djmw 20210211

textgrid = Read from file: "TIMIT_train_dr1_fcjf0_sa1_extended.TextGrid"
navigationContext1 = Create NavigationContext: "cv",  "vowels", "ix eh ih ux ao", "is equal to",
... "consonants", "sh hv jh k s q r w y", "is equal to",
... "", "", "is equal to", "left", "no"
selectObject: textgrid, navigationContext1
navigator = To TextGridNavigator: 1

index# = {3, 5, 8, 15, 21, 23, 25, 29, 35 }
label$# = {"sh", "hv", "jh", "s", "r" , "s", "w", "w", "y" }
combi$# = {"sh |ix", "hv | eh", "jh | ih", "s | ux", "r | ix", "s | ix", "w | ao", "w | ao", "y | ih" }

time = 0.0
for i to size (index#)
	index = Get next match after time: time
	assert index = index# [i]; 'index'
	label$ = Get label (l.c.): 1
	assert label$ = label$# [i]; 'index' 'label$'
	time = Get current end time
endfor

# combine two Navigation contexts tier 1 and tier 3
navigationContext2 = Create NavigationContext: "nouns", "noun", "suit wash water year", "is equal to",
... "", "", "is equal to", "", "", "is equal to", "no left and no right", "no"
selectObject: navigator, navigationContext2
Add navigation context: 3, "touches left and right"


removeObject: textgrid, navigationContext, navigator