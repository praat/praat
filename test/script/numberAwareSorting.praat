n = 1e3   ; 1e3 for testing, 1e6 for measuring

array$# = {
... "1-2", "1-02", "1-20", "10-20",
... "file 0", "file 0b", "file0", "file00", "file0a",
... "file5", "file05", "file5a", "file5a5",
... "file5a05", "file05a5",
... "file5a005", "file05a05", "file005a5",
... "file5a0005", "file05a005", "file005a05", "file0005a5",
... "file5a5a",
... "fred", "jane", "jane",
... "kile 0", "kile 0", "kile 0b", "kile 0b", "kile0", "kile0", "kile00", "kile00", "kile0a", "kile0a",
... "pic   7", "pic 4 else", "pic 5", "pic 5 ", "pic 5 something", "pic 6",
... "pic01", "pic2", "pic02", "pic02a", "pic3",
... "pic4", "pic05", "pic100", "pic100a", "pic120", "pic121",
... "pic02000", "tom", "x2-g8", "x2-y7", "x2-y08", "x8-y8" }

stopwatch
for i to n
	sorted$# = sort$# (shuffle$# (array$#))
endfor
t = stopwatch / n

writeInfoLine: "pure Unicode sort: ", round (t * 1e9), " nanoseconds"

stopwatch
for i to n
	sorted$# = sort_numberAware$# (shuffle$# (array$#))
	if sorted$# <> array$#
		appendInfoLine: array$#
		appendInfoLine: sorted$#
		exitScript: "SORTING ERROR"
	endif
endfor
t = stopwatch / n

appendInfoLine: "number-aware sort: ", round (t * 1e9), " nanoseconds"
appendInfoLine: round (t * 1e9), " nanoseconds; OK"
