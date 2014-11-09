# test_Discriminant.praat
# djmw 20110518, 20141030

appendInfoLine: "test_Discriminant"

t = Create TableOfReal (Pols 1973): "no"
Formula: "log10(self)"

dis = To Discriminant
plus t
clas = To ClassificationTable: "yes", "yes"
conf = To Confusion: "no"
fc = Get fraction correct
assert fc -0.74 < 0.00001

removeObject: t, dis, clas, conf

appendInfoLine: "test_Discriminant OK"
