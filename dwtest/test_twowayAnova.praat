# test_twowayAnova.praat
# djmw 20120625

printline two-way anova test

# equal groups hayes pg 446

table = Read from file... Hays_table_12.7.1.txt
report$ = Report two-way anova... Data Norms Standing n
group = extractNumber (report$, " Norms")
standing = extractNumber (report$, " Standing")
interaction =  extractNumber (report$, " Norms x Standing")
error = extractNumber (report$, " Error")
total = extractNumber (report$, " Total")

assert abs (group - 4.27) < 0.01
assert abs (standing - 4994.13) < 0.01
assert abs (interaction - 810.13) < 0.01
assert abs (error - 643.20) < 0.01
assert abs (total - 6451.73) < 0.01

Remove
printline Two-way anova test OK


