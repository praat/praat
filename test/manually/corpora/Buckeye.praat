# Buckeye.praat
# Paul Boersma, 3 May 2025

#
# This script cycles through all the sound files and annotations of the Buckeye corpus,
# listing the following:
#
# - duration of the Sound
# - duration of the TextGrid
#

form: "Report on Buckeye corpus"
	folder: "Buckeye folder", "/Volumes/Buckeye"
endform

corpusFolderPath$ = buckeye_folder$
writeInfoLine: "Reporting on the Buckeye folder “", corpusFolderPath$, "”..."

stopwatch
folderNames$# = folderNames$#: corpusFolderPath$ + "/s*"
numberOfFolders = size (folderNames$#)
assert numberOfFolders = 40
for ifolder to numberOfFolders
	folderName$ = folderNames$# [ifolder]
	folderPath$ = corpusFolderPath$ + "/" + folderName$
	appendInfoLine: folderPath$
	subfolderNames$# = folderNames$#: folderPath$ + "/*"
	numberOfSubfolders = size (subfolderNames$#)
	assert numberOfSubfolders >= 4
	assert numberOfSubfolders <= 11
	for isubfolder to numberOfSubfolders
		subfolderName$ = subfolderNames$# [isubfolder]
		subfolderPath$ = folderPath$ + "/" + subfolderName$
		appendInfo: subfolderPath$

		#
		# Read all the data.
		#
		soundFilePath$ = subfolderPath$ + "/" + subfolderName$ + ".wav"
		Read Sound with adjacent annotation files (Buckeye): soundFilePath$
		assert numberOfSelected () = 2
		assert selected$ (1) = "Sound " + subfolderName$
		assert selected$ (2) = "TextGrid " + subfolderName$

		#
		# Identify each of the two objects.
		#
		sound = selected ("Sound")
		textgrid = selected ("TextGrid")

		#
		# Run the "analyses."
		#
		selectObject: sound
		soundDuration = Get total duration
		selectObject: textgrid
		textgridDuration = Get total duration
		appendInfoLine: "   ", soundDuration, "   ", textgridDuration
		assert soundDuration - textgridDuration < 80.0
		if textgridDuration - soundDuration > 1.5
			appendInfoLine: "WARNING: the sound is ", textgridDuration - soundDuration, " seconds shorter than the annotation."
		endif

		#
		# Check grid structure.
		#
		numberOfTiers = Get number of tiers
		intendedTierNames$# = { "phon", "phon*", "words", "dict", "trans", "pos", "log" }
		assert size (intendedTierNames$#) = numberOfTiers
		for itier to numberOfTiers
			realizedTierName$ = Get tier name: itier
			assert realizedTierName$ = intendedTierNames$# [itier]
		endfor
		numberOfIntervalsOnTier1 = Get number of intervals: 1
		numberOfIntervalsOnTier2 = Get number of intervals: 2
		assert numberOfIntervalsOnTier1 = numberOfIntervalsOnTier2
		numberOfIntervalsOnTier3 = Get number of intervals: 3
		assert numberOfIntervalsOnTier3 < numberOfIntervalsOnTier1
		for itier from 4 to 6
			numberOfIntervals = Get number of intervals: itier
			assert numberOfIntervals = numberOfIntervalsOnTier3
		endfor

		#
		# Perform redundancy checks on the TextGrid.
		#
		# First check: tier 3 predicts tiers 1, 2, 4, 5, 6 and 7.
		#
		numberOfIntervalsOnTier3 = Get number of intervals: 3
		for iinterval to numberOfIntervalsOnTier3
			text3$ = Get label of interval: 3, iinterval
			if left$ (text3$) = "<"
				tmin = Get start time of interval: 3, iinterval
				tmax = Get end time of interval: 3, iinterval
				if startsWith (text3$, "<VOCNOISE") or startsWith (text3$, "<VOCNIOSE")
					@checkIntervalTexts: "VOCNOISE",  "", "U", "U", "null"
				elsif startsWith (text3$, "<SIL")
					@checkIntervalTexts: "SIL",       "", "S", "S", "null"
				elsif startsWith (text3$, "<NOISE") or startsWith (text3$, "<NOSIE-")
					@checkIntervalTexts: "NOISE",     "", "U", "U", "null"
				elsif startsWith (text3$, "<IVER>")
					@checkIntervalTexts: "IVER",      "", "S", "S", "null"
				elsif startsWith (text3$, "<IVER-") or startsWith (text3$, "<IVER_")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<CUTOFF") or startsWith (text3$, "<cutoff") or startsWith (text3$, "<CUTTOFF") or startsWith (text3$, "<CUTFF")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<EXCLUDE") or startsWith (text3$, "<EXCL-")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<EXT") or startsWith (text3$, "<EXt-")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<HES")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<Hes")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<LAUGH") or startsWith (text3$, "<LAUH-") or startsWith (text3$, "<LAUCH-") or startsWith (text3$, "<LAULGH-")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<UNKNOWN")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<ERROR")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<previ=")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				elsif startsWith (text3$, "<oney=")
					@checkIntervalTexts: "IVER",      "", "U", "U", "null"
				else
					exitScript: "Unknown text: ", text3$
				endif
			elsif left$ (text3$) = "{"
				if text3$ = "{B_TRANS}"
					@checkIntervalTexts: "{B_TRANS}", "", "B", "B", "null"
				elsif text3$ = "{E_TRANS}"
					@checkIntervalTexts: "{E_TRANS}", "", "E", "E", "null"
				else
					exitScript: "Unknown text: ", text3$
				endif
			endif
		endfor

		#
		# Look for combined intervals.
		#
		for itier from 1 to 6
			numberOfIntervals = Get number of intervals: itier
			for iinterval to numberOfIntervals
				tmin = Get start time of interval: itier, iinterval
				tmax = Get end time of interval: itier, iinterval
				text$ = Get label of interval: itier, iinterval
				if index (text$, "//")
					appendInfoLine: "WARNING: combined  text “", text$,
					... "” (between ", tmin, " and ", tmax, "), on tier ", itier, "."
				endif
			endfor
		endfor
		#
		# Clean up.
		#
		removeObject: sound, textgrid

	endfor
endfor
appendInfoLine: "Elapsed time: ", stopwatch, " seconds"

procedure checkIntervalTexts: .expectedText1$, .expectedText2$, .expectedText4$, .expectedText5$, .expectedText6$
	.realizedText4$ = Get label of interval: 4, iinterval
	.realizedText5$ = Get label of interval: 5, iinterval
	.realizedText6$ = Get label of interval: 6, iinterval
	if length (.realizedText4$) > 1 or not index (.expectedText4$, .realizedText4$)
		appendInfoLine: "WARNING: expected dict text “", .expectedText4$, "“, but found “", .realizedText4$,
		... "” (between ", tmin, " and ", tmax, "), for word ", text3$, "."
	endif
	if length (.realizedText5$) > 1 or not index (.expectedText5$, .realizedText5$)
		appendInfoLine: "WARNING: expected trans text “", .expectedText5$, "“, but found “", .realizedText5$,
		... "” (between ", tmin, " and ", tmax, "), for word ", text3$, "."
	endif
	if .realizedText6$ <> .expectedText6$
		appendInfoLine: "WARNING: expected pos text “", .expectedText6$, "“, but found “", .realizedText6$,
		... "” (between ", tmin, " and ", tmax, "), for word ", text3$, "."
	endif
endproc
