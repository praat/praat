# text100MB.praat
# Paul Boersma 2020-04-19

integerProperties$ = Report integer properties
bits = extractNumber (integerProperties$, "A pointer is")

echo Text 100 MB:
stopwatch
a$ = "a"
for i to if bits = 32 then 7 else 8 fi
	a$ = a$ + a$ + a$ + a$ + a$ + a$ + a$ + a$ + a$ + a$
endfor
t = stopwatch
printline creating: 't:3' seconds
stopwatch
a$ > kanweg.txt
t = stopwatch
printline writing: 't:3' seconds
stopwatch
b$ < kanweg.txt
t = stopwatch
printline reading: 't:3' seconds
stopwatch
assert a$ = b$
t = stopwatch
printline comparing: 't:3' seconds
deleteFile ("kanweg.txt")