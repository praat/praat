# test_onewayAnova.praat
# djmw 20120625

printline One-way Anova test

# unequal groups hayes pg 371

table = Read from file... Hayes_table_10.18.1.txt
report$ = Report one-way anova... Data Group n n n
between = extractNumber (report$, "Between")
within = extractNumber (report$, "Within")
total = extractNumber (report$, "Total")

assert abs (between-22.57) < 0.01
assert abs (within - 878.93) < 0.01
assert abs (total - 901.5) < 0.1
Remove
printline One-way Anova test OK


