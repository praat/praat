echo Local variables
procedure add .x .y
	.result = .x + .y
endproc
procedure square .x
	.result = .x ^ 2
endproc
procedure hypotenuse .x .y
	call square .x
	.x2 = square.result
	call square .y
	.y2 = square.result
	call add .x2 .y2
	.result = sqrt (add.result)
endproc
call hypotenuse 3 4
assert hypotenuse.result = 5
assert add.result = 25
assert square.result = 16
printline OK