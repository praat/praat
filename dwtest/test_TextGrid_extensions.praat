# test_TextGrid_extensions.praat
# djmw 20110523, 20140904
# ppgb 20180705 corrected regexp

appendInfoLine: "test_TextGrid_extensions"

s = Read Strings from raw text file: "TIMIT_train_dr1_fcjf0_sa1.phn"
ns = Get number of strings
tg = Read from file: "TIMIT_train_dr1_fcjf0_sa1.phn"

ntiers = Get number of tiers
assert ntiers = 2

nints = Get number of intervals: 1


assert nints = ns

for i to ns
	selectObject: s
	string$ = Get string: i
	slabel$ = replace_regex$ (string$, "([0-9]|\s)*", "", 0)
	selectObject: tg
	tglabel$ = Get label of interval: 1, i
	assert slabel$ = tglabel$  ; 'i'
endfor
removeObject: s, tg


appendInfoLine: "test_TextGrid_extensions OK"

