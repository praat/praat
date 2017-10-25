# test_Procrustes.praat
# djmw 20040117, 20070820

appendInfoLine: "test_Procrustes.praat"

eps =2.3e-15


@test_orthogonal_procrustes_gvl_12_4_3
@test_procrustes_bg_19_4
@test_procrustes_random_configurations: 12

appendInfoLine: "test_Procrustes.praat OK"

procedure test_procrustes_bg_19_4
	# example Borg&Groenen section 19.4
	appendInfoLine: tab$, "Example in Borg & Groenen section 19.4"
	.nr = 4 
 	.nc = 2
	.scale = 0.5
	.t[1] = 1
 	.t[2] = 2
	.alpha = 30
	.c_x = Create Configuration: "X", .nr, .nc, "0"
	Set value... 1 1 1
	Set value... 1 2 2
	Set value... 2 1 -1
	Set value... 2 2 2
	Set value... 3 1 -1
	Set value... 3 2 -2
	Set value... 4 1 1
	Set value... 4 2 -2
  
	for .i to 4
		Set row label (index): .i, "x_" + string$ (.i)
	endfor
  
	.c_y = Copy: "Y"
	! Horizontal reflection
	Invert dimension: 1
	Rotate: 1, 2, .alpha
  
	Formula:  ".scale * self + .t[col]"
  
	selectObject: .c_x, .c_y
	.p_xy = To Procrustes: 0
	Rename: "X_Y"
	plusObject: .c_y
	.c_z = To Configuration
	Rename: "Z"
	Formula: "self - object[.c_x, row, col]"
	.eps = .nr * .nc * eps
	for .i to 4
		for .j to 2
			assert object[.c_z, .i , .j] < .eps; z['.i','.j'] < '.eps'
		endfor
	endfor
  
	selectObject: .p_xy
	.p_xy_i = Invert
	Rename: "X_Yi"
	for .i to 2
		.tp = Get translation element: .i
		assert .t[.i] - .tp < .eps; translation['i']
	endfor
	.sp = Get scale
	assert .scale - .sp < .eps; '.scale' '.sp'
  
	removeObject: .c_x, .c_y, .c_z, .p_xy, .p_xy_i
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

procedure test_orthogonal_procrustes_gvl_12_4_3
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

	plusObject: .a
	.p = To Procrustes: "yes"

	.t2 = Get translation element: 2
	assert .t2 = 0
	.s = Get scale
	assert .s = 1; 

	.eps = 5e-5
	.r11 = Get transformation element: 1, 1
	.r22 = Get transformation element: 2, 2
	assert .r11-.r22 < .eps
	assert .r11-0.9999 < .eps
	.r12 = Get transformation element: 1, 2
	.r21 = Get transformation element: 2, 1
	assert .r12+0.0126 < .eps
	assert .r21-0.0126 < .eps

	removeObject: .a, .b, .p
endproc
