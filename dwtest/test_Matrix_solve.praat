# test_Matrix_solve.praat
# djmw 20031020, 20171211,20180918

appendInfoLine: "test_Matrix_solve.praat"

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

# test for several dimensions

@solve2by3
@matrix_solve: 1
@matrix_solve: 10
@matrix_solve: 100

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
appendInfoLine: "test_Matrix_solve.praat OK"
