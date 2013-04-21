writeInfoLine ("Local variables")
procedure add (.x, .y)
	.result = .x + .y
endproc
procedure square (.x)
	.result = .x ^ 2
endproc
procedure hypotenuse (.x, .y)
	@square (.x)
	.x2 = square.result
	@square (.y)
	.y2 = square.result
	@add (.x2, .y2)
	.result = sqrt (add.result)
endproc
@hypotenuse (3, 4)
assert hypotenuse.result = 5
assert add.result = 25
assert square.result = 16
printline OK