# henzeZirklerMultivariateNormalityTest.praat
# djmw 20090707

printline henzeZirklerMultivariateNormalityTest : start
ir = Create iris data set
ir50 = Extract row ranges...  1:50
report$ = Report multivariate normality... 0

stat = extractNumber (report$, "statistic:")
assert stat = 0.948845227555147
prob = extractNumber (report$, " normality:")
assert prob = 0.04995362583969921
lmean = extractNumber (report$, " mean:")
assert lmean = -0.27940830765481817
lvariance = extractNumber (report$, " variance:")
assert lvariance = 0.13790691675095398
beta = extractNumber (report$, " beta:")
assert beta = 1.276083424420824
sampleSize = extractNumber (report$, " size:")
assert sampleSize = 50
nvars = extractNumber (report$, " variables:")
assert nvars = 4

select ir
plus ir50
Remove

printline henzeZirklerMultivariateNormalityTest : end
