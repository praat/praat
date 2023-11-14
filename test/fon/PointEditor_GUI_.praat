sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
pulses = noprogress To PointProcess (periodic, cc): 75.0, 600.0
plusObject: sound
View & Edit
editor: pulses
	info$ = Editor info
	assert index (info$, "Editor type: PointEditor" + newline$)
	assert index (info$, "Editor name: ")
	assert index (info$, ". PointProcess sineWithNoise" + newline$)
	assert index (info$, "Editor end: 1 seconds" + newline$)
	assert index (info$, "Selection start: 0.5 seconds" + newline$)   ; if this editor is the only open editor
	assert index (info$, "Selection end: 0.5 seconds" + newline$)
	info$ = PointProcess info
	assert index (info$, "Object type: PointProcess" + newline$)
	assert index (info$, "Object name: sineWithNoise" + newline$)
	assert index (info$, "Time domain:" + newline$)
	assert index (info$, "   Start time: 0 seconds" + newline$)
	assert index (info$, "   End time: 1 seconds" + newline$)
	assert index (info$, "Number of times: 36")
	assert index (info$, "First time: ")
	assert index (info$, "   Number of periods: 36")
	assert index (info$, "   Jitter (local): 1.")
	info$ = Sound info
	assert index (info$, "Number of channels: 1 (mono)" + newline$)
	assert index (info$, "Time domain:" + newline$)
	assert index (info$, "Object type: Sound" + newline$)
	assert index (info$, "Object name: sineWithNoise" + newline$)
	assert index (info$, "   End time: 1 seconds" + newline$)
	assert index (info$, "   Number of samples: 44100" + newline$)
	assert index (info$, " Joule/m²)" + newline$)
	assert index (info$, " Watt/m² = ")

	#
	# FunctionEditor properties.
	#
	a = Get start of selection
	assert a = 0.5
	Move cursor to: 0.567
	a = Get cursor
	assert a = 0.567

	#
	# PointProcess properties.
	#
	Select: 0.3, 0.7
	jitter = Get jitter (local)
	assert jitter > 1%   ; 'jitter'
	assert jitter < 2%   ; 'jitter'
	shimmer = Get shimmer (local)
	assert shimmer > 4%   ; 'shimmer'
	assert shimmer < 8%   ; 'shimmer'

	Close
endeditor
Remove
