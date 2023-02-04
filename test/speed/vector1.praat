writeInfoLine: "runScript vector..."

size = 1e6
numberOfChecks = 10

vector# = zero# (size)
vector# ~ col

stopwatch
for n from 1 to numberOfChecks
	runScript: "../manually/vector2.praat", vector#
endfor

appendInfoLine: "time ", stopwatch / numberOfChecks / size * 1e9, " ns"

appendInfoLine: "OK"
