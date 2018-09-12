# test_Procrustes.praat
# djmw 20040117, 20070820

appendInfoLine: "test_Procrustes.praat"

eps =2.3e-15
@bg_19_4
@test_orthogonal_procrustes_gvl_12_4_1
@test_procrustes_random_configurations: 12

appendInfoLine: "test_Procrustes.praat OK"

procedure bg_19_4
	# example Borg & Groenen section 19.4
	appendInfoLine: tab$, "Example in Borg & Groenen section 19.4"
	.nrow = 4
	.ncol = 2
	.x1# = { 1,  2}
	.x2# = {-1,  2}
	.x3# = {-1, -2}
	.x4# = { 1, -2}
	.y1# = {0.07, 2.62}
	.y2# = {0.93, 3.12}
	.y3# = {1.93, 1.38}
	.y4# = {1.07, 0.88}
	.xconf = Create Configuration: "X", .nrow, .ncol, "0"
	.yconf = Create Configuration: "Y", .nrow, .ncol, "0"
	for .icol to .ncol
		selectObject: .xconf
		Set value: 1, .icol, .x1# [.icol]
		Set value: 2, .icol, .x2# [.icol]
		Set value: 3, .icol, .x3# [.icol]
		Set value: 4, .icol, .x4# [.icol]
		selectObject: .yconf
		Set value: 1, .icol, .y1# [.icol]
		Set value: 2, .icol, .y2# [.icol]
		Set value: 3, .icol, .y3# [.icol]
		Set value: 4, .icol, .y4# [.icol]
	endfor
	selectObject: .xconf, .yconf
	.p = To Procrustes: "no"
	.t_given# = {3.72, -2.46}; 3.73, -2.47
	.r1_given# = {-0.87, -0.50}
	.r2_given# = {-0.50, 0.87}
	.s_given = 2.0
	.s = Get scale
	.srounded = number (fixed$ (.s, 1))
	assert .srounded = .s_given; '.srounded' = '.s_given'
	for .i to .ncol
		.t = Get translation element: .i
		.trounded = number (fixed$ (.t, 2))
		.tgiven = .t_given#[.i]
		assert .trounded = .tgiven; '.trounded' = '.tgiven'
	endfor
	removeObject: .xconf, .yconf, .p
endproc

procedure test_procrustes_random_configurations: .numconf
	appendInfoLine: tab$, .numconf, " randomly generated configurations of dimension 2^k x 2"
	.nc = 2
	for .k to .numconf
		.nr = 2^.k
		appendInfoLine: tab$, tab$, .nr, " rows"
		.c_x = Create Configuration: "X", .nr, .nc, "randomUniform(-1, 1)"
 		.c_y = Copy: "Y"
		Invert dimension: 1
		.alpha = randomUniform (0,90)
		Rotate: 1, 2, .alpha
		.t1 = randomUniform (0,2)
 		.t2 = randomUniform (0,2)
 		.scale = randomUniform(0.5,2)
		Formula...  .scale*self + (if col=1 then .t1 else .t2 fi)
  
		plusObject: .c_x
		.p_xy = To Procrustes: 0
		Rename: "X_Y"
		plusObject: .c_y
		.c_z = To Configuration
		Rename: "Z"
		Formula: "self -object[.c_x, row, col]"
		.eps = 10 * .nr * .nc * eps
		for .i to .nr
 			for .j to 2
 				assert object[.c_z, .i, .j] < .eps; Configuration_Z['.i','.j'] < '.eps'
			endfor
		endfor
  
		selectObject: .p_xy
		.p_xy_i = Invert
		Rename: "X_Yi"
		# no need to test the translations, they need not be equal (see BG page 347)
		.sp = Get scale
 		assert abs(.scale - .sp) < .eps; '.scale' '.sp'

 		removeObject: .c_x, .c_y, .c_z, .p_xy, .p_xy_i
	endfor
endproc

procedure test_orthogonal_procrustes_gvl_12_4_1
	appendInfoLine: tab$, "Orthognal Procrustes transform (example 12.4.1 Golub & van Loan)"
	.a = Create Configuration: "a", 4, 2, "0"
	Set value: 1, 1, 1
	Set value: 2, 1, 3
	Set value: 3, 1, 5
	Set value: 4, 1, 7
	Set value: 1, 2, 2
	Set value: 2, 2, 4
	Set value: 3, 2, 6
	Set value: 4, 2, 8

	.b = Create Configuration: "b", 4, 2, "0"
	Set value: 1, 1, 1.2
	Set value: 2, 1, 2.9
	Set value: 3, 1, 5.2
	Set value: 4, 1, 6.8
	Set value: 1, 2, 2.1
	Set value: 2, 2, 4.3
	Set value: 3, 2, 6.1
	Set value: 4, 2, 8.1

	selectObject: .a, .b
	.p = To Procrustes: "yes"

	.t2 = Get translation element: 2
	assert .t2 = 0
	.s = Get scale
	assert .s = 1; 

	.eps = 5e-5
	.r11 = Get transformation element: 1, 1
	.r22 = Get transformation element: 2, 2
	assert .r11-.r22 < .eps; '.r11'-'.r22' < '.eps' ?
	assert .r11-0.9999 < .eps; '.r11'-0.9999 < '.eps' ?
	.r12 = Get transformation element: 1, 2
	.r21 = Get transformation element: 2, 1
	assert .r12+0.0126 < .eps; '.r12'+0.0126 < '.eps' ?
	assert .r21-0.0126 < .eps; '.r21'-0.0126 < '.eps' ?

	removeObject: .a, .b, .p
endproc
