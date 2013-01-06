# test_randomBinomial.praat
# djmw 20121211


printline  randomBinomial  test
p =1/6
n = 100

np = n * p

numberOfRuns = 100000

printline  'tab$''numberOfRuns' runs with randomBinomial (p, n), where p = 'p' and n = 'n'
printline 'tab$'We expect values between 0 and 'n' with a mean near n * p = 'np'
printline 'tab$'Bin 1 counts number of times randomBinomial return 0, bin 2 number of times randomBinomial return 1,  etc.
printline 'tab$'The probability q of the values in bin i equals binomialP (p, i - 1 , n) - binomialP (p, i - 2 , n)
printline 'tab$' In bin i we expect numberOfRuns * q (='numberOfRuns' * q)  values'newline$'

Create TableOfReal... t 3 n+1
for i to numberOfRuns
	nb = randomBinomial (p, n)
	val = Get value... 1 nb+1
	val += 1
	Set value... 1 nb+1 val
endfor

for i from 2 to n+1
	val = Get value... 1 i
	q = binomialP (p, i - 1 , n) - binomialP (p, i - 2 , n)
	expect = round (numberOfRuns * q)
	diff = val - expect
	Set value... 2 i  expect
	Set value... 3 i diff
	printline 'tab$'bin = 'i': diff = 'diff'   (expected = 'expect', q = 'q:3')
endfor

Remove

printline  randomBinomial  test OK
