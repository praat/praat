# test_MDS.praat

appendInfoLine: "test_MDS.praat"
@test_additiveConstant
@testDissimilarityInterface
# side effect: 6 configurations in the list of objects: configuration[1]...configuration[6]
# testINDSCAL uses these 6 configurations
@testINDSCAL
@testProcrustus

for i to 6
	removeObject: configuration [i]
endfor

appendInfoLine: "test_MDS.praat OK"

procedure testProcrustus
	appendInfoLine: tab$,  tab$, "Configuration & Configuration"
	for .i from 2 to  6
		selectObject: configuration [1]
		plusObject: configuration [.i]
		.procrustus [1] = To Procrustes: "no"
		plusObject: configuration [.i]
		.ct [.i] = To Configuration
		plusObject: configuration [1]
		.procrustus [2] = To Procrustes... no
		@check_if_identity_transform: .procrustus [2]
		removeObject: .procrustus [1], .procrustus [2]
	endfor
	for .i from 2 to 6
		removeObject: .ct [.i]
	endfor
endproc

procedure check_if_identity_transform:  .p
	selectObject: .p
	.scale = Get scale
	assert .scale > 1 - 1e-4
	for .j to 2
		.tj = Get translation element: .j
		assert abs(.tj) < 1e-6
		for .k to 2
			.tjk = Get transformation element: .j, .k
			if .j = .k
				assert .tjk > 1 - 1e-4
			else
				assert abs(.tjk) < 1e-6
			endif
		endfor
	endfor
endproc

