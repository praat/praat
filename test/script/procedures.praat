writeInfoLine: "Procedures"

@colon (33,"ho,p"",""kkk", 5, "h")
assert colon.result$ = "colon33ho,p"",""kkk5h"

call old 1 a 2 b
assert old.result$ = "old1a2b"
@old (3, "a", 4, "b")
assert old.result$ = "old3a4b"
@old: 5, "a", 6, "b"
assert old.result$ = "old5a6b"

call new 1 a 2 b
assert new.result$ = "new1a2b"
@new (3, "a", 4, "b")
assert new.result$ = "new3a4b"
@new: 5, "a", 6, "b"
assert new.result$ = "new5a6b"

call colon 1 a 2 b
assert colon.result$ = "colon1a2b"
@colon (3, "a", 4, "b")
assert colon.result$ = "colon3a4b"   ; 'colon.result$'
@colon: 5, "a", 6, "b"
assert colon.result$ = "colon5a6b"

@noarg1()
@noarg1 ( )
@noarg1:
@noarg1
@noarg1   
call noarg1

@noarg2()
@noarg2 ( )
@noarg2:
@noarg2
@noarg2   
;call noarg2

;asserterror The procedure "colon" expects 4 arguments but got only 3.
;@colon (1, "a", 2)

procedure old .a .b$ .c .d$
	.result$ = "old" + string$ (.a) + .b$ + string$ (old.c) + .d$
endproc

procedure new (.a, .b$, .c, .d$)
	.result$ = "new" + string$ (.a) + .b$ + string$ (new.c) + .d$
endproc

procedure colon: .a, .b$, .c, .d$
	.result$ = "colon" + string$ (.a) + .b$ + string$ (colon.c) + .d$
endproc

procedure noarg1
	appendInfoLine ("hoi1")
endproc

procedure noarg2 ( )
	appendInfoLine ("hoi2")
endproc
