# test_Discriminant.praat
# djmw 20110518

printline test_Discriminant

t = Create TableOfReal (Pols 1973)... no
Formula... log10(self)

dis = To Discriminant
plus t
clas = To ClassificationTable... yes yes
conf = To Confusion
fc = Get fraction correct
assert fc -0.74 < 0.00001

select t
plus dis
plus clas
plus conf
Remove

printline test_Discriminant OK
