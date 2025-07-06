# test_DataModeler.praat
# djmw 20161011, 20200326, 20240923

appendInfoLine: "test_DataModeler.praat"

@testDataModelerInterface

@test_exponential
@test_Jacquelin_page18
@test_exponential_plus_constant
@test_sigmoid
@test_sigmoid_plus_constant
@test_certified_norris_data
@test_certified_pontius_data
@test_certified_filip_data
@test_certified_wampler5_data

appendInfoLine: "test_DataModeler.praat OK"

procedure testDataModelerInterface
	appendInfo: tab$, "Test interface:"
	# create a table for model y = 2x +3
	.xmin = -1
	.xmax = 3
	.nx = 100
	@createData: .xmin, .xmax, .nx, 0
	.table = selected ("Table")
	.maximumPolynomialOrder = 3
	.dm = To DataModeler: .xmin, .xmax, "x", "y", "sy", "Legendre polynomials", .maximumPolynomialOrder
	#
	# Query -
	#
	.numberOfParameters = Get number of parameters
	assert .numberOfParameters = .maximumPolynomialOrder + 1
	.numberOfFixedParameters = Get number of fixed parameters
	assert .numberOfFixedParameters = 0
	for .ipar to .numberOfParameters
		.value = Get parameter value: .ipar
		.stdev = Get parameter standard deviation: .ipar
		.status$ = Get parameter status: .ipar
		.status$ = mid$ (.status$, 1, 4)
		assert .status$ = "Free"
		Set parameter value: .ipar, .value, .status$
		.value2 = Get parameter value: .ipar
		assert .value = .value2
		for .ipar2 from .ipar to .numberOfParameters
			.var = Get variance of parameters: .ipar, .ipar2
		endfor
	endfor
	.valxmin = Get model value at x: .xmin
	.valxmax = Get model value at x: .xmax
	.numberOfDataPoints = Get number of data points
	.numberOfInvalidDataPoints = Get number of invalid data points
	for .idata to .numberOfDataPoints
		.statusdata$ = Get data point status: .idata
		assert mid$ (.statusdata$, 1, 5) = "Valid"

		.savex = Get data point x value: .idata
		.set = randomUniform (.xmin, .xmin)
		Set data point x value: .idata, .set
		.get = Get data point x value: .idata
		assert .set = .get; ['.idata'] '.set' = '.get'
		Set data point x value: .idata, .savex

		.savey = Get data point y value: .idata
		.set = randomUniform (.xmin, .xmin)
		Set data point y value: .idata, .set
		.get = Get data point y value: .idata
		assert .set = .get; ['.idata'] '.set' = '.get'
		Set data point y value: .idata, .savey

		.saves = Get data point y sigma: .idata
		.set = randomUniform (1, 10)
		Set data point y sigma: .idata, .set
		.get = Get data point y sigma: .idata
		assert .set = .get; ['.idata'] '.set' = '.get'
		Set data point y sigma: .idata, .saves
		
	endfor
	.sumOfSquares_res = Get residual sum of squares
	.dataStdDev = Get data standard deviation
	.coef = Get coefficient of determination
	.chiSqReport$ = Report chi squared
	.ndf = Get degrees of freedom
	#
	# Modify -
	#
	Set data weighing: "Equal"
	Set tolerance: 1e-5
	# Set parameter value: see query part
	Set parameter free: 0, 0
	#
	Fit model
	.covar = To Covariance (parameters)
	selectObject: .dm
	.zscores = To Table (z-scores)
	removeObject: .zscores, .covar, .table, .dm
	appendInfoLine: " OK"
endproc

procedure createData: .xmin, .xmax, .nx, .ynoise_stdev
	.model = Create Table with column names: "table", .nx, "x y sy"
	# linear
	.dx = (.xmax -.xmin) / (.nx - 1)
	for .row to .nx
		.x = .xmin + (.row -1) * .dx
		.y = 2 * .x + 3 + randomGauss (0, .ynoise_stdev)
		.sy = randomGauss (0.35, 0.05) 
		Set numeric value: .row, "x", .x
		Set numeric value: .row, "y", .y
		Set numeric value: .row, "sy", .sy
	endfor
endproc

