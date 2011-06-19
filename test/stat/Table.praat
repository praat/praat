for i to 100000
	d'i' = i
endfor
d43789 = Create formant table (Weenink 1985)
name$ = selected$ ("Table")
stopwatch
for cycle to 300
	for row to 360
		;z = Table_'name$' [row, "F0"]
		;z = Object_'d43789' [row, "F0"]
		z = object [d43789, row, "F0"]
		;d34789 = Get value... row F0
	endfor
endfor
t = stopwatch
echo 't:3' 'z'