# test_regex.praat
# djmw 20070528, 20070917, 20080122, 20110526

printline test_regex

debug = 0

procedure match_index .s$ .match$ .i
  .il =  index_regex (.s$, .match$)
  if debug = 1
    printline '.i' ('.il' = index_regex ("'.s$'", "'.match$'"))
  endif
  assert .il = .i; index_regex ("'.s$'", "'.match$'")
endproc

procedure match_rindex .s$ .match$ .i
  .ir =  rindex_regex (.s$, .match$)
  if debug = 1
    printline '.i' ('.ir' = rindex_regex ("'.s$'", "'.match$'"))
  endif
  assert .ir = .i; rindex_regex ("'.s$'", "'.match$'")
endproc

procedure replace_re .string$ .search$ .replace$ .n .result$
  .r$ = replace_regex$ (.string$, .search$, .replace$, .n)
  if debug = 1
    printline '.result$' ("'.r$'" = replace_regex$ ("'.string$'", "'.search$'", "'.replace$'", "'.n'"))
  endif
  assert .r$ = .result$; '.result$' ("'.r$'" = replace_regex ("'.string$'", "'.search$'", "'.replace$'", "'.n'"))
endproc

# ordinary characters

printline ----  # match characters
alphabet$ = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

s$ = alphabet$
sl = length (s$)
sd$ = s$ + s$

for i to 52
  match$ = mid$ (s$, i, 1)
  call  match_index "'s$'" "'match$'" i
  ir = i + sl
  call  match_rindex "'sd$'" "'match$'" ir
endfor

printline ----  # special characters # backslash \

call match_index "cscscAaa\n" "aa\\n" 7
call match_rindex "cscscAaa\n" "aa\\n" 7

call match_index "cscscAa+a+" "b\+" 0
call match_index "cscscAa+a+" "a\+" 7
call match_rindex "cscscAa+a+" "b\+" 0
call match_rindex "cscscAa+a+" "a\+" 9

printline ----  # special characters # caret ^

call match_index "cscscAa+a+" "^c" 1
call match_rindex "cscscAa+a+" "^c" 1 

printline ----  # special characters # dollar $

call match_index "cscscAa+ac" "c$" 10
call match_rindex "cscscAa+ac" "c$" 10

printline ----  # special characters # quantifier brackets {}

call match_index "cscscAaa\n" "a{1}" 7
call match_rindex "cscscAaa\n" "a{1}" 8
call match_index "cscscAaa\n" "a{2}" 7
call match_rindex "cscscAaa\n" "a{2}" 7
call match_index "cscscAaa\n" "a{2,}" 7
call match_rindex "cscscAaa\n" "a{2,}" 7
call match_index "cscscAaa\n" "a{,2}" 1
call match_rindex "cscscAaa\n" "a{,2}" 11
call match_index "cscscAaa\n" "a{1,2}" 7
call match_rindex "cscscAaa\n" "a{1,2}" 8

printline ----  # special characters # open and close brackets []

s$ = alphabet$
sd$ = s$ + s$
for i to 26
  # match [a-z], [b-z], ..., [z-z]
  bl$ = mid$ (s$, i, 1)
  match$ = "[" + bl$ + "-z]"
  call match_index "'s$'" "'match$'" i
  call match_rindex "'sd$'" "'match$'" 78
  # now for uppercase
  bu$ = mid$ (s$, i+26, 1)
  match$ = "[" + bu$ + "-Z]"
  il = i+26
  call match_index "'s$'" "'match$'" il
  ir = 104
  call match_rindex "'sd$'" "'match$'" ir
  # match [a-a], [b-b], [z-z]
  match$ = "[" + bl$ + "-" + bl$ +"]"
  call match_index "'s$'" "'match$'" i
  ir = 52+i
  call match_rindex "'sd$'" "'match$'" ir  
endfor

printline ----  # special characters # grouping characters ()

call match_index "c" "(ab)" 0
call match_index "cc" "(ab)" 0
call match_index "ccc" "(ab)" 0
call match_index "ac" "(ab)" 0
call match_index "acc" "(ab)" 0
call match_index "accc" "(ab)" 0
call match_index "aca" "(ab)" 0
call match_index "acca" "(ab)" 0
call match_index "accca" "(ab)" 0

