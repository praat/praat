sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
pitch = noprogress To Pitch: 0.0, 75.0, 600.0
View & Edit
editor: pitch
	info$ = Editor info
	assert index (info$, "Editor type: PitchEditor" + newline$)
	assert index (info$, "Editor name: ")
	assert index (info$, ". Pitch sineWithNoise" + newline$)
	assert index (info$, "Editor end: 1 seconds" + newline$)
	assert index (info$, "Selection start: 0.5 seconds" + newline$)   ; if this editor is the only open editor
	assert index (info$, "Selection end: 0.5 seconds" + newline$)
	info$ = Pitch info
	assert index (info$, "Object type: Pitch" + newline$)
	assert index (info$, "Object name: sineWithNoise" + newline$)
	assert index (info$, "Time domain:" + newline$)
	assert index (info$, "   Start time: 0 seconds" + newline$)
	assert index (info$, "   End time: 1 seconds" + newline$)
	assert index (info$, "   Number of frames: 97 (97 voiced)" + newline$) or index (info$, "   Number of frames: 96 (96 voiced)" + newline$)
	assert index (info$, "Ceiling at: 600 Hz")
	assert index (info$, "Estimated quantiles:" + newline$)
	assert index (info$, "Minimum 3")
	assert index (info$, "Standard deviation: ")
	assert index (info$, "Mean absolute slope: ")
	assert index (info$, "Average: 37")

	#
	# FunctionEditor properties.
	#
	a = Get start of selection
	assert a = 0.5
	Move cursor to: 0.567
	a = Get cursor
	assert a = 0.567

	#
	# Pitch properties.
	#
	Select: 0.3, 0.7
	f0 = Get pitch
	assert f0 > 375   ; 'f0'
	assert f0 < 379   ; 'f0'

	Close
endeditor
plusObject: sound
Remove
