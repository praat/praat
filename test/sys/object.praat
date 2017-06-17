writeInfoLine: "object[]..."
sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
n = 1e3
stopwatch

procedure do (formula$)
	for i to n
		Formula: formula$
	endfor
	appendInfoLine: fixed$ (stopwatch, 3), " ", formula$
endproc

@do: "self + 5"
@do: "Sound_sineWithNoise[] + 5"
@do: "Object_'sound'[] + 5"

@do: "self [col] + 5"
@do: "Sound_sineWithNoise [col] + 5"
@do: "Object_'sound' [col] + 5"
@do: "object [sound, col] + 5"

@do: "Sound_sineWithNoise [row, col] + 5"
@do: "Object_'sound' [row, col] + 5"
@do: "self [row, col] + 5"
@do: "object [sound, row, col] + 5"
@do: "object [""Sound sineWithNoise"", row, col] + 5"
name$ = "Sound sineWithNoise"
@do: "object [name$, row, col] + 5"

Remove