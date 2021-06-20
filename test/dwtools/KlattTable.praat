# KlattTable.praat
# Paul Boersma 2021-06-16

klattTable = Create KlattTable example
table = To Table

#
# Test whether list initialization worked correctly.
#
numberOfColumns = Get number of columns
assert numberOfColumns = 40
firstColumnLabel$ = Get column label: 1
assert firstColumnLabel$ = "f0"
lastColumnLabel$ = Get column label: numberOfColumns
assert lastColumnLabel$ = "gain"

removeObject: klattTable, table
