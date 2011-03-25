echo Logistic regression
# Paul Boersma, 30 November 2009

printline Spec & Dur table
Read Table from table file... logisticRegression/rh.Table
To logistic regression... "Spec Dur" /I/ /i/
info$ = Info
intercept = extractNumber (info$, "Intercept: ")
assert fixed$ (intercept, 4) = "-8.7028"   ; 'intercept'
spec = extractNumber (info$, "Coefficient of factor Spec: ")
assert fixed$ (spec, 4) = "1.6587"   ; 'spec'
dur = extractNumber (info$, "Coefficient of factor Dur: ")
assert fixed$ (dur, 4) = "0.6041"   ; 'dur'
plus Table rh
Remove

printline OK
