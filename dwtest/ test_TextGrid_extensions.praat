# test_TextGrid_extensions.praat
# djmw 20110523

printline test_TextGrid_extensions

s = Read Strings from raw text file... TIMIT_train_dr1_fcjf0_sa1.phn
ns = Get number of strings
tg = Read from file... TIMIT_train_dr1_fcjf0_sa1.phn

ntiers = Get number of tiers
assert ntiers = 2

nints = Get number of intervals... 1


assert nints = ns

for i to ns
	select s
	string$ = Get string... i
	slabel$ = replace_regex$ (string$, "[0-9\s]", "", 0)
	select tg
	tglabel$ = Get label of interval... 1 i
	assert slabel$ = tglabel$
endfor
select s
plus tg
Remove

printline test_TextGrid_extensions OK

