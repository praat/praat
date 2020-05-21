/* TextGrid_Sound.cpp
 *
 * Copyright (C) 1992-2019 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TextGrid_Sound.h"
#include "Pitch_to_PitchTier.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "LongSound.h"

static bool IntervalTier_check (IntervalTier me) {
	for (integer iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
		TextInterval interval = my intervals.at [iinterval];
		if (interval -> xmin >= interval -> xmax) {
			Melder_casual (U"Interval ", iinterval, U" starts at ", interval -> xmin,
				U" but ends at ", interval -> xmax, U" seconds.");
			return false;
		}
	}
	if (my intervals.size < 2) return true;
	for (integer iinterval = 1; iinterval < my intervals.size; iinterval ++) {
		TextInterval thisInterval = my intervals.at [iinterval];
		TextInterval nextInterval = my intervals.at [iinterval + 1];
		if (thisInterval -> xmax != nextInterval -> xmin) {
			Melder_casual (U"Interval ", iinterval, U" ends at ", thisInterval -> xmax,
				U" but the next interval starts at ", nextInterval -> xmin, U" seconds.");
			return false;
		}
	}
	return true;
}

static void IntervalTier_insertIntervalDestructively (IntervalTier me, double tmin, double tmax) {
	Melder_assert (tmin < tmax);
	Melder_assert (tmin >= my xmin);
	Melder_assert (tmax <= my xmax);
	/*
	 * Make sure that the tier has boundaries at the edges of the interval.
	 */
	integer firstIntervalNumber = IntervalTier_hasTime (me, tmin);
	if (! firstIntervalNumber) {
		integer intervalNumber = IntervalTier_timeToIndex (me, tmin);
		if (intervalNumber == 0)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (tmin, 6), U" seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = my intervals.at [intervalNumber];
		/*
		 * Move the text to the left of the boundary.
		 */
		autoTextInterval newInterval = TextInterval_create (tmin, interval -> xmax, U"");
		interval -> xmax = tmin;
		my intervals. addItem_move (newInterval.move());
		firstIntervalNumber = IntervalTier_hasTime (me, interval -> xmin);
	}
	Melder_assert (firstIntervalNumber >= 1 && firstIntervalNumber <= my intervals.size);
	integer lastIntervalNumber = IntervalTier_hasTime (me, tmax);
	if (! lastIntervalNumber) {
		integer intervalNumber = IntervalTier_timeToIndex (me, tmax);
		if (intervalNumber == 0)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (tmin, 6), U" seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = my intervals.at [intervalNumber];
		/*
		 * Move the text to the right of the boundary.
		 */
		autoTextInterval newInterval = TextInterval_create (interval -> xmin, tmax, U"");
		interval -> xmin = tmax;
		my intervals. addItem_move (newInterval.move());
		lastIntervalNumber = IntervalTier_hasTime (me, interval -> xmax);
	}
	Melder_assert (lastIntervalNumber >= 1 && lastIntervalNumber <= my intervals.size);
	/*
	 * Empty the interval in the word tier.
	 */
	trace (U"Empty interval ", lastIntervalNumber, U" down to ", U".", firstIntervalNumber);
	for (integer iinterval = lastIntervalNumber; iinterval >= firstIntervalNumber; iinterval --) {
		TextInterval interval = my intervals.at [iinterval];
		if (interval -> xmin > tmin && interval -> xmin < tmax) {
			Melder_assert (iinterval > 1);
			TextInterval previous = my intervals.at [iinterval - 1];
			previous -> xmax = tmax;   // collapse left and right intervals into left interval
			TextInterval_setText (previous, U"");
			my intervals. removeItem (iinterval);   // remove right interval
		}
		if (interval -> xmax == tmax) {
			TextInterval_setText (interval, U"");
		}
	}
}

static double IntervalTier_boundaryTimeClosestTo (IntervalTier me, double tmin, double tmax) {
	integer intervalNumber = IntervalTier_timeToLowIndex (me, tmax);
	if (intervalNumber != 0) {
		TextInterval interval = my intervals.at [intervalNumber];
		if (interval -> xmin > tmin && interval -> xmin < tmax) {
			return interval -> xmin;
		}
	}
	return 0.5 * (tmin + tmax);
}

