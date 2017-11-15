writeInfoLine: "object[]..."
nsamp = 44100
x.Sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, nsamp, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
assert object [x.Sound]. xmin = 0
assert object [x.Sound]. xmax = 1
assert object [x.Sound]. ncol = nsamp
assert object [x.Sound]. nx = nsamp
assert object [x.Sound]. dx = 1 / 44100
;assert object [x.Sound]. x1 = 0.5 / 44100
assert object [x.Sound]. ymin = 1
assert object [x.Sound]. ymax = 1
assert object [x.Sound]. nrow = 1
assert object [x.Sound]. ny = 1
assert object [x.Sound]. dy = 1
niter = 1e3
stopwatch

procedure do (formula$)
	for i to niter
		Formula: formula$
	endfor
	appendInfoLine: fixed$ (1e9 * stopwatch / niter / nsamp, 1), " ns: ", formula$
endproc

@do: ~ 0
@do: ~ 5
@do: ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
@do: ~ self + 5
@do: ~ Sound_sineWithNoise[] + 5
@do: ~ Object_'x.Sound'[] + 5
@do: ~ self
@do: ~ self [col]
@do: ~ self [col] + 5
@do: ~ Sound_sineWithNoise [col] + 5
@do: ~ Object_'x.Sound' [col] + 5
@do: "object [x.Sound, col] + 5"
;@do: ~ x.Sound [col] + 5
@do: ~ self
@do: "Sound_sineWithNoise [row, col] + 5"
@do: "Object_'x.Sound' [row, col] + 5"
@do: "self [row, col] + 5"
@do: "object [x.Sound, row, col] + 5"
;@do: ~ x.Sound [row, col] + 5
@do: "object [""Sound sineWithNoise"", row, col] + 5"
name$ = "Sound sineWithNoise"
@do: "object [name$, row, col] + 5"
appendInfoLine: "OK"
Remove