procedure test_exponential
	appendInfoLine: tab$, "Test exponential"
	.table = Create Table with column names: "exponential", 20, "x y"
	Formula: "x", ~ randomUniform (-1, 1)
	par# = {  -0.6, 1.7 }
	Formula: "y", ~ par#[1] * exp (par# [2] * self ["x"])
	Sort rows: "x"
	.dm1 = To DataModeler: -1, 1, "x", "y", "", "Exponential", 3
	.dm1p1 = Get parameter value: 1
	.dm1p2 = Get parameter value: 2
	assert abs(.dm1p1 - par# [1]) < 1e-5	; '.p1'
	assert abs(.dm1p2 - par# [2]) < 1e-5	; '.p2'
	rSquared = Get coefficient of determination
	assert rSquared > 0.999

	appendInfo: tab$, tab$, "Test exponential, parameter 1 fixed:"
	Set parameter value: 1, 0.5, "Fixed"
	Fit model
	assert rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test exponential, parameter 2 fixed:"
	Set parameter free: 1, 2
	Set parameter value: 2, 1.7, "Fixed"
	Fit model
	assert rSquared > 0.999
	appendInfoLine: " OK"

	# 
	selectObject: .table
	Formula: "y", ~ -self
	.dm2 = To DataModeler: -1, 1, "x", "y", "", "Exponential", 3
	.dm2p1 = Get parameter value: 1
	.dm2p2 = Get parameter value: 2
	assert .dm2p1 = - .dm1p1
	assert .dm2p2 = .dm1p2
	appendInfoLine: tab$, "Test exponential OK"
	removeObject: .dm2, .dm1, .table
endproc

procedure test_Jacquelin_page18
	appendInfo: tab$, "Test a+b*exp(c*x) with data from Jacquelin, page 18:"
	xk# = {-0.99,-0.945,-0.874,-0.859,-0.64,-0.573,-0.433, -0.042,-0.007,0.054,
	... 0.088, 0.222,0.401,0.465,0.633,0.637,0.735,0.762,0.791,0.981}
	yk# = {0.418,0.412,0.452,0.48,0.453,0.501,0.619,0.9,0.911,0.966,
	... 0.966,1.123,1.414,1.683,2.101,1.94,2.473,2.276,2.352,3.544}
	assert size (xk#) == size (yk#)
	parsFitJacquelin# = {0.313648, 0.574447, 1.716029}
	.table = Create Table with column names: "epc_p18", 20, "x y"
	Formula: "x", ~ xk# [row]
	Formula: "y", ~ yk# [row]
	.dm = To DataModeler: -1, 1, "x", "y", "", "Exponential plus constant", 3	
	parsFit# = List parameter values
	for ipar to 3
		assert abs (parsFitJacquelin# [ipar] - parsFit# [ipar]) < 1e-6
	endfor
	removeObject: .table, .dm
	appendInfoLine: " OK"
endproc

procedure test_exponential_plus_constant
	appendInfoLine: tab$, "Test exponential plus constant"
	appendInfo: tab$, tab$, "Test exponential plus constant, self:"
	.table = Create Table with column names: "epc", 20, "x y"
	Formula: "x", ~ randomUniform (-1, 1)
	par# = { 0.3, 0.6, 3.7 }
	Formula: "y", ~ par# [1] + par#[2] * exp (par# [3] * self ["x"])
	Sort rows: "x"
	.dm = To DataModeler: -1, 1, "x", "y", "", "Exponential plus constant", 3	
	rSquared = Get coefficient of determination
	assert rSquared > 0.999   ; 'rSquared'
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test exponential plus constant, parameter 1 fixed:"
	Set parameter value: 1, par# [1], "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test exponential plus constant, parameter 2 fixed:"
	Set parameter free: 0, 0
	Set parameter value: 2, par# [2], "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.97   ; 'rSquared'
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test exponential plus constant, parameter 3 fixed:"
	Set parameter free: 0, 0
	Set parameter value: 3, par# [3], "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: " OK"
	
	appendInfo: tab$, tab$, "Test exponential plus constant, parameter 1 & 2 fixed:"
	Set parameter free: 0, 0
	Set parameter value: 1, par# [1], "Fixed"
	Set parameter value: 2, par# [2], "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: " OK"
	
	appendInfo: tab$, tab$, "Test exponential plus constant, parameter 1 & 3 fixed:"
	Set parameter free: 1, 3
	Set parameter value: 1, par# [1], "Fixed"
	Set parameter value: 3, par# [3], "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test exponential plus constant, parameter 2 & 3 fixed:"
	Set parameter free: 1, 3
	Set parameter value: 2, par# [2], "Fixed"
	Set parameter value: 3, par# [3], "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test exponential plus constant, noisy data, self:"
	selectObject: .table
	Formula: "y", ~ self + randomUniform (-0.1 * self, 0.1 * self)
	Rename: "epc_noise"
	.dm2 = To DataModeler: -1, 1, "x", "y", "", "Exponential plus constant", 3	
	rSquared = Get coefficient of determination
	assert rSquared > 0.95
	removeObject: .table, .dm, .dm2
	appendInfoLine: " OK"
	appendInfoLine: tab$, "Test exponential plus constant OK"
endproc

procedure test_sigmoid
	appendInfoLine: tab$, "Test sigmoid"
	appendInfo: tab$, tab$, "Test sigmoid, no noise:"
	# example Jacquelin page 39
	.table = Create Table with column names: "sig", 500, "x y"
	.xxmin = -100
	.xxmax = 800
	.pars# = {50, 300, 200} ; lambda, mu, sigma
	Formula: "x", ~ randomUniform (.xxmin, .xxmax)
	Formula: "y", ~ .pars# [1] / (1.0 + exp ( - (self["x"] - .pars# [2]) / .pars# [3]))
	Sort rows: "x"
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid", 3
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, parameter 1 fixed:"
	Set parameter value: 1, 50, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, parameter 2 fixed"
	Set parameter free: 1, 3
	Set parameter value: 2, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, parameter 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 3, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, parameter 1 & 2 fixed:"
	Set parameter free: 1, 3
	Set parameter value: 1, 50, "Fixed"
	Set parameter value: 2, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, parameter 1 & 3 fixed:"
	Set parameter free: 1, 3
	Set parameter value: 1, 50, "Fixed"
	Set parameter value: 3, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, parameter 2 & 3 fixed:"
	Set parameter free: 1, 3
	Set parameter value: 2, 300, "Fixed"
	Set parameter value: 3, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid, max noise fraction 0.25:"
	selectObject: .table
	Formula: "y", ~ self * (1 + 0.25*randomUniform (-1,1))
	.dm2 = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid", 3
	Rename: "sig_noisy"
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.85
	removeObject: .table, .dm, .dm2
	appendInfoLine: " OK"
	appendInfoLine: tab$, "Test sigmoid OK"
endproc

procedure test_sigmoid_plus_constant
	appendInfoLine: tab$, "Test sigmoid plus constant"
	appendInfo: tab$, tab$, "Test sigmoid plus constant, no noise:"
	.table = Create Table with column names: "sigpc", 500, "x y"
	.xxmin = -100
	.xxmax = 800
	.pars# = {10, 40, 300, 200} ; gamma, lambda, mu, sigma
	Formula: "x", ~ randomUniform (.xxmin, .xxmax)
	Formula: "y", ~ .pars# [1] + .pars# [2] / (1.0 + exp ( - (self["x"] - .pars# [3]) / .pars# [4]))
	Sort rows: "x"
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid plus constant", 4
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 1 fixed:"
	Set parameter value: 1, 10, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 2 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 2, 40, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 3 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 3, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 4 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 4, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"
	
	# combinations with parameter 1 fixed are always delegated to sigmoid and 
	# are tested in test_sigmoid. No need to repeat here.

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 1 & 2 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 1, 10, "Fixed"
	Set parameter value: 2, 40, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 1 & 3 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 1, 10, "Fixed"
	Set parameter value: 3, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 1 & 4 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 1, 10, "Fixed"
	Set parameter value: 4, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"
	
	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 2 & 3 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 2, 40, "Fixed"
	Set parameter value: 3, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 2 & 4 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 2, 40, "Fixed"
	Set parameter value: 4, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 3 & 4 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 3, 300, "Fixed"
	Set parameter value: 4, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, parameter 2, 3 & 4 fixed:"
	Set parameter free: 1, 4
	Set parameter value: 2, 40, "Fixed"
	Set parameter value: 3, 300, "Fixed"
	Set parameter value: 4, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: " OK"

	appendInfo: tab$, tab$, "Test sigmoid plus constant, max noise fraction 0.1:"
	selectObject: .table
	Formula: "y", ~ self * (1 + 0.1*randomUniform (-1,1))
	.dm2 = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid plus constant", 4
	Rename: "sigpc_noisy"
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.95
	removeObject: .table, .dm, .dm2	
	appendInfoLine: " OK"
	appendInfoLine: tab$, "Test sigmoid plus constant OK"
endproc

procedure test_certified_norris_data
	appendInfo: tab$, "Test line with Norris data"
	# https://itl.nist.gov/div898/strd/lls/data/LINKS/DATA/Norris.dat
	#
	# y = p[1] + p[2] * x
	#
	.yx## = {{0.1, 0.2},     {338.8, 337.4}, {118.1, 118.2}, {888.0, 884.6},
			...  {9.2, 10.1},    {228.1, 226.5}, {668.5, 666.3}, {998.5, 996.3},
			...  {449.1, 448.6}, {778.9, 777.0}, {559.2, 558.2}, {0.3, 0.4},
			...  {0.1, 0.6},     {778.1, 775.5}, {668.8, 666.9}, {339.3, 338.0},
			...  {448.9, 447.5}, {10.8, 11.6},   {557.7, 556.0}, {228.3, 228.1},
			...  {998.0, 995.8}, {888.8, 887.6}, {119.6, 120.2}, {0.3, 0.3},
			...  {0.6, 0.3},     {557.6, 556.8}, {339.3, 339.1}, {888.0, 887.2},
			...  {998.5, 999.0}, {778.9, 779.0}, {10.2, 11.1},   {117.6, 118.3},
			...  {228.9, 229.2}, {668.4, 669.1}, {449.2, 448.9}, {0.2, 0.5}}
	.table = Create Table with column names: "norris", 36, "x y"
	
	.pCertified# = {-0.262323073774029, 1.00211681802045}
	.rsquaredCertified = 0.999993745883712
	.residualStdevCertified = 0.884796396144373
	.pSigmaCertified# = {0.232818234301152,0.000429796848199937} 

	for .irow to 36
		Set numeric value: .irow, "y", .yx## [.irow, 1]
		Set numeric value: .irow, "x", .yx## [.irow, 2]
	endfor
	Sort rows: "x"
	.xxmin = 0
	.xxmax = 1000
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Standard polynomials", 1

	for .ipar to 2
		.pari = Get parameter value: .ipar
		assert abs(.pari - .pCertified# [.ipar]) < 1e-11
		.pariSigma = Get parameter standard deviation: .ipar
		assert abs (.pariSigma - .pSigmaCertified# [.ipar]) < 1e-11
	endfor
	.rSquared = Get coefficient of determination
	assert abs(.rSquared - .rsquaredCertified) < 1e-13
	.residualStdev = Get residual standard deviation
	assert abs(.residualStdev - .residualStdevCertified) < 1e-13
	removeObject: .table, .dm
	appendInfoLine: " OK"
endproc

procedure test_certified_pontius_data
	appendInfo: tab$, "Test quadratic with Pontius data"
	# https://www.itl.nist.gov/div898/strd/lls/data/LINKS/DATA/Pontius.dat
	#
	# y = p[1] + p[2] * x + p[3]* x^2
	# extreme data:
	# the condition number, d[1]/d[3], of the SVD of the design matrix is 14229935256378.453
	#
	.yx## ={{0.11019, 150000},  {0.21956, 300000},  {0.32949, 450000},  {0.43899, 600000},
			... {0.54803, 750000},  {0.65694, 900000},  {0.76562, 1050000}, {0.87487, 1200000},
			... {0.98292, 1350000}, {1.09146, 1500000}, {1.20001, 1650000}, {1.30822, 1800000},
			... {1.41599, 1950000}, {1.52399, 2100000}, {1.63194, 2250000}, {1.73947, 2400000},
			... {1.84646, 2550000}, {1.95392, 2700000}, {2.06128, 2850000}, {2.16844, 3000000},
			... {0.11052, 150000},  {0.22018, 300000},  {0.32939, 450000},  {0.43886, 600000},
			... {0.54798, 750000},  {0.65739, 900000},  {0.76596, 1050000}, {0.87474, 1200000},
			... {0.98300, 1350000}, {1.09150, 1500000}, {1.20004, 1650000}, {1.30818, 1800000},
			... {1.41613, 1950000}, {1.52408, 2100000}, {1.63159, 2250000}, {1.73965, 2400000}, 
			... {1.84696, 2550000}, {1.95445, 2700000}, {2.06177, 2850000}, {2.16829, 3000000}}
	.pCertified# = {0.673565789473684e-3, 0.732059160401003e-6, -0.316081871345029e-14}
	.rsquaredCertified =  0.999999900178537
	.residualStdevCertified = 0.205177424076185e-03
	.pSigmaCertified# = {0.107938612033077e-3, 0.157817399981659e-9, 0.486652849992036e-16} 

	.table = Create Table with column names: "pontius", 40, "x y"
	for .irow to 40
		Set numeric value: .irow, "y", .yx## [.irow, 1]
		Set numeric value: .irow, "x", .yx## [.irow, 2]
	endfor
	Sort rows: "x"
	.xxmin = 150000
	.xxmax = 3e6
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Standard polynomials", 2
	for .ipar to 2
		.pari = Get parameter value: .ipar
		assert abs(.pari - .pCertified# [.ipar]) < 1e-7
		.pariSigma = Get parameter standard deviation: .ipar
		assert abs (.pariSigma - .pSigmaCertified# [.ipar]) < 1e-7
	endfor
	.rSquared = Get coefficient of determination
	assert abs(.rSquared - .rsquaredCertified) < 1e-12
	.residualStdev = Get residual standard deviation
	assert abs(.residualStdev - .residualStdevCertified) < 1e-9
	removeObject: .table, .dm
	appendInfoLine: " OK"
endproc

procedure test_certified_filip_data
	appendInfo: tab$, "Test 10th order polynome with Filippelli data"
	# https://www.itl.nist.gov/div898/strd/lls/data/LINKS/DATA/Filip.dat
	#
	# y = p[1] + p[2] * x + p[3]* x^2 + ... p[11] * x^10
	# extreme data:
	# the condition number, d[1]/d[11], of the SVD of the design matrix is 1767966769500694.8
	#
	# 
	.yx## ={{0.8116, -6.860120914}, {0.9072, -4.324130045}, {0.9052, -4.358625055}, {0.9039, -4.358426747},
			... {0.8053, -6.955852379}, {0.8377, -6.661145254}, {0.8667, -6.355462942}, {0.8809, -6.118102026},
			... {0.7975, -7.115148017}, {0.8162, -6.815308569}, {0.8515, -6.519993057}, {0.8766, -6.204119983},
			... {0.8885, -5.853871964}, {0.8859, -6.109523091}, {0.8959, -5.79832982},  {0.8913, -5.482672118},
			... {0.8959, -5.171791386}, {0.8971, -4.851705903}, {0.9021, -4.517126416}, {0.909,  -4.143573228},
			... {0.9139, -3.709075441}, {0.9199, -3.499489089}, {0.8692, -6.300769497}, {0.8872, -5.953504836},
			... {0.89,   -5.642065153}, {0.891,  -5.031376979}, {0.8977, -4.680685696}, {0.9035, -4.329846955},
			... {0.9078, -3.928486195}, {0.7675, -8.56735134},  {0.7705, -8.363211311}, {0.7713, -8.107682739},
			... {0.7736, -7.823908741}, {0.7775, -7.522878745}, {0.7841, -7.218819279}, {0.7971, -6.920818754},
			... {0.8329, -6.628932138}, {0.8641, -6.323946875}, {0.8804, -5.991399828}, {0.7668, -8.781464495},
			... {0.7633, -8.663140179}, {0.7678, -8.473531488}, {0.7697, -8.247337057}, {0.77,   -7.971428747},
			... {0.7749, -7.676129393}, {0.7796, -7.352812702}, {0.7897, -7.072065318}, {0.8131, -6.774174009},
			... {0.8498, -6.478861916}, {0.8741, -6.159517513}, {0.8061, -6.835647144}, {0.846,  -6.53165267},
			... {0.8751, -6.224098421}, {0.8856, -5.910094889}, {0.8919, -5.598599459}, {0.8934, -5.290645224},
			... {0.894,  -4.974284616}, {0.8957, -4.64454848},  {0.9047, -4.290560426}, {0.9129, -3.885055584},
			... {0.9209, -3.408378962}, {0.9219, -3.13200249},  {0.7739, -8.726767166}, {0.7681, -8.66695597},
			... {0.7665, -8.511026475}, {0.7703, -8.165388579}, {0.7702, -7.886056648}, {0.7761, -7.588043762},
			... {0.7809, -7.283412422}, {0.7961, -6.995678626}, {0.8253, -6.691862621}, {0.8602, -6.392544977},
			... {0.8809, -6.067374056}, {0.8301, -6.684029655}, {0.8664, -6.378719832}, {0.8834, -6.065855188},
			... {0.8898, -5.752272167}, {0.8964, -5.132414673}, {0.8963, -4.811352704}, {0.9074, -4.098269308},
			... {0.9119, -3.66174277},  {0.9228, -3.2644011}}
	.pCertified# = {-1467.48961422980,     -2772.17959193342,     -2316.37108160893,   -1127.97394098372, 
							... -354.478233703349,     -75.1242017393757,     -10.8753180355343,   -1.06221498588947, 
							... -0.670191154593408e-1, -0.246781078275479e-2, -0.402962525080404e-4}
	.rsquaredCertified =  0.996727416185620
	.residualStdevCertified = 0.334801051324544e-2
	.pSigmaCertified# = {298.084530995537, 559.779865474950, 466.477572127796, 227.204274477751, 
									... 71.6478660875927, 15.2897178747400, 2.23691159816033, 0.221624321934227,
									... 0.142363763154724e-1, 0.535617408889821e-3, 0.896632837373868e-5} 
	.table = Create Table with column names: "filip", 82, "x y"
	for .irow to 82
		Set numeric value: .irow, "y", .yx## [.irow, 1]
		Set numeric value: .irow, "x", .yx## [.irow, 2]
	endfor
	Sort rows: "x"
	.xxmin = -9
	.xxmax = -3
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Standard polynomials", 10
	Set tolerance: 1e-30
	Fit model
	for .ipar to 11
		.pari = Get parameter value: .ipar
		assert abs ((.pari - .pCertified# [.ipar]) / .pCertified# [.ipar]) < 1e-4; for parameter '.ipar'
		.pariSigma = Get parameter standard deviation: .ipar
		assert abs ((.pariSigma - .pSigmaCertified# [.ipar]) / .pSigmaCertified# [.ipar]) < 0.01; for parameter '.ipar'
	endfor
	.rSquared = Get coefficient of determination
	assert abs(.rSquared - .rsquaredCertified) < 1e-4
	.residualStdev = Get residual standard deviation
	assert abs ((.residualStdev - .residualStdevCertified) /.residualStdevCertified)  < 5e-3
	removeObject: .table, .dm
	appendInfoLine: " OK"
endproc

procedure test_certified_wampler5_data
	appendInfo: tab$, "Test 5th order polynome with Wampler5 data"
	# https://www.itl.nist.gov/div898/strd/lls/data/LINKS/DATA/Wampler5.dat
	#
	# y = p[1] + p[2] * x + p[3]* x^2 + ... p[6] * x^5
	# extreme data:
	# the condition number, d[1]/d[6], of the SVD of the design matrix is 6398930.0537865367
	#
	# 
	.yx## = {{ 7590001, 0},  {-20479994, 1},  { 20480063, 2},  {-20479636, 3},
			... { 25231365, 4},  {-20476094, 5},  { 20489331, 6},  {-20460392, 7},
			... { 18417449, 8},  {-20413570, 9},  { 20591111, 10}, {-20302844, 11},
			... { 18651453, 12}, {-20077766, 13}, { 21059195, 14}, {-19666384, 15},
			... { 26348481, 16}, {-18971402, 17}, { 22480719, 18}, {-17866340, 19},
			... { 10958421, 20}}
	.pCertified# = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}
	.rsquaredCertified =  0.224668921574940e-2
	.residualStdevCertified = 23601450.2379268
	.pSigmaCertified# = {21523262.4678170, 23635517.3469681, 7793435.24331583,
									... 1014755.07550350, 56456.6512170752, 1123.24854679312} 

	.table = Create Table with column names: "wampler5", 21, "x y"
	for .irow to 21
		Set numeric value: .irow, "y", .yx## [.irow, 1]
		Set numeric value: .irow, "x", .yx## [.irow, 2]
	endfor
	Sort rows: "x"
	.xxmin = 0
	.xxmax = 20
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Standard polynomials", 5
	for .ipar to 6
		.pari = Get parameter value: .ipar
		assert abs ((.pari - .pCertified# [.ipar]) / .pCertified# [.ipar]) < 1e-5; for parameter '.ipar'
		.pariSigma = Get parameter standard deviation: .ipar
		assert abs ((.pariSigma - .pSigmaCertified# [.ipar]) / .pSigmaCertified# [.ipar]) < 1e-9; for parameter '.ipar'
	endfor
	.rSquared = Get coefficient of determination
	assert abs(.rSquared - .rsquaredCertified) < 1e-12
	.residualStdev = Get residual standard deviation
	assert abs ((.residualStdev - .residualStdevCertified) /.residualStdevCertified)  < 1e-12
	removeObject: .table, .dm
	appendInfoLine: " OK"
endproc

