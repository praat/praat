writeInfoLine: "Table: Rows to columns"

orig = Create formant table (Peterson & Barney 1952)

# Syntax 1 (command line syntax with "last argument = rest of line": poorly readable)
new1 = nowarn Rows to columns... "Type Sex Speaker" IPA F0 F1 F2 F3

# Syntax 2 (with space-separated strings)
selectObject: orig
new2 = nowarn Rows to columns: "Type Sex Speaker", "IPA", "F0 F1 F2 F3"
assert objectsAreIdentical (new1, new2)

# Syntax 3 (with string arrays)
selectObject: orig
new3 = nowarn Rows to columns: { "Type", "Sex", "Speaker" }, "IPA", { "F0", "F1", "F2", "F3" }
assert objectsAreIdentical (new1, new3)

@test: new1
@test: new2
@test: new3

procedure test: which
	appendInfoLine: which
	selectObject: which
	numberOfRows = Get number of rows
	assert numberOfRows = 76
	value = Get value: 10, "F1.u"
	assert value = 450 or value = 420
	numberOfColumns = Get number of columns
	assert numberOfColumns = 43
	columnName$ = Get column label: 10
	assert columnName$ = "F0.\ic"
endproc

removeObject: orig, new1, new2, new3

appendInfoLine: "OK"
