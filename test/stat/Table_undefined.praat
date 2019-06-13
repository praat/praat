Create Table with column names: "table", 1, "speaker"
;Set string value: 1, "speaker", ""
a$ = Get value: 1, "speaker"
assert a$ = ""   ; <<'a$'>>
a = Get value: 1, "speaker"
assert a = undefined   ; 'a'
Remove