asserterror The command "Draw inner box" does not return anything. Not assigned to variable "a".
a = Draw inner box

Create Sound from formula: "silence", 1, 0.0, 0.1, 44100, ~ 0.0
asserterror The command "Play" does not return anything. Not assigned to variable "a".
a = Play

selectObject: "Sound silence"
a = Get value at time: 1, 0.05, "nearest"
assert a = 0.0   ; 'a'

selectObject: "Sound silence"
a$ = Get value at time: 1, 0.05, "nearest"
writeInfo: a$
assert a$ = "0 Pascal"   ; 'a$'

selectObject: "Sound silence"
asserterror The command "Get value at time..." returns a number or a string. Not assigned to the vector variable "a#".
a# = Get value at time: 1, 0.05, "nearest"

selectObject: "Sound silence"
asserterror The command "Get value at time..." returns a number or a string. Not assigned to the matrix variable "a##".
a## = Get value at time: 1, 0.05, "nearest"

selectObject: "Sound silence"
asserterror The command "Get value at time..." returns a number or a string. Not assigned to the string array variable "a$#".
a$# = Get value at time: 1, 0.05, "nearest"

#
# The following two tests are repeated from our first two tests,
# to check that the return type is reset to void by "Draw inner box" and "Play".
#
sound = Create Sound from formula: "silence2", 1, 0.0, 0.1, 44100, ~ 0.0   ; this sets the return type to OBJECT_
asserterror The command "Draw inner box" does not return anything. Not assigned to variable "a".
a = Draw inner box
Return
;
sound = Create Sound from formula: "silence2", 1, 0.0, 0.1, 44100, ~ 0.0   ; this sets the return type to OBJECT_
asserterror The command "Play" does not return anything. Not assigned to variable "a".
a = Play
Remove

#
# Check clearing the return type to non-vector after setting it to vector.
#
sound = Create Sound from formula: "dummy", 1, 0.0, 0.1, 44100, ~ 0.0   ; this sets the return type to OBJECT_
times# = List all sample times
asserterror The command "Draw inner box" does not return anything; not assigned to the vector variable "a#".
a# = Draw inner box
Remove


