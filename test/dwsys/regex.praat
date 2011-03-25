a$ = replace_regex$ ("hallo", ".", "&&", 0)
assert a$ = "hhaalllloo"   ; 'a$'

a$ = replace_regex$ ("hɑllɔ", ".", "&&", 0)
assert a$ = "hhɑɑllllɔɔ"   ; 'a$'