# Discriminant3.praat
# Paul Boersma 2020-05-14

writeInfoLine: "Discriminant3"

Debug: 0, 52

table = Create TableOfReal (Pols 1973): "no"
Save as text file: "kanweg.Table"
Formula: "log10(self)"
computedDiscriminant = To Discriminant
Save as text file: "kanweg.Discriminant"
plusObject: table
classificationTable = To ClassificationTable: "yes", "yes"
Save as text file: "kanweg.ClassificationTable"
confusion = To Confusion: "no"
Save as text file: "kanweg.Confusion"
fractionCorrect = Get fraction correct
appendInfoLine: "Computed correct: ", fractionCorrect
removeObject: classificationTable, confusion, computedDiscriminant, table

Debug: 0, 0

appendInfoLine: "Discriminant3 OK"
