# test_Covariance.praat
# djmw 2015, 2020

appendInfoLine: "test_Covariance"
appendInfoLine: tab$, "Morrison_example_7.3"
@test_Morrison_example_7_3
appendInfoLine: tab$, "Morrison_example_3.5"
@test_Morrison_example_3_5
appendInfoLine: tab$, "Morrison_example_4.3"
@test_Morrison_example_4_3
appendInfoLine: "test_Covariance OK"

procedure test_Morrison_example_4_3
	.covariances$ = "11.264 9.4060 7.1550 3.3791 13.5265 7.3784 2.5014 11.5796 2.6167 5.8133"
	.means1$ = "12.57 9.57 11.49 7.97"
	.means2$ = "8.75 5.33 8.50 4.75"
	.covariance1 = Create simple Covariance: "4_3_1", .covariances$, .means1$, 37
	.covariance2 = Create simple Covariance: "4_3_2", .covariances$, .means2$, 12
	selectObject: .covariance1, .covariance2
	.report$ =  Report multivariate mean difference: "yes"
	.fisher = extractNumber (.report$, "Fisher's F:")
	assert abs (.fisher - 5.18) < 0.005
	.p = extractNumber (.report$, "Significance from zero:")
	assert .p < 0.005
	.numberOfObservations1 = extractNumber (.report$, "Number of observations 1:")
	assert .numberOfObservations1 = 37
	.numberOfObservations2 = extractNumber (.report$, "Number of observations 2:")
	assert .numberOfObservations2 = 12
	.dof1 = 	extractNumber (.report$, "Degrees of freedom 1:")
	assert .dof1 == 4
	.dof2 = 	extractNumber (.report$, "Degrees of freedom 2:")
	assert .dof2 = 44
	removeObject: .covariance1, .covariance2
endproc

procedure test_Morrison_example_7_3
	.p = 2
	.k = 2
	.sm = Create simple Covariance: "sm", "4.32 1.88 9.18", "0 0",  32
	.numberOfColumns = Get number of columns
	.numberOfRows = Get number of rows
	assert .numberOfColumns = .p and .numberOfRows = .p; "nrows = ncols = 2"
	.numberOfObservations = Get number of observations
	assert .numberOfObservations = 32; "numberOfObservations" 'no' <> 32
	.dof = Get degrees of freedom
	assert .dof = .numberOfObservations - 1 ; "dof = no-1", 'dof'
	.x = Get centroid element: 1
	assert .x = 0
	.c11 = Get value: 1, 1
	.c22 = Get value: 2, 2
	assert .c11 = 4.32
	assert .c22 = 9.18
	.ln_det_sm = Get ln(determinant)
	.det_sm = exp (.ln_det_sm)
	.det_sm$ = fixed$ (.det_sm, 3)
	assert .det_sm$ = "36.123"

	.sf = Create simple Covariance: "sf", "2.52 1.90 10.06", "0 0",  32
	.ln_det_sf = Get ln(determinant)
	.det_sf = exp (.ln_det_sf)
	.det_sf$ = fixed$ (.det_sf, 3)
	assert .det_sf$ = "21.741"
	selectObject: .sm, .sf
	.report$ = Report equality of covariances
	.chisq = extractNumber (.report$, "Chi-squared (bartlett) = ")
	.chisq$ = fixed$ (.chisq, 2)
	assert .chisq$ = "2.72" 
	.cInv = 1 - (2*.p^2+3*.p-1)/(6*(.p+1)*(.k-1))*(1/31+1/31-1/62)
	appendInfoLine: tab$, "C^-1 = ", .cInv
	removeObject: .sm, .sf
endproc

procedure test_Morrison_example_3_5
	.cor = Create simple Correlation: "correlation", "1.0 0.69 0.43 1.0 0.3 1.0", "0.0 0.0 0.0", 54
	.report$ = Get diagonality (bartlett): 1
 	.chisq = extractNumber (.report$, "chisq = ")
	.chisq$ = fixed$ (.chisq, 2)
	assert .chisq$ = "43.54"
	.ln_det_r = Get ln(determinant)
	.det_r = exp (.ln_det_r)
	.det_r$ = fixed$ (.det_r, 3)
	assert .det_r$ = "0.427"
	removeObject: .cor
endproc

;Difference
