Create simple Matrix: "xy", 10, 10, "x*y"
a = Get value in cell: 3, 7
assert a = 21
a$ = Get value in cell: 3, 8
assert a$ = "24 (value in column 8 of row 3)"
Get value in cell: 9, 8
assert info$() = "72 (value in column 8 of row 9)" + newline$

asserterror The command "Draw inner box" does not return anything; not assigned to the numeric variable "a".
a = Draw inner box

a# = Get all values in row: 6
assert a# = { 6, 12, 18, 24, 30, 36, 42, 48, 54, 60 }

Remove

Create Sound from formula: "silence", 1, 0.0, 0.1, 44100, ~ 0.0
asserterror The command "Draw:" does not return anything; not assigned to the numeric variable "a".
a = Draw: 0.0, 0.0, 0.0, 0.0, "yes", "curve"

selectObject: "Sound silence"
a = Get value at time: 1, 0.05, "nearest"
assert a = 0.0   ; 'a'

selectObject: "Sound silence"
a$ = Get value at time: 1, 0.05, "nearest"
writeInfo: a$
assert a$ = "0 Pascal"   ; 'a$'

selectObject: "Sound silence"
asserterror The command "Get value at time:" returns a number or a string; not assigned to the vector variable "a#".
a# = Get value at time: 1, 0.05, "nearest"

selectObject: "Sound silence"
asserterror The command "Get value at time:" returns a number or a string; not assigned to the matrix variable "a##".
a## = Get value at time: 1, 0.05, "nearest"

selectObject: "Sound silence"
asserterror The command "Get value at time:" returns a number or a string; not assigned to the string array variable "a$#".
a$# = Get value at time: 1, 0.05, "nearest"

removeObject: "Sound silence"

#
# The following two tests are repeated from our first two tests,
# to check that the return type is reset to void by "Draw inner box" and "Draw...".
#
sound = Create Sound from formula: "silence2", 1, 0.0, 0.1, 44100, ~ 0.0   ; this sets the return type to OBJECT_
asserterror The command "Draw inner box" does not return anything; not assigned to the numeric variable "a".
a = Draw inner box
Remove
;
sound = Create Sound from formula: "silence2", 1, 0.0, 0.1, 44100, ~ 0.0   ; this sets the return type to OBJECT_
asserterror The command "Draw:" does not return anything; not assigned to the numeric variable "a".
a = Draw: 0.0, 0.0, 0.0, 0.0, "yes", "curve"
Remove

#
# Check clearing the return type to non-vector after setting it to vector.
#
sound = Create Sound from formula: "dummy", 1, 0.0, 0.1, 44100, ~ 0.0   ; this sets the return type to OBJECT_
times# = List all sample times
asserterror The command "Draw inner box" does not return anything; not assigned to the vector variable "a#".
a# = Draw inner box
Remove

strings1 = Create Strings from tokens: "tokens1", "there are seven tokens in this text", " "
strings2 = Copy: "tokens2"
selectObject: strings1, strings2
equal = Equal?
Remove