static void IntervalTier_removeEmptyIntervals (IntervalTier me, IntervalTier boss) {
	IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (me, U"");
	if (my intervals.size < 2) return;
	TextInterval firstInterval = my intervals.at [1];
	if (Melder_equ (firstInterval -> text.get(), U"")) {
		IntervalTier_removeLeftBoundary (me, 2);
	}
	if (my intervals.size < 2) return;
	TextInterval lastInterval = my intervals.at [my intervals.size];
	if (Melder_equ (lastInterval -> text.get(), U"")) {
		IntervalTier_removeLeftBoundary (me, my intervals.size);
	}
	if (my intervals.size < 3) return;
	for (integer iinterval = my intervals.size - 1; iinterval >= 2; iinterval --) {
		TextInterval interval = my intervals.at [iinterval];
		if (Melder_equ (interval -> text.get(), U"")) {
			/*
			 * Distribute the empty interval between its neigbours.
			 */
			double newBoundaryTime =
				boss ?
				IntervalTier_boundaryTimeClosestTo (boss, interval -> xmin, interval -> xmax) :
				0.5 * (interval -> xmin + interval -> xmax);
			TextInterval previous = my intervals.at [iinterval - 1];
			TextInterval next = my intervals.at [iinterval + 1];
			previous -> xmax = newBoundaryTime;
			next -> xmin = newBoundaryTime;
			my intervals. removeItem (iinterval);
		}
	}
}

