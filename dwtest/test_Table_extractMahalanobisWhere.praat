# test_Table_extractMahalanobisWhere.praat
# djmw 20140509

appendInfoLine: "Table_extractMahalanobisWhere test"

t = Create Table with column names: "t", 1000, "f x y"
Formula (column range): "f", "f", "randomInteger (1,1)"
Formula (column range): "x", "y", "randomGauss (self[1],0.1)"
tm = Extract rows where (mahalanobis): "x y", "greater than", 3, "f", "1"
tm1 = Extract rows where: "self$[""f""]=""1"""
nm1 = Get number of rows
selectObject: t
t1 = Extract rows where: "self$[""f""]=""1"""
t1m1 = Extract rows where (mahalanobis): "x y", "greater than", 3, "", "1"
n1m1 = Get number of rows
assert nm1 = n1m1
# only for interactive use
; @draw
procedure draw
	Erase all
	selectObject: t
	Select outer viewport: 0, 6, 0, 6
	Scatter plot: "x", 0, 4, "y", 0, 4, "f", 10, "yes"
	Colour: "Red"
	Draw ellipses: "x", 0, 4, "y", 0, 4, "f", 3, 12, "no"
	Colour: "Black"
endproc

removeObject: t, tm, tm1, t1, t1m1

appendInfoLine: "Table_extractMahalanobisWhere test OK"