procedure testDissimilarityInterface
	appendInfoLine: tab$, "test interface"
	appendInfoLine: tab$, tab$, "Query"
	.dissimilarity = Create letter R example: 0
	.numberOfRows = Get number of rows
	.numberOfColumns = Get number of columns
	for .irow to .numberOfRows
		.rowLabel$ = Get row label: .irow
		.rowIndex = Get row index: .rowLabel$
		assert .irow = .rowIndex; '.irow' '.rowIndex'
	endfor
	for .icol to .numberOfColumns
		.columnLabel$ = Get column label: .icol
		.columnIndex = Get column index: .columnLabel$
		assert .icol = .columnIndex; '.icol' '.columnIndex'
	endfor
	for .irow to .numberOfRows
		for .icol to .numberOfColumns
			val = Get value: .irow, .icol
		endfor
	endfor
	.norm = Get table norm
	.additiveConstant = Get additive constant

	appendInfoLine: tab$, tab$, "Modify: skipped"
	appendInfoLine: tab$, tab$, "Synthesize: skipped"

	appendInfoLine: tab$, tab$, "Extract part"
	selectObject: .dissimilarity
	.tmp1 = Extract row ranges: "1 2"
	.numberOfRows1 = Get number of rows
	assert .numberOfRows1 == 2; '.numberOfRows1' "= 2"
	selectObject: .dissimilarity
	.tmp2 = Extract rows where: "1"
	.numberOfRows2 = Get number of rows
	.numberOfColumns2 = Get number of columns
	assert .numberOfRows2 == .numberOfRows; '.numberOfRows2' "==" '.numberOfRows'
	assert .numberOfColumns2 == .numberOfColumns; '.numberOfColumns2' "==" '.numberOfColumns'
	selectObject: .dissimilarity
	.tmp3 = Extract column ranges: "1 2"
	.numberOfColumns3 = Get number of columns
	assert .numberOfColumns3 == 2; '.numberOfColumns3' "= 2"
	selectObject: .dissimilarity
	.tmp4 = Extract columns where: "1"
	.numberOfRows4 = Get number of rows
	.numberOfColumns4 = Get number of columns
	assert .numberOfRows4 == .numberOfRows; '.numberOfRows4' "==" '.numberOfRows'
	assert .numberOfColumns4 == .numberOfColumns; '.numberOfColumns4' "==" '.numberOfColumns'
	removeObject: .tmp1, .tmp2, .tmp3, .tmp4
	for .irow to .numberOfRows
		selectObject: .dissimilarity
		.rowLabel$ = Get row label: .irow
		.tmpi = Extract rows where label: "is equal to", .rowLabel$
		.numberOfRows5 = Get number of rows
		assert .numberOfRows5 >= 1
		removeObject: .tmpi
	endfor
	for .icol to .numberOfColumns
		selectObject: .dissimilarity
		.columnLabel$ = Get column label: .icol
		.tmpi = Extract columns where label: "is equal to", .columnLabel$
		.numberOfColumns6 = Get number of columns
		assert .numberOfColumns6 >= 1
		removeObject: .tmpi
	endfor

	appendInfoLine: tab$, tab$, "Extract"
	selectObject: .dissimilarity
	.strings1 = Extract row labels as Strings
	.numberOfStrings = Get number of strings
	assert .numberOfStrings == .numberOfRows
	selectObject: .dissimilarity
	.strings2 = Extract column labels as Strings
	.numberOfStrings = Get number of strings
	assert .numberOfStrings == .numberOfColumns
	removeObject: .strings1, .strings2

	appendInfoLine: tab$, tab$, "Convert"
	selectObject: .dissimilarity
	.table = To Table: "col1"
	.numberOfColumnsT = Get number of columns
	assert .numberOfColumnsT = .numberOfColumns + 1
	selectObject: .dissimilarity
	.matrix = To Matrix
	.numberOfRowsM = Get number of rows
	.numberOfColumnsM = Get number of columns
	assert .numberOfRowsM == .numberOfRows
	assert .numberOfColumnsM == .numberOfColumns
	selectObject: .dissimilarity
	.tableOfReal = To TableOfReal
	.numberOfRowsT = Get number of rows
	.numberOfColumnsT = Get number of columns
	assert .numberOfRowsT == .numberOfRows
	assert .numberOfColumnsT == .numberOfColumns
	removeObject: .table, .matrix, .tableOfReal

	appendInfoLine: tab$, tab$, "To Configuration"
	selectObject: .dissimilarity
	for .ipar to 6
		.numberOfDimensions$ [.ipar] = "2, "
	endfor
	.numberOfDimensions$[6] = "2, 2, "
	.minimizationParameters$ = "1e-05, 10, 1"
	.mdsCommand$ [1] = "To Configuration (monotone mds): "
	.extraParameters$ [1] = """Primary approach"", "
	.mdsCommand$ [2] = "To Configuration (i-spline mds): "
	.extraParameters$ [2] = "1, 1, "
	.mdsCommand$ [3] = "To Configuration (interval mds): "
	.extraParameters$ [3] = ""
	.mdsCommand$ [4] = "To Configuration (ratio mds): "
	.extraParameters$ [4] = ""
	.mdsCommand$ [5] = "To Configuration (absolute mds): "
	.extraParameters$ [5] = ""
	.mdsCommand$ [6] = "To Configuration (kruskal): "
	.extraParameters$ [6] = """Primary approach"", ""Kruskal's stress-1"", "
	
	# Create a random configuration
	.command$ = .mdsCommand$ [1] + .numberOfDimensions$ [1] + .extraParameters$ [1] + .minimizationParameters$
	.randomConfiguration = '.command$'
	Formula: "randomUniform (-1, 1)"
	Rename: "random"

	# Use the 6 different "To Configuration (..)" commands to get 6 configurations

	for .itype to 6
		selectObject: .dissimilarity
		.command$ = .mdsCommand$ [.itype] + .numberOfDimensions$ [.itype] + .extraParameters$ [.itype] + .minimizationParameters$
		configuration [.itype] = '.command$'
	endfor

	# Use the dissimilarity and the configuration and try to improve the configuration 

	appendInfoLine: tab$, tab$, "Dissimilarity & Configuration"
	.minimizationParameters$ = "1e-08, 50, 1"
	for .itype to 6
		selectObject: .dissimilarity, configuration [.itype]
		.command$ = .mdsCommand$ [.itype] + .extraParameters$ [.itype] + .minimizationParameters$
		.configuration [.itype] = '.command$'
	endfor

	.stressMeasure$ [1] = "Normalized"
	.stressMeasure$ [2] = "Kruskal's stress-1"
	.stressMeasure$ [3] = "Kruskal's stress-2"
	.stressMeasure$ [4] = "Raw"
	.tiesHandling$ [1] = "Primary approach"
	.tiesHandling$ [2] = "Secondary approach"
	;.stressCalculation$ [1] = "Formula1"
	;.stressCalculation$ [2] = "Formula2"
	.stressCalculation$ [1] = "Kruskal's stress-1"
	.stressCalculation$ [2] = "Kruskal's stress-1"

	# test kruskal's stress-1 and stress-2

	for .ities to 2
		selectObject: .dissimilarity, .randomConfiguration	
		.stress1_random = Get stress (monotone mds): .tiesHandling$ [.ities], .stressMeasure$ [2]
		.stress2_random = Get stress (monotone mds): .tiesHandling$ [.ities], .stressMeasure$ [3]
		assert .stress1_random <= .stress2_random; '.stress1_random' <= '.stress2_random' ? random
		for .i to 6
			selectObject: .dissimilarity, .configuration [.i]
			.stress1 = Get stress (monotone mds): .tiesHandling$ [.ities], .stressMeasure$ [2]
			.stress2 = Get stress (monotone mds): .tiesHandling$ [.ities], .stressMeasure$ [3]
			assert .stress1 <= .stress1_random; '.stress1' <= '.stress1_random' ? '.ities' conf['.i']
			assert .stress2 <= .stress2_random; '.stress2' <= '.stress2_random' ? '.ities' conf['.i']
			assert .stress1 <= .stress2; '.stress1' <= '.stress2' ? '.ities' conf['.i']
		endfor
	endfor