void TextGrid_anySound_alignInterval (TextGrid me, Function anySound, integer tierNumber, integer intervalNumber, conststring32 languageName, bool includeWords, bool includePhonemes) {
	try {
		IntervalTier headTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		if (intervalNumber < 1 || intervalNumber > headTier -> intervals.size)
			Melder_throw (U"Interval ", intervalNumber, U" does not exist.");
		TextInterval interval = headTier -> intervals.at [intervalNumber];
		if (! includeWords && ! includePhonemes)
			Melder_throw (U"Nothing to be done, because you asked neither for word alignment nor for phoneme alignment.");
		if (str32str (headTier -> name.get(), U"/") )
			Melder_throw (U"The current tier already has a slash (\"/\") in its name. Cannot create a word or phoneme tier from it.");
		autoSound part =
			anySound -> classInfo == classLongSound ? 
				LongSound_extractPart (static_cast <LongSound> (anySound), interval -> xmin, interval -> xmax, true) :
				Sound_extractPart (static_cast <Sound> (anySound), interval -> xmin, interval -> xmax, kSound_windowShape::RECTANGULAR, 1.0, true);
		autoSpeechSynthesizer synthesizer = SpeechSynthesizer_create (languageName, U"Female1");
		synthesizer -> d_samplingFrequency = round (
			anySound -> classInfo == classLongSound ?
				static_cast <LongSound> (anySound) -> sampleRate :
				1.0 / static_cast <Sound> (anySound) -> dx
		);
		double silenceThreshold = -30.0, minSilenceDuration = 0.1, minSoundingDuration = 0.1;
		autoTextGrid analysis;
		if (! Melder_equ (interval -> text.get(), U"")) {
			analysis = SpeechSynthesizer_Sound_TextInterval_align
				(synthesizer.get(), part.get(), interval, silenceThreshold, minSilenceDuration, minSoundingDuration);
		}
		if (analysis) {
			/*
			 * Clean up the analysis.
			 */
			Melder_assert (fabs (analysis -> xmin - interval -> xmin) < 1e-12);
			if (analysis -> xmax != interval -> xmax) {
				//Melder_fatal (U"Analysis ends at ", analysis -> xmax, U" but interval at ", interval -> xmax, U"seconds.");
				analysis -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (1) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (2) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (3) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (4) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (1) -> intervals.at [analysis -> intervalTier_cast (1) -> intervals.size] -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (2) -> intervals.at [analysis -> intervalTier_cast (2) -> intervals.size] -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (3) -> intervals.at [analysis -> intervalTier_cast (3) -> intervals.size] -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (4) -> intervals.at [analysis -> intervalTier_cast (4) -> intervals.size] -> xmax = interval -> xmax;
			}
			Melder_assert (analysis -> tiers->size == 4);
			IntervalTier analysisWordTier = analysis -> intervalTier_cast (3);
			if (! IntervalTier_check (analysisWordTier))
				Melder_throw (U"Analysis word tier out of order.");
			IntervalTier_removeEmptyIntervals (analysisWordTier, nullptr);
			Melder_assert (analysisWordTier -> xmax == analysis -> xmax);
			Melder_assert (analysisWordTier -> intervals.size >= 1);
			TextInterval firstInterval = analysisWordTier -> intervals.at [1];
			TextInterval lastInterval = analysisWordTier -> intervals.at [analysisWordTier -> intervals.size];
			firstInterval -> xmin = analysis -> xmin;
			lastInterval  -> xmax = analysis -> xmax;
			if (lastInterval -> xmax != analysis -> xmax)
				Melder_fatal (U"analysis ends at ", analysis -> xmax, U", but last interval at ", lastInterval -> xmax, U" seconds");
			if (! IntervalTier_check (analysisWordTier))
				Melder_throw (U"Analysis word tier out of order (2).");
			IntervalTier analysisPhonemeTier = analysis -> intervalTier_cast (4);
			if (! IntervalTier_check (analysisPhonemeTier))
				Melder_throw (U"Analysis phoneme tier out of order.");
			IntervalTier_removeEmptyIntervals (analysisPhonemeTier, analysisWordTier);
			Melder_assert (analysisPhonemeTier -> xmax == analysis -> xmax);
			Melder_assert (analysisPhonemeTier -> intervals.size >= 1);
			firstInterval = analysisPhonemeTier -> intervals.at [1];
			lastInterval  = analysisPhonemeTier -> intervals.at [analysisPhonemeTier -> intervals.size];
			firstInterval -> xmin = analysis -> xmin;
			lastInterval  -> xmax = analysis -> xmax;
			Melder_assert (lastInterval -> xmax == analysis -> xmax);
			if (! IntervalTier_check (analysisPhonemeTier))
				Melder_throw (U"Analysis phoneme tier out of order (2).");
		}
		integer wordTierNumber = 0, phonemeTierNumber = 0;
		IntervalTier wordTier = nullptr, phonemeTier = nullptr;
		/*
		 * Include a word tier.
		 */
		if (includeWords) {
			/*
			 * Make sure that the word tier exists.
			 */
			autoMelderString newWordTierName;
			MelderString_copy (& newWordTierName, headTier -> name.get(), U"/word");
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				Function tier = my tiers->at [itier];
				if (Melder_equ (newWordTierName.string, tier -> name.get())) {
					if (tier -> classInfo != classIntervalTier)
						Melder_throw (U"A tier with the prospective word tier name (", tier -> name.get(), U") already exists, but it is not an interval tier."
							U"\nPlease change its name or remove it.");
					wordTierNumber = itier;
					break;
				}
			}
			if (! wordTierNumber) {
				autoIntervalTier newWordTier = IntervalTier_create (my xmin, my xmax);
				Thing_setName (newWordTier.get(), newWordTierName.string);
				my tiers -> addItemAtPosition_move (newWordTier.move(), wordTierNumber = tierNumber + 1);
			}
			Melder_assert (wordTierNumber >= 1 && wordTierNumber <= my tiers->size);
			wordTier = static_cast <IntervalTier> (my tiers->at [wordTierNumber]);
			/*
			 * Make sure that the word tier has boundaries at the edges of the interval.
			 */
			IntervalTier_insertIntervalDestructively (wordTier, interval -> xmin, interval -> xmax);
			/*
			 * Copy the contents of the word analysis into the interval in the word tier.
			 */
			integer wordIntervalNumber = IntervalTier_hasTime (wordTier, interval -> xmin);
			Melder_assert (wordIntervalNumber != 0);
			if (analysis) {
				IntervalTier analysisWordTier = analysis -> intervalTier_cast (3);
				if (! IntervalTier_check (analysisWordTier))
					Melder_throw (U"Analysis word tier out of order (3).");
				if (! IntervalTier_check (wordTier))
					Melder_throw (U"Word tier out of order (3).");
				for (integer ianalysisInterval = 1; ianalysisInterval <= analysisWordTier -> intervals.size; ianalysisInterval ++) {
					TextInterval analysisInterval = analysisWordTier -> intervals.at [ianalysisInterval];
					TextInterval wordInterval = nullptr;
					double tmin = analysisInterval -> xmin, tmax = analysisInterval -> xmax;
					if (tmax == analysis -> xmax) {
						wordInterval = wordTier -> intervals.at [wordIntervalNumber];
						TextInterval_setText (wordInterval, analysisInterval -> text.get());
					} else {
						wordInterval = wordTier -> intervals.at [wordIntervalNumber];
						autoTextInterval newInterval = TextInterval_create (tmin, tmax, analysisInterval -> text.get());
						wordInterval -> xmin = tmax;
						wordTier -> intervals. addItem_move (newInterval.move());
						wordIntervalNumber ++;
					}
				}
				if (! IntervalTier_check (analysisWordTier))
					Melder_throw (U"Analysis word tier out of order (4).");
				if (! IntervalTier_check (wordTier))
					Melder_throw (U"Word tier out of order (4).");
			}
		}
		/*
		 * Include a phoneme tier.
		 */
		if (includePhonemes) {
			/*
			 * Make sure that the phoneme tier exists.
			 */
			autoMelderString newPhonemeTierName;
			MelderString_copy (& newPhonemeTierName, headTier -> name.get(), U"/phon");
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				Function tier = my tiers->at [itier];
				if (Melder_equ (newPhonemeTierName.string, tier -> name.get())) {
					if (tier -> classInfo != classIntervalTier)
						Melder_throw (U"A tier with the prospective phoneme tier name (", tier -> name.get(), U") already exists, but it is not an interval tier."
							U"\nPlease change its name or remove it.");
					phonemeTierNumber = itier;
					break;
				}
			}
			if (! phonemeTierNumber) {
				autoIntervalTier newPhonemeTier = IntervalTier_create (my xmin, my xmax);
				Thing_setName (newPhonemeTier.get(), newPhonemeTierName.string);
				my tiers -> addItemAtPosition_move (newPhonemeTier.move(),
					phonemeTierNumber = wordTierNumber ? wordTierNumber + 1 : tierNumber + 1);
			}
			Melder_assert (phonemeTierNumber >= 1 && phonemeTierNumber <= my tiers->size);
			phonemeTier = my intervalTier_cast (phonemeTierNumber);
			/*
			 * Make sure that the phoneme tier has boundaries at the edges of the interval.
			 */
			IntervalTier_insertIntervalDestructively (phonemeTier, interval -> xmin, interval -> xmax);
			/*
			 * Copy the contents of the phoneme analysis into the interval in the phoneme tier.
			 */
			integer phonemeIntervalNumber = IntervalTier_hasTime (phonemeTier, interval -> xmin);
			Melder_assert (phonemeIntervalNumber != 0);
			if (analysis) {
				IntervalTier analysisPhonemeTier = analysis -> intervalTier_cast (4);
				for (integer ianalysisInterval = 1; ianalysisInterval <= analysisPhonemeTier -> intervals.size; ianalysisInterval ++) {
					TextInterval analysisInterval = analysisPhonemeTier -> intervals.at [ianalysisInterval];
					TextInterval phonemeInterval = nullptr;
					double tmin = analysisInterval -> xmin, tmax = analysisInterval -> xmax;
					if (tmax == analysis -> xmax) {
						phonemeInterval = phonemeTier -> intervals.at [phonemeIntervalNumber];
						TextInterval_setText (phonemeInterval, analysisInterval -> text.get());
					} else {
						phonemeInterval = phonemeTier -> intervals.at [phonemeIntervalNumber];
						autoTextInterval newInterval = TextInterval_create (tmin, tmax, analysisInterval -> text.get());
						phonemeInterval -> xmin = tmax;
						phonemeTier -> intervals. addItem_move (newInterval.move());
						phonemeIntervalNumber ++;
					}
				}
			}
			if (includeWords) {
				/*
				 * Synchronize the boundaries between the word tier and the phoneme tier.
				 */
				//for (integer iinterval = 1; iinterval <=
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", anySound, U": interval not aligned.");
	}
}

