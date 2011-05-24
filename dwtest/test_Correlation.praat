# test_Correlation.praat
# djmw 20110518

printline test_Correlation

t = Create TableOfReal (Pols 1973)... no
Formula... log10(self)
cor = To Correlation

# table 3.1 Weenink (2006)
c21ref$ = "-0.302"
c31ref$ = "0.195"
c32ref$ = "0.120"

c21 = Get value... 2 1
c31 = Get value... 3 1
c32 = Get value... 3 2

assert fixed$(c21,3) = c21ref$
assert fixed$(c31,3) = c31ref$
assert fixed$(c32,3) = c32ref$

plus t
Remove

printline test_Correlation OK



