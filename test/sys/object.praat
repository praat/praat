writeInfoLine: "object[]..."
nsamp = 44100
sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, nsamp, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
niter = 1e3
stopwatch

procedure do (formula$)
	for i to niter
		Formula: formula$
	endfor
	appendInfoLine: fixed$ (1e9 * stopwatch / niter / nsamp, 1), " ns: ", formula$
endproc

@do: "0"
@do: "5"
@do: "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
@do: "self + 5"
@do: "Sound_sineWithNoise[] + 5"
@do: "Object_'sound'[] + 5"
@do: "self"
@do: "self [col]"
@do: "self [col] + 5"
@do: "Sound_sineWithNoise [col] + 5"
@do: "Object_'sound' [col] + 5"
@do: "object [sound, col] + 5"
@do: "self"
@do: "Sound_sineWithNoise [row, col] + 5"
@do: "Object_'sound' [row, col] + 5"
@do: "self [row, col] + 5"
@do: "object [sound, row, col] + 5"
@do: "object [""Sound sineWithNoise"", row, col] + 5"
name$ = "Sound sineWithNoise"
@do: "object [name$, row, col] + 5"
appendInfoLine: "OK"
Remove