void TextGrid_Sound_draw (TextGrid me, Sound sound, Graphics g, double tmin, double tmax,
	bool showBoundaries, bool useTextStyles, bool garnish) {
	integer numberOfTiers = my tiers ->size;

	Function_unidirectionalAutowindow (me, & tmin, & tmax);

	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, -1.0 - 0.5 * numberOfTiers, 1.0);

	/*
	 * Draw sound in upper part.
	 */
	integer first, last;
	if (sound && Sampled_getWindowSamples (sound, tmin, tmax, & first, & last) > 1) {
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, tmin, 0.0, tmax, 0.0);
		Graphics_setLineType (g, Graphics_DRAWN);      
		Graphics_function (g, & sound -> z [1] [0], first, last,
			Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
	}

	/*
	 * Draw labels in lower part.
	 */
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setPercentSignIsItalic (g, useTextStyles);
	Graphics_setNumberSignIsBold (g, useTextStyles);
	Graphics_setCircumflexIsSuperscript (g, useTextStyles);
	Graphics_setUnderscoreIsSubscript (g, useTextStyles);
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		Function anyTier = my tiers->at [itier];
		double ymin = -1.0 - 0.5 * itier, ymax = ymin + 0.5;
		Graphics_rectangle (g, tmin, tmax, ymin, ymax);
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			integer ninterval = tier -> intervals.size;
			for (integer iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals.at [iinterval];
				double intmin = interval -> xmin, intmax = interval -> xmax;
				if (intmin < tmin) intmin = tmin;
				if (intmax > tmax) intmax = tmax;
				if (intmin >= intmax) continue;
				if (showBoundaries && intmin > tmin && intmin < tmax) {
					Graphics_setLineType (g, Graphics_DOTTED);
					Graphics_line (g, intmin, -1.0, intmin, 1.0);   // in sound part
					Graphics_setLineType (g, Graphics_DRAWN);
				}      
				/* Draw left boundary. */
				if (intmin > tmin && intmin < tmax) Graphics_line (g, intmin, ymin, intmin, ymax);
				/* Draw label text. */
				if (interval -> text && intmax >= tmin && intmin <= tmax) {
					double t1 = tmin > intmin ? tmin : intmin;
					double t2 = tmax < intmax ? tmax : intmax;
					Graphics_text (g, 0.5 * (t1 + t2), 0.5 * (ymin + ymax), interval -> text.get());
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			integer numberOfPoints = tier -> points.size;
			for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
				TextPoint point = tier -> points.at [ipoint];
				double t = point -> number;
				if (t > tmin && t < tmax) {
					if (showBoundaries) {
						Graphics_setLineType (g, Graphics_DOTTED);
						Graphics_line (g, t, -1.0, t, 1.0);   // in sound part
						Graphics_setLineType (g, Graphics_DRAWN);
					}
					Graphics_line (g, t, ymin, t, 0.8 * ymin + 0.2 * ymax);
					Graphics_line (g, t, 0.2 * ymin + 0.8 * ymax, t, ymax);
					if (point -> mark)
						Graphics_text (g, t, 0.5 * (ymin + ymax), point -> mark.get());
				}
			}
		}
	}
	Graphics_setPercentSignIsItalic (g, true);
	Graphics_setNumberSignIsBold (g, true);
	Graphics_setCircumflexIsSuperscript (g, true);
	Graphics_setUnderscoreIsSubscript (g, true);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

