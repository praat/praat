echo Logistic regression
# Paul Boersma, 16 December 2015

printline Spec & Dur table
table = Read Table from table file... logisticRegression/rh.Table
logreg = To logistic regression: "Spec Dur", "/I/", "/i/"
logreg2 = Read from file: "logisticRegression/rh.LogisticRegression"
assert objectsAreIdentical (logreg, logreg2)
info$ = Info
intercept = extractNumber (info$, "Intercept: ")
assert fixed$ (intercept, 4) = "-8.7028"   ; 'intercept'
spec = extractNumber (info$, "Coefficient of factor Spec: ")
assert fixed$ (spec, 4) = "1.6587"   ; 'spec'
dur = extractNumber (info$, "Coefficient of factor Dur: ")
assert fixed$ (dur, 4) = "0.6041"   ; 'dur'
removeObject: table, logreg, logreg2

printline OK
