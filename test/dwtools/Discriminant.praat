# Discriminant.praat
# Paul Boersma 2016-01-01

#
# This test follows the Praat manual page on "Discriminant analysis"
#
writeInfoLine: "test discriminant analysis"

table = Create TableOfReal (Pols 1973): "no"
Formula: ~ if col <= 3 then log10 (self) else self fi
Standardize columns
Set column label (index): 1, "standardized log (%F__1_)"
Set column label (index): 2, "standardized log (%F__2_)"
Set column label (index): 3, "standardized log (%F__3_)"
;Set column label (index): 4, "standardized %L__1_"
;Set column label (index): 5, "standardized %L__2_"
;Set column label (index): 6, "standardized %L__3_"

Erase all
Select outer viewport: 0.0, 5.0, 0.0, 5.0
Draw scatter plot: 1, 2, 0, 0, -2.9, 2.9, -2.9, 2.9, 10, "yes", "+", "yes"

discriminant = To Discriminant
test1 = Read from file: "Pols.text.Discriminant"
test2 = Read from file: "Pols.binary.Discriminant"
assert objectsAreIdentical (test1, test2)

plusObject: table
configuration = To Configuration: 0
Erase all
Draw: 1, 2, -2.9, 2.9, -2.9, 2.9, 12, "yes", "+", "yes"

removeObject: table, discriminant, test1, test2, configuration

appendInfoLine: "OK"
