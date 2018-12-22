# test_EditCostTable.praat
# djmw 20120522

printline test_EditCostTable

numberOfTargets = 4
numberOfSources = 4
table = Create empty EditCostsTable... editCosts numberOfTargets numberOfSources

printline 'tab$'Set values...
 
for i to numberOfTargets
	targetSymbol$[i] = "t'i'"
	Set target symbol (index)... i t'i'
	Set insertion costs... t'i' numberOfSources*i
endfor
for i to numberOfSources
	sourceSymbol$[i] = "s'i'"
	Set source symbol (index)... i s'i'
	Set deletion costs... s'i' numberOfTargets*i
endfor

cost = 0
for i to numberOfTargets
	for j to numberOfSources
		cost += 1
		Set substitution costs... t'i' s'j' cost
	endfor
endfor

for i to numberOfSources
	Set substitution costs... nomatch s'i'  20
endfor
for i to numberOfTargets
	Set substitution costs... t'i' nomatch  21
endfor
Set costs (others)... 1 2 3 4

costs = Get cost (others)... Insertion
assert costs = 1
costs = Get cost (others)... Deletion
assert costs = 2
costs = Get cost (others)... Equality
assert costs = 3
costs = Get cost (others)... Inequality
assert costs = 4

printline 'tab$'Get values...

for i to numberOfTargets
	target$ = targetSymbol$[i]
	index = Get target index... 'target$'
	assert index = i
	cost = Get insertion cost... 'target$'
	assert cost = numberOfSources*i
endfor

for i to numberOfSources
	source$ = sourceSymbol$[i]
	index = Get source index... 'source$'
	assert index = i
	cost = Get deletion cost... 'source$'
	assert cost = numberOfTargets*i
endfor

cost = 0
for i to numberOfTargets
	target$ = targetSymbol$[i]
	for j to numberOfSources
		source$ = sourceSymbol$[j]
		cost += 1
		scost = Get substitution cost... 'target$' 'source$'
		assert scost = cost; 'target$' 'source$'
	endfor
endfor

for i to numberOfSources
	source$ = sourceSymbol$[i]
	cost = Get substitution cost... nomatch 'source$'
	assert cost = 20
endfor

for i to numberOfTargets
	target$ = targetSymbol$[i]
	cost = Get substitution cost... 'target$' nomatch
	assert cost =  21
endfor

Remove
printline test_EditCostTable OK