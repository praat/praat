echo arrays...
; writeInfoLine ("arrays...")

# should give a different error:
;a

a=5

# should give an error:
a+5

a [1] = 3
assert a [1] = 3
printline 'a[1]'
; appendInfoLine (a [1])

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

assert numberOfRows (zero# (5, 6)) = 5
assert numberOfColumns (zero# (5, 6)) = 6
a# = zero# (5, 6)
assert numberOfRows (a#) = 5
assert a# [3, 4] = 0

c# = linear# (0, 100, 101)
assert c# [98] = 97
c# = linear# (0, 100, 101, 0)
assert c# [98] = 97
c# = linear# (0, 100, 100, 1)
assert c# [98] = 97.5

d# = randomGauss# (c#, 20, 1)
a = d# [98]
b = d# [99]
c = d# [100]
printline 'a' 'b' 'c'

d# = randomUniform# (c#, 7, 10)
a = d# [98]
b = d# [99]
c = d# [100]
printline 'a' 'b' 'c'

d# = randomInteger# (c#, 7, 10)
a = d# [98]
b = d# [99]
c = d# [100]
printline 'a' 'b' 'c'

;e# = d# + c#

;speaker$# = empty$# [2]
;speaker$# [1] = "JM"
;speaker$# [2] = "PB"

;speaker$ [1] = "JM"
;speaker$ [2] = "PB"

printline OK
