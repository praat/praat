# test_StringsIndex.praat
# djmw 20220810

appendInfoLine: "test_StringsIndex.praat"

# example from:  https://github.com/sourcefrog/natsort/blob/master/example-out.txt
example1 = Create Strings from tokens: "example", "1-2,1-02,1-20,10-20,fred,jane,pic01,pic2,pic02,pic02a,pic3,
...pic4,pic 4 else,pic 5,pic05,pic 5 ,pic 5 something,pic 6,pic   7,pic100,pic100a,pic120,pic121,
...pic02000,tom,x2-g8,x2-y7,x2-y08,x8-y8", ","

example2= Create Strings from tokens: "example", "pic 5,pic05,pic 5 ,pic 5 something", ","

str$# =     { "0b0 0a11 0a2 0a1",  "a  00a  0a", "a1 1b 11b b11 c2.2 c2..2 d3 .aa2 b4 b04 a1 a1",
	... "00d 00b 00c 0a ", "4b 04b 4a 004a 1 d c 004b"}
strn$# =  { "0a1 0a2 0a11 0b0",  "0a 00a a",  "1b 11b .aa2 a1 a1 a1 b4 b04 b11 c2.2 c2..2 d3",
	... "0a 00b 00c 00d", "1 4a 004a 4b 04b 004b c d" }

for ivec to size(str$#)
	.str = Create Strings from tokens: "str", str$# [ivec], " "
	.str$# = List all strings
	.stri = To StringsIndex: "natural"
	.stris= To Strings
	.stris$# = List all strings
	for i to size (.str$#)
		assert .str$#  [i] = .stris$# [i]
	endfor
	.strn = Create Strings from tokens: "strn", strn$# [ivec], " "
	.strn$# = List all strings
	selectObject: .str
	.p = To Permutation: "natural"
	selectObject: .str, .p
	.strp = Permute strings
	.strp$# = List all strings
	for i to size (.str$#)
		assert .strp$# [i] = .strn$# [i]
	endfor


	removeObject: .str, .stri, .stris, .strn
endfor
appendInfoLine: "test_StringsIndex.praat OK"