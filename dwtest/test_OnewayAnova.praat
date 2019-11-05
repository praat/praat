# test_OnewayAnova.praat
# djmw 20120625

printline test_OnewayAnova.praat

# unequal groups hayes pg 371

table = Read from file... Hays_table_10.18.1.txt
report$ = Report one-way anova... Data Group n n n
between = extractNumber (report$, "Between")
within = extractNumber (report$, "Within")
total = extractNumber (report$, "Total")

assert abs (between-22.57) < 0.01
assert abs (within - 878.93) < 0.01
assert abs (total - 901.5) < 0.1
removeObject: table

printline test_OnewayAnova.praat OK
