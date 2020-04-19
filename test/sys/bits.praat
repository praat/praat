integerProperties$ = Report integer properties
bits = extractNumber (integerProperties$, "A pointer is")
writeInfoLine: "This computer is ", bits, "-bits"