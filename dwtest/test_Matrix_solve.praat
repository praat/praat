# test_Matrix_solve.praat
# djmw 20031020, 20171211

appendInfoLine: "test_Matrix_solve.praat"

procedure matrix_solve: .ncols
  for .i to 4
    .nrows = .i * .ncols
   appendInfoLine: tab$, "nrows = ", .nrows, ", ncols = ", .ncols
    .eps = .nrows * 1e-7
    .m = Create simple Matrix: string$(.i), .nrows, .ncols+1, "0.0"
    Formula: "if (col <= ((row - 1) mod .ncols)+1) then 1 else 0 fi"
    Formula: "if col = 1 then self + ((row-1) div .ncols) else self fi"
    Formula: "if col = .ncols+1 then (row-1) mod .ncols + 1 + ((row-1) div .ncols)  else self fi"

    .ms = Solve equation: 0
    # solution must be all ones.
    for .j to .ncols
      .s = Get value in cell: 1, .j
      assert (.s - 1.0) < .eps ; nowsr = '.nrows' ncols = '.ncols'
    endfor
    removeObject: .m, .ms
  endfor
endproc

# test for several dimensions

@matrix_solve: 1
@matrix_solve: 10
@matrix_solve: 100

appendInfoLine: "test_Matrix_solve.praat OK"
