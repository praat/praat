a$ = replace_regex$ ("hallo", ".", "&&", 0)
assert a$ = "hhaalllloo"   ; 'a$'

a$ = replace_regex$ ("hɑllɔ", ".", "&&", 0)
assert a$ = "hhɑɑllllɔɔ"   ; 'a$'

assert replace_regex$ ("g a t - i", "d - i", "- j i", 1) = "g a t - i"

# something that went wrong when Rob compiled Praat with -O2 on mingw (20110916):
s$ = "ma0"
sNew$ = replace_regex$(s$, "5", "0", 0)
assert sNew$ = "ma0"
sNew2$ = replace_regex$(s$, "ma", "pa5", 0)
assert sNew2$ = "pa50"
