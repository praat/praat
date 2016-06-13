writeInfoLine: "arrays..."

# should give a different error:
;a

a=5

# should give an error:
a+5

a [1] = 3
assert a [1] = 3
appendInfoLine: a [1]

; abcdefghijklmnopqrstuvwxyz
;a [12345678]

a = 7
asserterror Missing expression after variable a[9].
a [a+2] =

;a [2]

a [3], 5 = 7
printline 'a[3,5]', 'a[3]'

a [1] = 2
b [a [1]] = 3
assert b [a [1]] = 3
printline 'b[2]'

speaker$[1]="paul"
printline <'speaker$[1]'>
speaker$ [2] = "silke"
printline <'speaker$[2]'>
a$ = speaker$ [1] + " " + speaker$ [2]
printline <'a$'>

a[2+length("h]""k")]=6
assert a[6] = 6
a[6]+=3
assert a[6]=9

speaker$ [1] = "JM"
speaker$ [2] = "PB"

printline OK