autoSoundList TextGrid_Sound_extractAllIntervals (TextGrid me, Sound sound, integer tierNumber, bool preserveTimes) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoSoundList list = SoundList_create ();
		for (integer iseg = 1; iseg <= tier -> intervals.size; iseg ++) {
			TextInterval segment = tier -> intervals.at [iseg];
			autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
			Thing_setName (interval.get(), segment -> text ? segment -> text.get() : U"untitled");
			list -> addItem_move (interval.move());
		}
		return list;
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": intervals not extracted.");
	}
}

autoSoundList TextGrid_Sound_extractNonemptyIntervals (TextGrid me, Sound sound, integer tierNumber, bool preserveTimes) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoSoundList list = SoundList_create ();
		for (integer iseg = 1; iseg <= tier -> intervals.size; iseg ++) {
			TextInterval segment = tier -> intervals.at [iseg];
			if (segment -> text && segment -> text [0] != U'\0') {
				autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
				Thing_setName (interval.get(), segment -> text ? segment -> text.get() : U"untitled");
				list -> addItem_move (interval.move());
			}
		}
		if (list->size == 0) Melder_warning (U"No non-empty intervals were found.");
		return list;
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": non-empty intervals not extracted.");
	}
}

autoSoundList TextGrid_Sound_extractIntervalsWhere (TextGrid me, Sound sound, integer tierNumber,
	kMelder_string which, conststring32 text, bool preserveTimes)
{
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoSoundList list = SoundList_create ();
		integer count = 0;
		for (integer iseg = 1; iseg <= tier -> intervals.size; iseg ++) {
			TextInterval segment = tier -> intervals.at [iseg];
			if (Melder_stringMatchesCriterion (segment -> text.get(), which, text, true)) {
				autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
				Thing_setName (interval.get(), Melder_cat (sound -> name ? sound -> name.get() : U"", U"_", text, U"_", ++ count));
				list -> addItem_move (interval.move());
			}
		}
		if (list->size == 0)
			Melder_warning (U"No label that ", kMelder_string_getText (which), U" the text \"", text, U"\" was found.");
		return list;
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": intervals not extracted.");
	}
}

