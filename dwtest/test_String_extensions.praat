# test_String_extensions.praat
# djmw 20170401

appendInfoLine: "test_String_extensions"

procedure test: .string$, .separators$, .ntokens
	tokens = Create Strings as tokens: .string$, .separators$
	numberOfStrings = Get number of strings
	assert numberOfStrings = .ntokens; "'.string$'", "'.separators$', 'numberOfStrings'"
	removeObject: tokens
endproc

@test: "a", " ", 1
@test: "a", ",", 1
@test: "a", "	", 1

@test: "a b c", " ", 3
@test: "a b c", " ,", 3
@test: "a b c", ",", 1

@test: "a   b  c", ",", 1
@test: "a   b   c", " ", 3
@test: "a,,,,  b   c", " ", 3
@test: "a,,,,b,,,c", " ", 1
@test: "a,,,,b,,,c", ",", 3
@test: "a", " ,", 1
@test: "a	b", " ,	", 2
@test: "	a	b", " ,	", 2
@test: "a,,,,b,,,c", " ", 1
@test: "a, ,b, ,c", ",", 5

@test: "A string\tof ..tokens\nand some  more tokens", ".,", 2
@test: "A string\tof ..tokens\nand some  more tokens", " .,", 6
@test: "A string\tof ..tokens\nand some  more tokens", " .,", 6
@test: "A string" + tab$+ "of ..tokens"+newline$+"and some  more tokens", " .,"+tab$+newline$, 8

appendInfoLine: "test_String_extensions OK"
