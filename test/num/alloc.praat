writeInfoLine: "alloc versus zero"

numberOfChecks = 100
for n from 1 to numberOfChecks
	result$ = Praat test: "TimeAlloc", string$ (10^7 / n), string$ (n), "", ""
	durationAlloc = extractNumber (result$, "")
	result$ = Praat test: "TimeAlloc0", string$ (10^7 / n), string$ (n), "", ""
	durationAlloc0 = extractNumber (result$, "")
	result$ = Praat test: "TimeZero", string$ (10^7 / n), string$ (n), "", ""
	durationZero = extractNumber (result$, newline$)
	result$ = Praat test: "TimeMalloc", string$ (10^7 / n), string$ (n), "", ""
	durationMalloc = extractNumber (result$, newline$)
	result$ = Praat test: "TimeCalloc", string$ (10^7 / n), string$ (n), "", ""
	durationCalloc = extractNumber (result$, newline$)
	appendInfoLine (n, " ", durationAlloc, " ", durationAlloc0, " ", durationZero, " ", durationMalloc, " ", durationCalloc)
endfor
for i from 1 to 100
	n = i*100
	result$ = Praat test: "TimeAlloc", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationAlloc = extractNumber (result$, "")
	result$ = Praat test: "TimeAlloc0", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationAlloc0 = extractNumber (result$, "")
	result$ = Praat test: "TimeZero", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationZero = extractNumber (result$, newline$)
	result$ = Praat test: "TimeMalloc", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationMalloc = extractNumber (result$, newline$)
	result$ = Praat test: "TimeCalloc", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationCalloc = extractNumber (result$, newline$)
	appendInfoLine (n, " ", durationAlloc, " ", durationAlloc0, " ", durationZero, " ", durationMalloc, " ", durationCalloc)
endfor
for i from 1 to 100
	n = i*10000
	result$ = Praat test: "TimeAlloc", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationAlloc = extractNumber (result$, "")
	result$ = Praat test: "TimeAlloc0", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationAlloc0 = extractNumber (result$, "")
	result$ = Praat test: "TimeZero", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationZero = extractNumber (result$, newline$)
	result$ = Praat test: "TimeMalloc", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationMalloc = extractNumber (result$, newline$)
	result$ = Praat test: "TimeCalloc", string$ (max (10^7 / n, 1)), string$ (n), "", ""
	durationCalloc = extractNumber (result$, newline$)
	appendInfoLine (n, " ", durationAlloc, " ", durationAlloc0, " ", durationZero, " ", durationMalloc, " ", durationCalloc)
endfor
