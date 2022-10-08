# test_StringsIndex.praat
# djmw 20220810

appendInfoLine: "test_StringsIndex.praat"

str$# =     { "0b0 0a11 0a2 0a1",  "a  00a  0a", "a1 1b 11b b11 c2.2 c2..2 d3 .aa2 b4 b04 a1 a1",
	... "00d 00b 00c 0a "}
strn$# =  { "0a1 0a2 0a11 0b0",  "00a 0a a",  "1b 11b .aa2 a1 a1 a1 b04 b4 b11 c2..2 c2.2 d3",
	... "00b 00c 00d 0a" }

for ivec to size(str$#)
	.str = Create Strings from tokens: "str", str$# [ivec], " "
	.stri = To StringsIndex: "numerical part"
	.stris= To Strings
	.str$# = List all strings
	.strn = Create Strings from tokens: "strn", strn$# [ivec], " "
	.strn$# = List all strings
	for i to size (.str$#)
		assert .str$#  [i] = .strn$# [i]
	endfor
	removeObject: .str, .stri, .stris, .strn
endfor
appendInfoLine: "test_StringsIndex.praat OK"