static void autoMarks (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	double dy = ymax - ymin;
	if (dy < 26.0) {
		integer imin = Melder_iroundUp ((ymin + 2.0) / 5.0), imax = Melder_ifloor ((ymax - 2.0) / 5.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 5.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 110.0) {
		integer imin = Melder_iroundUp ((ymin + 8.0) / 20.0), imax = Melder_ifloor ((ymax - 8.0) / 20.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 20.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 260.0) {
		integer imin = Melder_iroundUp ((ymin + 20.0) / 50.0), imax = Melder_ifloor ((ymax - 20.0) / 50.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 50.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 510.0) {
		integer imin = Melder_iroundUp ((ymin + 40.0) / 100.0), imax = Melder_ifloor ((ymax - 40.0) / 100.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 100.0, true, true, haveDottedLines, nullptr);
	}
}

static void autoMarks_logarithmic (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	double fy = ymax / ymin;
	for (int i = -12; i <= 12; i ++) {
		double power = pow (10, i), y = power;
		if (y > ymin * 1.2 && y < ymax / 1.2)
			Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
		if (fy > 2100) {
			;   /* Enough. */
		} else if (fy > 210) {
			y = 3.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
		} else {
			y = 2.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			y = 5.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			if (fy < 21) {
				y = 3.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
				y = 7.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			}
			if (fy < 4.1) {
				y = 1.5 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
				y = 4.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			}
		}
	}
}

static void autoMarks_semitones (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	double dy = ymax - ymin;
	if (dy < 16.0) {
		integer imin = Melder_iroundUp ((ymin + 1.2) / 3.0), imax = Melder_ifloor ((ymax - 1.2) / 3.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 3.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 32.0) {
		integer imin = Melder_iroundUp ((ymin + 2.4) / 6.0), imax = Melder_ifloor ((ymax - 2.4) / 6.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 6.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 64.0) {
		integer imin = Melder_iroundUp ((ymin + 4.8) / 12.0), imax = Melder_ifloor ((ymax - 4.8) / 12.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 12.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 128.0) {
		integer imin = Melder_iroundUp ((ymin + 9.6) / 24.0), imax = Melder_ifloor ((ymax - 9.6) / 24.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 24.0, true, true, haveDottedLines, nullptr);
	}
}

void TextGrid_Pitch_drawSeparately (TextGrid grid, Pitch pitch, Graphics g, double tmin, double tmax,
	double fmin, double fmax, bool showBoundaries, bool useTextStyles, bool garnish, bool speckle, kPitch_unit unit)
{
	integer numberOfTiers = grid -> tiers->size;
	Function_unidirectionalAutowindow (grid, & tmin, & tmax);
	if (Function_isUnitLogarithmic (pitch, Pitch_LEVEL_FREQUENCY, (int) unit)) {
		fmin = Function_convertStandardToSpecialUnit (pitch, fmin, Pitch_LEVEL_FREQUENCY, (int) unit);
		fmax = Function_convertStandardToSpecialUnit (pitch, fmax, Pitch_LEVEL_FREQUENCY, (int) unit);
	}
	if (unit == kPitch_unit::HERTZ_LOGARITHMIC)
		Pitch_draw (pitch, g, tmin, tmax, pow (10.0, fmin - 0.25 * (fmax - fmin) * numberOfTiers), pow (10.0, fmax), false, speckle, unit);
	else
		Pitch_draw (pitch, g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * numberOfTiers, fmax, false, speckle, unit);
	TextGrid_Sound_draw (grid, nullptr, g, tmin, tmax, showBoundaries, useTextStyles, false);
	/*
	 * Restore window for the sake of margin drawing.
	 */
	Graphics_setWindow (g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * numberOfTiers, fmax);
	if (unit == kPitch_unit::HERTZ_LOGARITHMIC)
		fmin = pow (10, fmin), fmax = pow (10.0, fmax);
	if (garnish) {
		Graphics_drawInnerBox (g);
		if (unit == kPitch_unit::HERTZ_LOGARITHMIC) {
			Graphics_markLeftLogarithmic (g, fmin, true, true, false, nullptr);
			Graphics_markLeftLogarithmic (g, fmax, true, true, false, nullptr);
			autoMarks_logarithmic (g, fmin, fmax, false);
		} else if (unit == kPitch_unit::SEMITONES_100) {
			Graphics_markLeft (g, fmin, true, true, false, nullptr);
			Graphics_markLeft (g, fmax, true, true, false, nullptr);
			autoMarks_semitones (g, fmin, fmax, false);
		} else {
			Graphics_markLeft (g, fmin, true, true, false, nullptr);
			Graphics_markLeft (g, fmax, true, true, false, nullptr);
			autoMarks (g, fmin, fmax, false);
		}
		Graphics_textLeft (g, true, Melder_cat (U"Pitch (", Function_getUnitText (pitch, Pitch_LEVEL_FREQUENCY, (int) unit, Function_UNIT_TEXT_GRAPHICAL), U")"));
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

void TextGrid_Pitch_draw (TextGrid grid, Pitch pitch, Graphics g,
	integer tierNumber, double tmin, double tmax, double fmin, double fmax,
	double fontSize, bool useTextStyles, int horizontalAlignment, bool garnish, bool speckle, kPitch_unit unit)
{
	try {
		Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (grid, tierNumber);
		const double oldFontSize = Graphics_inqFontSize (g);
		Pitch_draw (pitch, g, tmin, tmax, fmin, fmax, garnish, speckle, unit);
		Function_unidirectionalAutowindow (grid, & tmin, & tmax);
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch);
		if (Function_isUnitLogarithmic (pitch, Pitch_LEVEL_FREQUENCY, (int) unit)) {
			fmin = Function_convertStandardToSpecialUnit (pitch, fmin, Pitch_LEVEL_FREQUENCY, (int) unit);
			fmax = Function_convertStandardToSpecialUnit (pitch, fmax, Pitch_LEVEL_FREQUENCY, (int) unit);
		}
		Graphics_setTextAlignment (g, (kGraphics_horizontalAlignment) horizontalAlignment, Graphics_BOTTOM);
		Graphics_setInner (g);
		Graphics_setFontSize (g, fontSize);
		Graphics_setPercentSignIsItalic (g, useTextStyles);
		Graphics_setNumberSignIsBold (g, useTextStyles);
		Graphics_setCircumflexIsSuperscript (g, useTextStyles);
		Graphics_setUnderscoreIsSubscript (g, useTextStyles);
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (integer i = 1; i <= tier -> intervals.size; i ++) {
				TextInterval interval = tier -> intervals.at [i];
				double tleft = interval -> xmin, tright = interval -> xmax, tmid, f0;
				if (! interval -> text || ! interval -> text [0]) continue;
				if (tleft < pitch -> xmin) tleft = pitch -> xmin;
				if (tright > pitch -> xmax) tright = pitch -> xmax;
				tmid = (tleft + tright) / 2;
				if (tmid < tmin || tmid > tmax) continue;
				f0 = Function_convertStandardToSpecialUnit (pitch, RealTier_getValueAtTime (pitchTier.get(), tmid), Pitch_LEVEL_FREQUENCY, (int) unit);
				if (f0 < fmin || f0 > fmax) continue;
				Graphics_text (g,
					horizontalAlignment == (int) Graphics_LEFT ? tleft : horizontalAlignment == (int) Graphics_RIGHT ? tright : tmid,
					f0, interval -> text.get());
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (integer i = 1; i <= tier -> points.size; i ++) {
				TextPoint point = tier -> points.at [i];
				double t = point -> number;
				if (! point -> mark || ! point -> mark [0]) continue;
				if (t < tmin || t > tmax) continue;
				double f0 = Function_convertStandardToSpecialUnit (pitch, RealTier_getValueAtTime (pitchTier.get(), t), Pitch_LEVEL_FREQUENCY, (int) unit);
				if (f0 < fmin || f0 > fmax) continue;
				Graphics_text (g, t, f0, point -> mark.get());
			}
		}
		Graphics_setPercentSignIsItalic (g, true);
		Graphics_setNumberSignIsBold (g, true);
		Graphics_setCircumflexIsSuperscript (g, true);
		Graphics_setUnderscoreIsSubscript (g, true);
		Graphics_setFontSize (g, oldFontSize);
		Graphics_unsetInner (g);
	} catch (MelderError) {
		Melder_throw (grid, U" & ", pitch, U": not drawn.");
	}
}

/* End of file TextGrid_Sound.cpp */
