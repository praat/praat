# test_Confusion.praat
# djmw 20110511, 20151020

printline test_Confusion.praat

l$[1] = "u"
l$[2] = "i"
l$[3] = "e"
nlabels = 3
c1 = Create simple Confusion... test 'l$[1]'  'l$[2]'   'l$[3]' 
nrows = Get number of rows
ncols = Get number of columns
assert nrows = ncols
assert nrows = nlabels
for i to nlabels
	rl$ = Get row label... i
	cl$ = Get column label... i
	assert rl$ = cl$ and l$[i] = rl$
endfor

for i to 10
	stim$ = l$[1]
	resp$ =  l$[2]
	Increase... 'stim$' 'resp$'
	val = Get value (labels)... 'stim$' 'resp$'
	assert val = i
	ssum = Get stimulus sum... 'stim$'
	rsum = Get response sum... 'resp$'
	assert ssum = rsum and ssum = i
endfor
Remove
printline ...2
c2 = Create simple Confusion... test 'l$[1]'  'l$[2]'   'l$[3]' 
Set value... 1 1 6
Set value... 1 2 2
Set value... 1 3 1
Set value... 2 1 3
Set value... 2 2 4
Set value... 2 3 2
Set value... 3 1 1
Set value... 3 2 4
Set value... 3 3 4
select c2
c3 = Group stimuli... "u i" high 1
v = Get value (labels)... high u
assert v = 9
v = Get value (labels)... high i
assert v = 6
v = Get value (labels)... high e
assert v = 3
Remove
printline ...3
select c2
c4 = Group responses... "i e" front 1
v = Get value (labels)... u front
assert v = 3
v = Get value (labels)... i front
assert v = 6
v = Get value (labels)... e front
assert v = 8

plus c2
Remove

printline test_Confusion.praat OK
