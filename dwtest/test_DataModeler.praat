# test_DataModeler.praat
# djmw 20161011, 20200326

appendInfoLine: "test_DataModeler.praat"


@testDataModelerInterface

@test_exponential
@test_exponential_plus_constant
@test_sigmoid
@test_sigmoid_plus_constant

appendInfoLine: "test_DataModeler.praat OK"

procedure testDataModelerInterface
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

procedure test_Jacquelin_page18
	appendInfoLine: tab$, tab$, "Test with data from Jacquelin, page 18"
	xk# = {-0.99,-0.945,-0.874,-0.859,-0.64,-0.573,-0.433, -0.042,-0.007,0.054,
	... 0.088, 0.222,0.401,0.465,0.633,0.637,0.735,0.762,0.791,0.981}
	yk# = {0.418,0.412,0.452,0.48,0.453,0.501,0.619,0.9,0.911,0.966,
	... 0.966,1.123,1.414,1.683,2.101,1.94,2.473,2.276,2.352,3.544}
	assert size (xk#) == size (yk#)
	parsFitJacquelin# = {0.313648, 0.574447, 1.716029}
	.table = Create Table with column names: "epc", 20, "x y"
	Formula: "x", ~ xk# [row]
	Formula: "y", ~ yk# [row]
	.dm = To DataModeler: -1, 1, "x", "y", "", "Exponential plus constant", 3	
	parsFit# = List parameter values
	for ipar to 3
		assert abs (parsFitJacquelin# [ipar] - parsFit# [ipar]) < 1e-6
	endfor
	removeObject: .table, .dm
	appendInfoLine: tab$, tab$, "Test with data from Jacquelin, page 18 OK"
endproc

procedure test_exponential
	appendInfoLine: tab$, "Test exponential"
	.table = Create Table with column names: "epc", 20, "x y"
	Formula: "x", ~ randomUniform (-1, 1)
	par# = {  0.6, 1.7 }
	Formula: "y", ~ par#[1] * exp (par# [2] * self ["x"])
	Sort rows: "x"
	.dm = To DataModeler: -1, 1, "x", "y", "", "Exponential", 3	
	rSquared = Get coefficient of determination
	assert rSquared > 0.999

	appendInfoLine: tab$, tab$, "Test exponential, parameter 1 fixed"
	Set parameter value: 1, 0.5, "Fixed"
	Fit model
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential, parameter 1 fixed OK"

	appendInfoLine: tab$, tab$, "Test exponential, parameter 2 fixed"
	Set parameter free: 1, 2
	Set parameter value: 2, 1.7, "Fixed"
	Fit model
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential, parameter 2 fixed OK"

	appendInfoLine: tab$, tab$, "Test exponential OK"
	removeObject: .dm, .table
endproc

procedure test_exponential_plus_constant
	appendInfoLine: tab$, "Test exponential plus constant"
	@test_Jacquelin_page18
	# example Jacquelin page 18 with own data
	appendInfoLine: tab$, tab$, "Test exponential plus constant, self"
	.table = Create Table with column names: "epc", 20, "x y"
	Formula: "x", ~ randomUniform (-1, 1)
	par# = { 0.3, 0.6, 1.7 }
	Formula: "y", ~ par# [1] + par#[2] * exp (par# [3] * self ["x"])
	Sort rows: "x"
	.dm = To DataModeler: -1, 1, "x", "y", "", "Exponential plus constant", 3	
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential plus constant, self OK"

	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 1 fixed"
	Set parameter value: 1, 0.3, "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 1 fixed OK"

	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 2 fixed"
	Set parameter free: 1, 3
	Set parameter value: 2, 0.6, "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 2 fixed OK"

	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 3, 1.7, "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 3 fixed OK"
	
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 1 & 2 fixed"
	Set parameter free: 1, 3
	Set parameter value: 1, 0.3, "Fixed"
	Set parameter value: 2, 0.6, "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 1 & 2 fixed OK"
	
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 1 & 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 1, 0.3, "Fixed"
	Set parameter value: 3, 1.7, "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 1 & 3 fixed OK"

	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 2 & 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 2, 0.6, "Fixed"
	Set parameter value: 3, 1.7, "Fixed"
	Fit model
	rSquared = Get coefficient of determination
	assert rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test exponential plus constant, parameter 2 & 3 fixed OK"

	selectObject: .table
	Formula: "y", ~ self + randomUniform (-0.1 * self, 0.1 * self)
	.dm2 = To DataModeler: -1, 1, "x", "y", "", "Exponential plus constant", 3	
	rSquared = Get coefficient of determination
	assert rSquared > 0.95
	removeObject: .table, .dm, .dm2
	appendInfoLine: tab$, tab$, "Test exponential plus constant, self OK"
	appendInfoLine: tab$, "Test exponential plus constant OK"
endproc

procedure test_sigmoid
	appendInfoLine: tab$, "Test sigmoid"
	appendInfoLine: tab$, tab$, "Test sigmoid, no noise"
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
	appendInfoLine: tab$, tab$, "Test sigmoid, no noise OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 1 fixed"
	Set parameter value: 1, 50, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 1 fixed OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 2 fixed"
	Set parameter free: 1, 3
	Set parameter value: 2, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 2 fixed OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 3, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 3 fixed OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 1 & 2 fixed"
	Set parameter free: 1, 3
	Set parameter value: 1, 50, "Fixed"
	Set parameter value: 2, 300, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 1 & 2 fixed OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 1 & 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 1, 50, "Fixed"
	Set parameter value: 3, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 1 & 3 fixed OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 2 & 3 fixed"
	Set parameter free: 1, 3
	Set parameter value: 2, 300, "Fixed"
	Set parameter value: 3, 200, "Fixed"
	Fit model
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.99
	appendInfoLine: tab$, tab$, "Test sigmoid, parameter 2 & 3 fixed OK"

	appendInfoLine: tab$, tab$, "Test sigmoid, max noise fraction 0.25"
	selectObject: .table
	Formula: "y", ~ self * (1 + 0.25*randomUniform (-1,1))
	.dm2 = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid", 3
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.85
	removeObject: .table, .dm, .dm2
	appendInfoLine: tab$, tab$, "Test sigmoid, max noise fraction 0.25 OK"
	appendInfoLine: tab$, "Test sigmoid OK"
endproc

procedure test_sigmoid_plus_constant
	appendInfoLine: tab$, "Test sigmoid plus constant"
	appendInfoLine: tab$, tab$, "Test sigmoid plus constant, no noise"
	.table = Create Table with column names: "sig", 500, "x y"
	.xxmin = -100
	.xxmax = 800
	.pars# = {10, 40, 300, 200} ; gamma, lambda, mu, sigma
	Formula: "x", ~ randomUniform (.xxmin, .xxmax)
	Formula: "y", ~ .pars# [1] + .pars# [2] / (1.0 + exp ( - (self["x"] - .pars# [3]) / .pars# [4]))
	Sort rows: "x"
	.dm = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid plus constant", 4
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.999
	appendInfoLine: tab$, tab$, "Test sigmoid plus constant, no noise OK"
	appendInfoLine: tab$, tab$, "Test sigmoid plus constant, max noise fraction 0.1"
	selectObject: .table
	Formula: "y", ~ self * (1 + 0.1*randomUniform (-1,1))
	.dm2 = To DataModeler: .xxmin, .xxmax, "x", "y", "", "Sigmoid plus constant", 4
	.rSquared = Get coefficient of determination
	assert .rSquared > 0.95
	removeObject: .table, .dm, .dm2	
	appendInfoLine: tab$, tab$, "Test sigmoid plus constant, max noise fraction 0.1 OK"
	appendInfoLine: tab$, "Test sigmoid plus constant OK"
endproc
