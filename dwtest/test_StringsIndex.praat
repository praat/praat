# test_StringsIndex.praat
# djmw 20221010

appendInfoLine: "test_StringsIndex.praat"

# example from:  https://github.com/sourcefrog/natsort/blob/master/example-out.txt
str = Create Strings from tokens: "example1", "1-2,1-02,1-20,10-20,fred,jane,pic01,pic2,pic02,pic02a,pic3,
...pic4,pic 4 else,pic 5,pic05,pic 5 ,pic 5 something,pic 6,pic   7,pic100,pic100a,pic120,pic121,
...pic02000,tom,x2-g8,x2-y7,x2-y08,x8-y8", ","

str$# = List all strings
strs$# = shuffle$# (str$#)
stri = To StringsIndex: "number-aware"
stris= To Strings
stris$# = List all strings
for i to size (str$#)
	assert str$#  [i] = stris$# [i]
endfor
selectObject: str
p = To Permutation: "number-aware"
selectObject: str, p
strp = Permute strings
strp$# = List all strings

removeObject: stri, stris, p, strp, str

appendInfoLine: "test_StringsIndex.praat OK"
