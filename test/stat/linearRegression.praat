echo Linear regression
# Paul Boersma, 23 November 2015

printline Spec & Dur table
Read Table from table file... logisticRegression/rh.Table
To linear regression
info$ = Info
intercept = extractNumber (info$, "Intercept: ")
assert fixed$ (intercept, 4) = "10.0000"   ; 'intercept'
spec = extractNumber (info$, "Coefficient of factor Spec: ")
assert left$ (fixed$ (abs (spec), 4), 6) = "0.0000"   ; 'spec'
dur = extractNumber (info$, "Coefficient of factor Dur: ")
assert left$ (fixed$ (abs (dur), 4), 6) = "0.0000"   ; 'dur'
i = extractNumber (info$, "Coefficient of factor /I/: ")
assert fixed$ (i, 4) = "-1.0000"   ; 'i'
plus Table rh
Remove

printline OK