if 0
	for .k to 4
		selectObject: .dissimilarity, .randomConfiguration
		.stress0 = Get stress (i-spline mds): 1, 3, .stressMeasure$ [.k]
		selectObject: .dissimilarity, configuration [2]
		.stress1 = Get stress (i-spline mds): 1, 3, .stressMeasure$ [.k]
		assert .stress1 <= .stress0
		selectObject: .dissimilarity, .configuration [2]
		.stress2 = Get stress (i-spline mds): 1, 3, .stressMeasure$ [.k]
		assert .stress2 <= .stress1; '.stress2' '.stress1' '.k'
	endfor
	for .k from 1 to 4
		selectObject: .dissimilarity, .randomConfiguration
		.stress10 = Get stress (interval mds): .stressMeasure$ [.k]
		selectObject: .dissimilarity, configuration [3]
		.stress11 = Get stress (interval mds): .stressMeasure$ [.k]
		assert .stress11 <= .stress10 ; '.k'
		selectObject: .dissimilarity, .configuration [3]
		.stress12 = Get stress (interval mds): .stressMeasure$ [.k]
		assert .stress12 <= .stress11 ; '.k'
		selectObject: .dissimilarity, .randomConfiguration
		.stress20 = Get stress (ratio mds): .stressMeasure$ [.k]
		selectObject: .dissimilarity, configuration [4]
		.stress21 = Get stress (ratio mds): .stressMeasure$ [.k]
		assert .stress21 <= .stress20 ; '.k'
		selectObject: .dissimilarity, .configuration [4]
		.stress22 = Get stress (ratio mds): .stressMeasure$ [.k]
		assert .stress22 <= .stress21 ; '.k' '.stress22' < '.stress21' ?
		selectObject: .dissimilarity, .randomConfiguration
		.stress30 = Get stress (absolute mds): .stressMeasure$ [.k]
		selectObject: .dissimilarity, configuration [5]
		.stress31 = Get stress (absolute mds): .stressMeasure$ [.k]
		assert .stress31 <= .stress30 ; '.k'
		selectObject: .dissimilarity, .configuration [5]
		.stress32 = Get stress (absolute mds): .stressMeasure$ [.k]
		assert .stress32 <= .stress31 ; '.k'
	endfor
endif

	for .itype to 6
		removeObject: .configuration [.itype]
	endfor

	removeObject: .dissimilarity, .randomConfiguration
endproc

procedure dissimilarity_to_Configurations: .dissimilarity

endproc

procedure testINDSCAL
	for .i  to 6
		selectObject: configuration [.i]
		.distance [.i] = To Distance
	endfor

	selectObject: .distance [1]
	for .i from 2 to 6
		plusObject: .distance [.i]
	endfor

	To Configuration (indscal): 2, "no", 1e-5, 10, 1, "yes", "no"
	.configuration = selected ("Configuration")
	.salience = selected ("Salience")

	# test old interface
	;To Configuration (indscal): "no", 1e-5, 10
	;.configuration2 = selected ("Configuration")
	;.salience2 = selected ("Salience")
	;removeObject: .configuration2, .salience2

	for .i from 1 to 6
		removeObject: .distance[.i]
	endfor

	removeObject: .configuration, .salience
endproc

procedure test_additiveConstant
	# create table 18.1 Borg & Groenen (1997): Modern MDS
	# Check with top of table 18.3 where a value of 1.291 is given

	.distance = Create TableOfReal: "18.1", 4, 4
	.row1# = {0, pi, pi/4, pi/2}
	.row2# = {pi, 0, 3*pi/4, pi/2}
	.row3# = {pi/4, 3*pi/4, 0, 3*pi/4}
	.row4# = {pi/2, pi/2, 3*pi/4, 0}
	for .icol to 4
		for .irow to 4
		Set value: .irow, .icol, .row'.irow'# [.icol]
		endfor
	endfor
	.dissimilarity = To Dissimilarity
	.additiveConstant = Get additive constant
	.additiveConstant_rounded = number (fixed$ (.additiveConstant, 3))
	assert .additiveConstant_rounded  = 1.291; '.additiveConstant_rounded'
	removeObject: .dissimilarity, .distance
endproc


