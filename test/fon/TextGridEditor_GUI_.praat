sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"
textgrid = To TextGrid: "sentences words bell", "bell"
plusObject: sound
View & Edit
editor: textgrid
	a$ = Editor info
	writeInfoLine: a$
	b$ = Sound info
	appendInfoLine: b$
	c$ = info$ ()
	assert index (c$, "Editor type: TextGridEditor" + newline$)
	assert index (c$, "Editor name: ")
	assert index (c$, ". TextGrid sineWithNoise" + newline$)
	assert index (c$, "Editor end: 1 seconds" + newline$)
	assert index (c$, "Selection start: 0.5 seconds" + newline$)
	assert index (c$, "Selection end: 0.5 seconds" + newline$)
	assert index (c$, "Pitch (raw AC and CC) octave jump cost: ")
	assert index (c$, " per octave" + newline$)
	assert index (c$, "Number of channels: 1 (mono)" + newline$)
	assert index (c$, "Time domain:" + newline$)
	assert index (c$, "Object type: Sound" + newline$)
	assert index (c$, "Object name: sineWithNoise" + newline$)
	assert index (c$, "   End time: 1 seconds" + newline$)
	assert index (c$, "   Number of samples: 44100" + newline$)
	assert index (c$, " Joule/m²)" + newline$)
	assert index (c$, " Watt/m² = ")

	#
	# FunctionEditor properties.
	#
	a = Get start of selection
	assert a = 0.5
	Move cursor to: 0.567
	a = Get cursor
	assert a = 0.567

	#
	# SoundAnalysisArea properties.
	#

	#
	# Save old settings.
	#
	showSpectrogram = extractNumber (c$, "Spectrogram show: ")
	showPitch = extractNumber (c$, "Pitch show: ")
	showIntensity = extractNumber (c$, "Intensity show: ")
	showFormants = extractNumber (c$, "Formant show: ")
	showPulses = extractNumber (c$, "Pulses show: ")
	longestAnalysis = extractNumber (c$, "Longest analysis: ")
	assert longestAnalysis > 0.0
	#
	# Show everything.
	#
	Show analyses: "yes", "yes", "yes", "yes", "yes", 10.0
	#
	# Query spectral properties.
	#
	a = Get frequency at frequency cursor
	a = Get spectral power at cursor cross
	#
	# Query pitch properties.
	#
	asserterror The command "Pitch listing" returns a string; not assigned to the numeric variable "a".
	a = Pitch listing
	a = Get pitch
	asserterror Make a selection first.
	a = Get minimum pitch
	asserterror Make a selection first.
	a = Get maximum pitch
	#
	# Query intensity properties.
	#
	asserterror The command "Intensity listing" returns a string; not assigned to the numeric variable "a".
	a = Intensity listing
	a = Get intensity
	asserterror Make a selection first.
	a = Get minimum intensity
	asserterror Make a selection first.
	a = Get maximum intensity
	#
	# Query pulses properties.
	#
	asserterror Make a selection first.
	report$ = Voice report
	Select: 0.5, 0.567
	report$ = Voice report
	writeInfoLine: "Voice report>>>", report$, "<<<Voice report"
	assert index (report$, "   Jitter (local): ")
	listing$ = Pulse listing
	assert index (listing$, "Time_s")
	#
	# Query formant properties.
	#
	f1 = Get first formant
	assert f1 > 0.0
	b4 = Get bandwidth: 4
	assert b4 > 0.0

	#
	# Restore old settings.
	#
	Show analyses: showSpectrogram, showPitch, showIntensity, showFormants, showPulses, longestAnalysis

	#
	# Query TextGrid properties.
	#
	Move cursor to: 0.3
	Add on selected tier
	t2 = Get starting point of interval
	assert t2 = 0.3
	t3 = Get end point of interval
	assert t3 = 1.0

	Close
endeditor
selectObject: textgrid
Set interval text: 1, 1, "This is the first interval"
Set interval text: 1, 2, "This is the second interval"
plusObject: sound
View & Edit
editor: textgrid
	Move cursor to: 0.1
	text1$ = Get label of interval
	assert text1$ = "This is the first interval"
	Move cursor to: 0.9
	text2$ = Get label of interval
	assert text2$ = "This is the second interval"
	Close
endeditor

Remove
