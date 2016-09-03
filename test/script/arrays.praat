writeInfoLine: "vectors and matrices..."

a# = zero#(16)
a#[3] = 4
assert a#[3] = 4

asserterror Vector b# does not exist.
b# [5] = 3

asserterror A vector index cannot be less than 1 (the index you supplied is 0).
a# [0] = 932875289

asserterror A vector index cannot be greater than the number of elements (here 16). The index you supplied is 20.
a# [20] = 45786457

assert numberOfRows (zero## (5, 6)) = 5
assert numberOfColumns (zero## (5, 6)) = 6
a## = zero## (5, 6)
assert numberOfRows (a##) = 5
assert a## [3, 4] = 0

a## [5, 6] = 567
assert a##[5,6] = 567

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

e# = a# + a#
assert e# [3] = 8

asserterror numbers of elements should be equal
e# = a# + d#

; q### =
; data####
;e# = d# + c#

;speaker$# = empty$# [2]
;speaker$# [1] = "JM"
;speaker$# [2] = "PB"

;speaker$ [1] = "JM"
;speaker$ [2] = "PB"

printline OK
