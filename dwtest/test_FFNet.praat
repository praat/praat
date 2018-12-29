# test_FFNet.praat
# djmw 20110512, 20180918

appendInfoLine: "test_FFNet.praat"

Create iris example... 0 0
ffnet = selected ("FFNet")
pattern = selected ("Pattern")
cat = selected ("Categories")

selectObject: ffnet
@testInterface: 0
@testInterface: 1
@testInterface: 2

selectObject: pattern, cat
tab = To TableOfReal
To Pattern and Categories: 0, 0, 0, 0
Remove
removeObject: tab

ntries = 10
selectObject: ffnet, pattern, cat
error1 = Get total costs... Minimum-squared-error
tab = Create Table with column names... results ntries costsb  costsa fc 

for i to ntries
	select ffnet
	Reset... 0.1
	plus pattern
	plus cat
	costsb = Get total costs... Minimum-squared-error
	Learn...  200 1e-7 Minimum-squared-error
	costsa = Get total costs... Minimum-squared-error
	select ffnet
	plus pattern
	cati = To Categories... Winner-takes-all
	plus cat
	fd = Get fraction different
	fc = 1 -fd
	select tab
	Set numeric value... i costsb costsb
	Set numeric value... i costsa costsa
	Set numeric value... i fc fc
	select cati
	Remove
endfor

select tab
mean = Get mean... fc
assert mean > 0.97
plus ffnet
plus pattern
plus cat
Remove

@test_openSave

appendInfoLine: "test_FFNet.praat OK"

procedure test_openSave
	.ffnet_read= Read from file: "iris_4-2-3-3.FFNet"
	Create iris example: 2, 3
	.ffnet = selected ("FFNet")
	.pattern = selected ("Pattern")
	.categories = selected ("Categories")

	selectObject: .ffnet
	Save as binary file: "kanweg.FFNet"
	.ffnet_read2 = Read from file: "kanweg.FFNet"
	# are they the same ??

	selectObject: .ffnet_read, .pattern, .categories
	.costs[1] = Get total costs: "Minimum-squared-error"
	selectObject: .ffnet_read, .pattern, .categories
	Learn: 100, 1e-7, "Minimum-squared-error"
	.costs[2] = Get total costs: "Minimum-squared-error"
	assert .costs[1] >= .costs[2]

	removeObject: 	.ffnet, .ffnet_read, .ffnet_read2, .pattern, .categories
endproc

procedure testInterface: .numberOfHiddenLayers
	.numberOfInputs_set = randomInteger (2, 5)
	.numberOfOutputs_set = randomInteger (2, 5)
	.name$ = string$ (.numberOfInputs_set)
	for .layer to 2
		.numberOfHiddenUnits_set [.layer] = 0
		.numberOfUnits = randomInteger (2, 10)
		if .layer <= .numberOfHiddenLayers
			.numberOfHiddenUnits_set [.layer] = .numberOfUnits
			.name$ = .name$ + "-" + string$ (.numberOfUnits)
		endif
	endfor
	.name$ = .name$ + "-" + string$ (.numberOfOutputs_set)
	.ffnet = Create FFNet: .name$, .numberOfInputs_set, .numberOfOutputs_set, .numberOfHiddenUnits_set [1], .numberOfHiddenUnits_set [2]
	
	appendInfoLine: tab$, "interface test for topology " + .name$
	appendInfoLine: tab$, tab$, "Query & Modify"

	.numberOfLayers = Get number of layers
	appendInfoLine: tab$, tab$, "number of layer(s): ",  .numberOfLayers
	.numberOfOutputs = Get number of outputs
	.numberOfUnitsInPreviousLayer = Get number of inputs
	for .ilayer to .numberOfLayers - 1
		.numberOfHiddenUnits = Get number of hidden units: .ilayer
		assert .numberOfHiddenUnits == .numberOfHiddenUnits_set [.ilayer]; "layer = "'.ilayer'
		for .iunit to .numberOfHiddenUnits
			.value_set = randomUniform (-0.1, 0.1)
			Set bias: .ilayer, .iunit, .value_set
			.value_get = Get bias: .ilayer, .iunit
			assert .value_get == .value_set; bias '.ilayer', '.iunit', '.value_set'
			for .punit to .numberOfUnitsInPreviousLayer
				.value_set = randomUniform (-0.1, 0.1)
				Set weight: .ilayer, .iunit, .punit, .value_set
				.value_get = Get weight: .ilayer, .iunit, .punit
				assert .value_get == .value_set; '.ilayer', '.iunit', '.punit', '.value_set'
			endfor
		endfor
		.numberOfUnitsInPreviousLayer = .numberOfHiddenUnits
		.numberOfHiddenWeights = Get number of hidden weights: .ilayer
		Select biases: .ilayer
		Select all weights
	endfor
	.numberOfOutputWeights = Get number of output weights
	for .iunit to .numberOfOutputs
		.outputCategory$ = Get category of output unit: .iunit
		.outputUnit = Get output unit of category: "xpiepeledokie"
		assert .outputUnit == 0; '.outputUnit'
	endfor
	.minimum = Get minimum

	appendInfoLine: tab$, tab$, "Extract"

	for .ilayer to .numberOfLayers - 1
		selectObject: .ffnet
		.tableOfReal = Extract weights: .ilayer
		removeObject: .tableOfReal
	endfor
	removeObject: .ffnet
endproc


