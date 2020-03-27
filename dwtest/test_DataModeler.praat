# test_DataModeler.praat
# djmw 20161011, 20200326

appendInfoLine: "test_DataModeler.praat"


@testDataModelerInterface

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

