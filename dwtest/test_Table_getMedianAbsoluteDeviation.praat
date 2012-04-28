# test_Table_getMedianAbsoluteDeviation.praat
# djmw 20120405

# create dataset with median 2
tab = Create Table with column names... table 7 c1
Set numeric value... 1 c1 1
Set numeric value... 2 c1 1
Set numeric value... 3 c1 2
Set numeric value... 4 c1 2
Set numeric value... 5 c1 4
Set numeric value... 6 c1 6
Set numeric value... 7 c1 9

mad = Get median absolute deviation... c1
assert abs (mad -1.4826) < 1.1e-16
Remove

printline  test_Table_getMedianAbsoluteDeviation ok











