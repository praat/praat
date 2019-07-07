# test_GaussianMixture.praat
# djmw 20190707

appendInfoLine: "test_GaussianMixture.praat"
appendInfoLine: tab$, "Interface"
pols = Create TableOfReal (Pols 1973): "no"
gm = To GaussianMixture (row labels): "Complete"
numberOfComponents = Get number of components
assert numberOfComponents == 12
dimension = Get dimension of component
assert dimension == 3
tor = To TableOfReal (random sampling): 100
nrow = Get number of rows
assert nrow == 100
removeObject: tor
selectObject: gm
within = To Covariance (within)
selectObject: gm
between= To Covariance (between)
selectObject: gm
total= To Covariance (total)
selectObject: gm
pca= To PCA
removeObject: within, between, total, pca

appendInfoLine: tab$, "Drawing"
selectObject: gm
Erase all
Select outer viewport: 0, 3, 0, 3
Draw concentration ellipses: 1.0, "no", 1, 2, 0, 0, 0, 0, 12, "yes"
Select outer viewport: 3, 6, 0, 3
Draw concentration ellipses: 1.0, "yes", 1, 2, 0, 0, 0, 0, 12, "yes"
Select outer viewport: 0, 3, 3, 6
Draw marginal pdf: 1, 0, 0, 0, 0, 500, 0, "yes"
Select outer viewport: 3, 6, 3, 6
Draw marginal pdf: 2, 0, 0, 0, 0, 500, 0, "yes"

appendInfoLine: tab$, "GaussinaMixture and TableOfReal"

selectObject: pols, gm
p = Get likelihood value: "Likelihood"
cemm = To GaussianMixture (CEMM): 1, 0.001, 200, 0.001, "MessageLength"
selectObject: pols, gm
ct = To ClassificationTable
selectObject: pols, gm
correlation = To Correlation
selectObject: pols, gm
tor = To TableOfReal (BHEP normality tests): 1.41
removeObject:  tor, correlation, ct, cemm


removeObject: pols, gm

appendInfoLine: "test_GaussianMixture.praat OK"





