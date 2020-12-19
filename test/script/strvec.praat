writeInfoLine: "string vectors..."

a$ = Calculator: ~ { "hello", "goodbye" }
assert a$ = "hello" + newline$ + "goodbye" + newline$

assert { "" } = {""}

assert { "hello", "goodbye" } = { "hello", "goodbye" }
assert ({ "hello", "goodbye" } = { "hello", "goodbye" }) = 1

assert not { "hello", "goodbye" } <> { "hello", "goodbye" }
assert ({ "hello", "goodbye" } <> { "hello", "goodbye" }) = 0

writeInfoLine: { "hello", "goodbye" }

a$# = { "hello", "goodbye" }
a$# [1] = "hallo"

assert a$# [1] = "hallo"
assert a$# [2] = "goodbye"
b$# = a$#
writeInfoLine: "<<", a$#, ">>"

assert a$# = { "hallo", "goodbye" }

appendInfoLine: "OK"
