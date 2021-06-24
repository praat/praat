# test_MDS.praat

appendInfoLine: "test_MDS.praat"

@test_additiveConstant

@testLetterRExample

@testDissimilarityInterface

@testDissimilarityToConfiguration: 5

@testCarrolWishExample

@testProcrustes

appendInfoLine: "test_MDS.praat OK"

procedure testLetterRExample
	appendInfoLine: tab$, "test Dissimilarity letter R"
	.dissimilarity = Create letter R example: 0
	.numberOfRows = Get number of rows
	.numberOfColumns = Get number of columns
	assert .numberOfRows = .numberOfColumns
	assert .numberOfRows = 32
	
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

	# check some dissimilarity values
	assert object[.dissimilarity, 1, 1] = 0
	assert abs(object[.dissimilarity, 1, 2] - 6) < 1e-10
	assert abs(object[.dissimilarity, 1, 3] - 9) < 1e-10
	assert abs(object[.dissimilarity, 12, 10] - 7) < 1e-10
	assert abs(object[.dissimilarity, 22, 13] - 46) < 1e-10
	assert abs(object[.dissimilarity, 32, 27] - 79) < 1e-10
	assert abs(object[.dissimilarity, 32, 31] - 7) < 1e-10
	.norm = Get table norm

	.additiveConstant = Get additive constant
	assert abs (.additiveConstant -153.74)/ 153.74 < 1e-6

	removeObject: .dissimilarity
	appendInfoLine: tab$, "test Dissimilarity letter R OK"
endproc

procedure testProcrustes
	appendInfoLine: tab$, "Procrustes: Configuration & Configuration"
	.dissimilarity1 = Create letter R example: 0.4
	.configuration1 = To Configuration (monotone mds): 2, "Primary approach", 1e-5, 50, 1
	.dissimilarity2 = Create letter R example: 0.5
	.configuration2 = To Configuration (monotone mds): 2, "Primary approach", 1e-5, 50, 1
	selectObject: .configuration1, .configuration2
	.procrustes1 = To Procrustes: "no"
	plusObject: .configuration2
	.ct1 = To Configuration
	plusObject: .configuration1
	.procrustes2 = To Procrustes: "no"
	@check_if_identity_transform: .procrustes2
	removeObject: .procrustes1, .procrustes2, .ct1,.dissimilarity1, .dissimilarity2, .configuration1, .configuration2
	appendInfoLine: tab$, "Procrustes: Configuration & Configuration OK"
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

