# Discriminant2.praat
# Paul Boersma 2020-05-06

writeInfoLine: "Discriminant2"

table = Create TableOfReal (Pols 1973): "no"
Formula: "log10(self)"

procedure tryOldFormat
	oldDiscriminant = Read from file: "pols_50males_format0.Discriminant"
	Save as text file: "kanweg.Discriminant"
	plusObject: table
	classificationTable = To ClassificationTable: "yes", "yes"
	confusion = To Confusion: "no"
	fractionCorrect = Get fraction correct
	appendInfoLine: "Old format correct: ", fractionCorrect
	removeObject: classificationTable, confusion, oldDiscriminant
endproc

procedure tryNewFormat
	newDiscriminant = Read from file: "kanweg.Discriminant"
	plusObject: table
	classificationTable = To ClassificationTable: "yes", "yes"
	confusion = To Confusion: "no"
	fractionCorrect = Get fraction correct
	appendInfoLine: "New format correct: ", fractionCorrect
	removeObject: classificationTable, confusion, newDiscriminant
endproc

procedure tryComputation
	selectObject: table
	computedDiscriminant = To Discriminant
	plusObject: table
	classificationTable = To ClassificationTable: "yes", "yes"
	confusion = To Confusion: "no"
	fractionCorrect = Get fraction correct
	appendInfoLine: "Computed correct: ", fractionCorrect
	removeObject: classificationTable, confusion, computedDiscriminant
endproc

@tryComputation
@tryOldFormat
@tryNewFormat

@tryComputation
@tryOldFormat
@tryNewFormat

@tryNewFormat
@tryOldFormat
@tryComputation

@tryNewFormat
@tryOldFormat
@tryComputation

removeObject: table

appendInfoLine: "Discriminant2 OK"
