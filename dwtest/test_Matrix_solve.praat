# test_Matrix_solve.praat
# djmw 20031020, 20171211,20180918,20100406

appendInfoLine: "test_Matrix_solve.praat"

@solve_undetermined: 10, 100
@solve3x3
for i to 200
	@solve_sparse_system
endfor

@solve2by3
@matrix_solve: 1
@matrix_solve: 10
@matrix_solve: 100

procedure solve_sparse_system
	.nrow = 100
	.ncol = 1000
	.x# = zero# (.ncol)
	.numberOfNonZeros = 0
	for .i to size (.x#)
		.x# [.i] = 0.0
	 	if randomUniform (0,1) < 0.005
		 	.x# [.i] = randomUniform (0.1, 10)
			.numberOfNonZeros += 1;
		endif
	endfor
	if .numberOfNonZeros > 0
		.phi## = randomGauss## (.nrow, .ncol, 0.0, 1.0 / .nrow)
		.y# = mul# (.phi##, .x#)
		.numberOfNonzerosToSearch = .numberOfNonZeros + 5
		appendInfoLine: "Sparse 100x100: solve for ",  .numberOfNonZeros, " non zero elements."
		.xs# = solveSparse# (.phi##, .y#, .numberOfNonzerosToSearch, 200, 1e-17, 0) ; 6 arguments
		.dif# = .x# - .xs#
		.inner = inner (.dif#, .dif#)
		assert .inner < 1e-7; '.inner'
		.xs2# = solveSparse# (.phi##, .y#, .xs#,  .numberOfNonzerosToSearch, 10, 1e-20, 1) ; 7 arguments
		.dif# = .x# - .xs2#
		.inner = inner (.dif#, .dif#)
		assert .inner < 1e-7; '.inner'
	endif
endproc

procedure matrix_solve: .ncol
  for .i to 4
    .nrow = .i * .ncol
    appendInfoLine: tab$, "nrow = ", .nrow, ", ncol = ", .ncol
    .eps = .nrow * 1e-7
    .m = Create simple Matrix: string$(.i), .nrow, .ncol+1, "0.0"
    Formula: "if (col <= ((row - 1) mod .ncol)+1) then 1 else 0 fi"
    Formula: "if col = 1 then self + ((row-1) div .ncol) else self fi"
    Formula: "if col = .ncol+1 then (row-1) mod .ncol + 1 + ((row-1) div .ncol)  else self fi"

    .ms = Solve equation: 0
    # solution must be all ones.
    for .j to .ncol
      .s = Get value in cell: 1, .j
      assert abs(.s - 1.0) < .eps ; nowsr = '.nrow' ncols = '.ncol'
    endfor
    removeObject: .m, .ms
  endfor
endproc

procedure solve_undetermined: .nrow, .ncol
	appendInfoLine: tab$, "underdetermined system"
	 .m = Create simple Matrix: "u", .nrow, .ncol, "if row==col then 1 else 0 fi"
	Formula: "if col==row+1 then 1 else self fi"
	Formula: "if col == .ncol then if row == .nrow then 1 else 2 fi  else self fi"
	.ms = nowarn Solve equation: 1e-7
	 .ncols = Get number of columns
	assert .ncols == .ncol - 1
	for .irow to .nrow
		.c = Get value in cell: 1, .irow
		assert .c > 1-1e-7 and .c < 1+1e-7
	endfor
	removeObject: .m, .ms
endproc

# test for several dimensions

procedure solve2by3
	.nrow = 2
	.ncol = 3
  	appendInfoLine: tab$, "nrow = ", .nrow, ", ncol = ", .ncol
	.m = Create simple Matrix: "2x3", .nrow, .ncol, "0"
	Set value: 1, 1, 1
	Set value: 1, 2, 2
	Set value: 1, 3, 3
	Set value: 2, 1, 3
	Set value: 2, 2, 5
	Set value: 2, 3, 8
	.eps = 1.2e-7
	.solution = Solve equation: .eps	
	.s1 = Get value in cell: 1, 1
	.s2 = Get value in cell: 1, 2
	assert .s1-.eps < 1 and .s1+.eps > 1; '.s1'
	assert .s2-.eps < 1 and .s2+.eps > 1; '.s2'
	removeObject: .m, .solution		
endproc

procedure solve3x3
; a * x = b
	.a## = {{1,2,3}, {1,5,7},{2,3,5}}
	.x## = {{1,12},{2,20},{3,30}}
	.b## ={{14,142},{32,322},{23,234}}
	.a = Create simple Matrix: "1", 3, 4, "0"
	for .irow to 3
		for .icol to 3
			Set value: .irow, .icol,  .a## [.irow, .icol]
		endfor
		Set value: .irow, 4, .b## [.irow, 1]
	endfor
	.xx = Solve equation: 0
        .eps = 1e-9
	for .irow to 3
		.dif = abs (object [.xx, 1, .irow] - .x## [.irow, 1])
		assert .dif < .eps; '.dif'< '.eps'
	endfor
  	appendInfoLine: tab$, "A[3x3] * X[3x2] = B[3x2]"
	removeObject: .a, .xx
	.a = Create simple Matrix: "1", 3, 3, "0"
	for .irow to 3
		for .icol to 3
			Set value: .irow, .icol,  .a## [.irow, .icol]
		endfor
	endfor
	.b = Create simple Matrix: "1", 3, 2, "0"
	for .irow to 3
		for .icol to 2
			Set value: .irow, .icol,  .b## [.irow, .icol]
		endfor
	endfor
	selectObject: .a, .b
	.xx = Solve matrix equation: 1e-7
	for .irow to 3
		for .icol to 2
		.dif = abs (object [.xx, .irow, .icol] - .x## [.irow, .icol])
		assert .dif < .eps; '.dif'< '.eps'
	endfor
	removeObject: .b, .xx, .a
endproc
appendInfoLine: "test_Matrix_solve.praat OK"