procedure testDissimilarityToConfiguration: .numberOfTries
	appendInfoLine: tab$, "Dissimilarity to Configuration"
	.mdsToConfigurationCommands$# = {"To Configuration (monotone mds): 2, ""Primary approach"", 1e-5, 2, 1",
	...	 "To Configuration (i-spline mds): 2, 1, 1, 1e-5, 2, 1",
	...	"To Configuration (interval mds): 2, 1e-5, 2, 1", 
	...	"To Configuration (ratio mds): 2, 1e-5, 2, 1", 
	...	"To Configuration (absolute mds): 2, 1e-5, 2, 1", 
	...	"To Configuration (kruskal): 2, 2, ""Primary approach"", ""Kruskal's stress-1"", 1e-5, 2, 1" }

	.mdsImproveConfigurationCommands$# = {"To Configuration (monotone mds): ""Primary approach"", 1e-5, 50, 1",
	...	 "To Configuration (i-spline mds): 1, 1, 1e-5, 50, 1",
	...	"To Configuration (interval mds): 1e-5, 50, 1", 
	...	"To Configuration (ratio mds): 1e-5, 50, 1", 
	...	"To Configuration (absolute mds): 1e-5, 50, 1", 
	...	"To Configuration (kruskal): ""Primary approach"", ""Kruskal's stress-1"", 1e-5, 50, 1" }

	.mdsStressQuery$# = { "Get stress (monotone mds): ""Primary approach"", ""Normalized""",	
	...	 "Get stress (i-spline mds): 1, 2, ""Normalized""",	
	...	 "Get stress (interval mds): ""Normalized""",	
	...	 "Get stress (ratio mds): ""Normalized""",	
	...	 "Get stress (absolute mds): ""Normalized""",	
	...	 "Get stress: ""Primary approach"", ""Kruskal's stress-1"""}	

	for .itry to .numberOfTries
		.noiseStdev = randomUniform (0, 10)
		.dissimilarity = Create letter R example: .noiseStdev
		for .icommand to size (.mdsToConfigurationCommands$#)
			selectObject: .dissimilarity
			.command$ = .mdsToConfigurationCommands$# [.icommand]
			.conf = '.command$'
			selectObject: .conf, .dissimilarity
			.getStressCommand$ = .mdsStressQuery$# [.icommand]
			.stressBefore = '.getStressCommand$'
			.improveCommand$ = .mdsImproveConfigurationCommands$# [.icommand]
			.confImproved = '.improveCommand$'
			selectObject: .dissimilarity, .confImproved
			.stressAfter = '.getStressCommand$'
			assert .stressAfter <= .stressBefore+1e-5; '.stressAfter' <= '.stressBefore'
			removeObject: .conf, .confImproved
		endfor
		removeObject: .dissimilarity
	endfor
	appendInfoLine: tab$, "Dissimilarity to Configuration OK"
endproc

procedure testDissimilarityInterface
	appendInfoLine: tab$, "test interface"
	appendInfoLine: tab$, tab$, "Query"
	.dissimilarity = Create letter R example: 0
	.numberOfRows = Get number of rows
	.numberOfColumns = Get number of columns
	for .irow to .numberOfRows
		for .icol to .numberOfColumns
			val = Get value: .irow, .icol
		endfor
	endfor

	appendInfoLine: tab$, tab$, "Modify: skipped"
	appendInfoLine: tab$, tab$, "Synthesize: skipped"

	appendInfoLine: tab$, tab$, "Extract part"
	selectObject: .dissimilarity
	.tor1 = Extract row ranges: "1 2"
	.numberOfRows1 = Get number of rows
	assert .numberOfRows1 == 2; '.numberOfRows1' "= 2"

	selectObject: .dissimilarity
	.tor2 = Extract rows where: "1"
	.numberOfRows2 = Get number of rows
	.numberOfColumns2 = Get number of columns
	assert .numberOfRows2 == .numberOfRows; '.numberOfRows2' "==" '.numberOfRows'
	assert .numberOfColumns2 == .numberOfColumns; '.numberOfColumns2' "==" '.numberOfColumns'

	selectObject: .dissimilarity
	.tor3 = Extract column ranges: "1 2"
	.numberOfColumns3 = Get number of columns
	assert .numberOfColumns3 == 2; '.numberOfColumns3' "= 2"

	selectObject: .dissimilarity
	.tor4 = Extract columns where: "1"
	.numberOfRows4 = Get number of rows
	.numberOfColumns4 = Get number of columns
	assert .numberOfRows4 == .numberOfRows; '.numberOfRows4' "==" '.numberOfRows'
	assert .numberOfColumns4 == .numberOfColumns; '.numberOfColumns4' "==" '.numberOfColumns'
	removeObject: .tor1, .tor2, .tor3, .tor4

	for .irow to .numberOfRows
		selectObject: .dissimilarity
		.rowLabel$ = Get row label: .irow
		.tori = Extract rows where label: "is equal to", .rowLabel$
		.numberOfRows5 = Get number of rows
		assert .numberOfRows5 >= 1
		removeObject: .tori
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

	removeObject: .dissimilarity
	appendInfoLine: tab$, "test interface OK"
endproc

procedure testCarrolWishExample
	appendInfoLine: tab$, "INDSCAL Carroll Wish example"
	Create INDSCAL Carroll Wish example: 0.0
	.dissimilarities# = selected# ("Dissimilarity")
	To Distance: "yes"
	.distances# = selected# ("Distance")
	To Configuration (indscal): 2, "no", 1e-5, 100, 1, "yes", "no"
	.conf = selected ("Configuration")
	.salience = selected ("Salience")
	selectObject: .dissimilarities# [1], .distances# [1]
	for .i from 2 to size (.dissimilarities#)
		plusObject: 	.dissimilarities# [.i], .distances# [.i]
	endfor
	plusObject: .salience, .conf
	Remove
	appendInfoLine: tab$, "INDSCAL Carroll Wish example OK"
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


