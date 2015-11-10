# test_Covariance.praat
# djmw 2015

sm = Create simple Covariance: "sm", "4.32 1.88 9.18", "0 0",  32

ncols = Get number of columns
nrows = Get number of rows
assert ncols = nrows and ncols = 2; "nrows = ncols = 2"
no = Get number of observations
assert no = 32; "numberOfObservations" 'no' <> 32
dof = Get degrees of freedom
assert dof = no-1 ; "dof = no-1", 'dof'
x = Get centroid element: 1
assert x = 0

c11 = Get value: 1, 1
assert c11=4.32 ; "c11=4.32" 'c11'

sf = Create simple Covariance: "sm", "2.52 1.90 10.06", "0 0",  32

removeObject: sm, sf