# test_Matrix_solve.praat
# djmw 20031020


procedure matrix_solve ncols
  for i to 4
    nrows = i * ncols
    printline nrows = 'nrows'; ncols = 'ncols'  
    eps = nrows * 1e-7
    Create simple Matrix... n'i' nrows ncols+1 0
    Formula... if (col <= ((row - 1) mod ncols)+1) then 1 else 0 fi
    Formula... if col = 1 then self + ((row-1) div ncols) else self fi
    Formula... if col = ncols+1 then (row-1) mod ncols + 1 + ((row-1) div ncols)  else self fi

    Solve equation... 0
    # solution must be all ones.
    for j to ncols
      s = Get value in cell... 1 j
      assert (s - 1) < eps ; nowsr = 'nrows' ncols = 'ncols'
    endfor
    select Matrix n'i'
    plus Matrix n'i'_solution
    Remove
  endfor
endproc

printline ----- test_Matrix_solve.praat -----

# test for several dimensions

call matrix_solve 1
call matrix_solve 10
call matrix_solve 100

printline ----- test_Matrix_solve.praat (end) -----