call match_index "ababcccc" "(ab)" 1
call match_rindex "ababcccc" "(ab)" 3
call match_index "cababcccc" "(ab)" 2
call match_rindex "cababcccc" "(ab)" 4
call match_index "cccccccabab" "(ab)" 8
call match_rindex "cccccccabab" "(ab)" 10
call match_index "ababcccc" "(ab)\1" 1
call match_rindex "ababcccc" "(ab)\1" 1

printline ----  # special characters # dot .

call match_index "ababcccc" ".d" 0
call match_rindex "ababcccc" ".d" 0
call match_index "ababcccc" ".b" 1
call match_rindex "ababcccc" ".b" 3
call match_index "ababcccc" ".a" 2
call match_rindex "ababcccc" ".a" 2
call match_index "ababcccc" ".c" 4
call match_rindex "ababcccc" ".c" 7

printline ----  # special characters # star *

call match_index "ababccccd" "b*" 1
call match_index "ababccccd" "ab*" 1
call match_rindex "ababccccd" "ab*" 3
call match_index "ababccccd" "^.*$" 1

printline ----  # special characters # plus +

call match_index "ababccccd" "b+" 2
call match_rindex "ababccccd" "b+" 4

printline ----  # special characters # question mark ?

call match_index "ababccccd" "ab?" 1
call match_rindex "ababccccd" "ab?" 3

printline ----  # quantifiers # *

call replace_re "ab" "(ab)c*" "x" 0 x
call replace_re "abc" "(ab)c*" "x" 0 x
call replace_re "abcc" "(ab)c*" "x" 0 x
call replace_re "abcccd" "(ab)c*" "x" 0 xd

call replace_re "ab" "(ab)c+" "x" 0 ab
call replace_re "abc" "(ab)c+" "x" 0 x
call replace_re "abcc" "(ab)c+" "x" 0 x
call replace_re "abcccd" "(ab)c+" "x" 0 xd
call replace_re "yyabcccd" "(ab)c+" "x" 0 yyxd

printline ----  # non greedy ?

call replace_re "abc" "(ab)c*?" "x" 0 xc
call replace_re "abcc" "(ab)c*?" "x" 0 xcc
call replace_re "abcccd" "(ab)c*?" "x" 0 xcccd

call replace_re "abcc" "(ab)c+?" "x" 0 xc
call replace_re "abcccd" "(ab)c+?" "x" 0 xccd

printline ----  # anchors

call replace_re "ababccccd" "ab" "x" 1 xabccccd

call replace_re "ababccccd" "^ab" "x" 1 xabccccd

printline ---  # empty string

call replace_re "" ".*" "aaa" 0 aaa
call replace_re "" ".*" "aaa" 1 aaa
call replace_re "" ".*" "aaa" 10 aaa

# special constructs with parentheses

# special control characters

# convenience escapes

# octal and hexadecima escapes

# sustitution special characters

printline ----  # replaced string much longer than original

call replace_re "ababababababababababababababababababab" "b" "xxxxx" 1 axxxxxabababababababababababababababababab

# The returned value is "xxccccd" instead of "xabccccd".
call replace_re "ababccccd" "^ab" "x" 0 xabccccd

printline --------- PREVIOUS BUGS < 5.0.4
printline ----  # replace by empty string ""
# Bugs in praat versions before 5.0.4
# Returned empty string ""
call replace_re "abc " " $" "" 1 abc
# Returned "xxccccd" instead of "xabccccd".
call replace_re "ababccccd" "^ab" "x" 0 xabccccd
printline --------- PREVIOUS BUGS < 5.0.5
call replace_re "hallo" "l" "b" 0 habbo
call replace_re "hallo" "." "&&" 0 hhaalllloo
printline --------- PREVIOUS BUGS --- END

printline --------- BUG in 5.0.40
call replace_re "c " ".*" "aaaa" 0 aaaa
printline --------- PREVIOUS BUGS --- END

printline test_regex OK
