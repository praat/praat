/* TextGrid_Sound.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2010/12/06 BDF/EDF files (for EEG)
 * pb 2010/12/08 split off from TextGrid.c and Sound.c
 * pb 2011/06/11 C++
 */

#include "TextGrid_Sound.h"
#include "Pitch_to_PitchTier.h"

void TextGrid_Sound_draw (TextGrid me, Sound sound, Graphics g, double tmin, double tmax,
	int showBoundaries, int useTextStyles, int garnish)   // STEREO BUG
{
	long numberOfTiers = my tiers -> size;

	/*
	 * Automatic windowing:
	 */
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;

	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, -1.0 - 0.5 * numberOfTiers, 1.0);

	/*
	 * Draw sound in upper part.
	 */
	long first, last;
	if (sound && Sampled_getWindowSamples (sound, tmin, tmax, & first, & last) > 1) {
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, tmin, 0.0, tmax, 0.0);
		Graphics_setLineType (g, Graphics_DRAWN);      
		Graphics_function (g, sound -> z [1], first, last,
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
	for (long itier = 1; itier <= numberOfTiers; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		double ymin = -1.0 - 0.5 * itier, ymax = ymin + 0.5;
		Graphics_rectangle (g, tmin, tmax, ymin, ymax);
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long ninterval = tier -> intervals -> size;
			for (long iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
				double intmin = interval -> xmin, intmax = interval -> xmax;
				if (intmin < tmin) intmin = tmin;
				if (intmax > tmax) intmax = tmax;
				if (intmin >= intmax) continue;
				if (showBoundaries && intmin > tmin && intmin < tmax) {
					Graphics_setLineType (g, Graphics_DOTTED);
					Graphics_line (g, intmin, -1.0, intmin, 1.0);   /* In sound part. */
					Graphics_setLineType (g, Graphics_DRAWN);
				}      
				/* Draw left boundary. */
				if (intmin > tmin && intmin < tmax) Graphics_line (g, intmin, ymin, intmin, ymax);
				/* Draw label text. */
				if (interval -> text && intmax >= tmin && intmin <= tmax) {
					double t1 = tmin > intmin ? tmin : intmin;
					double t2 = tmax < intmax ? tmax : intmax;
					Graphics_text (g, 0.5 * (t1 + t2), 0.5 * (ymin + ymax), interval -> text);
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long numberOfPoints = tier -> points -> size;
			for (long ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
				TextPoint point = (TextPoint) tier -> points -> item [ipoint];
				double t = point -> number;
				if (t > tmin && t < tmax) {
					if (showBoundaries) {
						Graphics_setLineType (g, Graphics_DOTTED);
						Graphics_line (g, t, -1.0, t, 1.0);   /* In sound part. */
						Graphics_setLineType (g, Graphics_DRAWN);
					}
					Graphics_line (g, t, ymin, t, 0.8 * ymin + 0.2 * ymax);
					Graphics_line (g, t, 0.2 * ymin + 0.8 * ymax, t, ymax);
					if (point -> mark)
						Graphics_text (g, t, 0.5 * (ymin + ymax), point -> mark);
				}
			}
		}
	}
	Graphics_setPercentSignIsItalic (g, TRUE);
	Graphics_setNumberSignIsBold (g, TRUE);
	Graphics_setCircumflexIsSuperscript (g, TRUE);
	Graphics_setUnderscoreIsSubscript (g, TRUE);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 1);
	}
}

Collection TextGrid_Sound_extractAllIntervals (TextGrid me, Sound sound, long tierNumber, int preserveTimes) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoCollection collection = Collection_create (NULL, tier -> intervals -> size);
		for (long iseg = 1; iseg <= tier -> intervals -> size; iseg ++) {
			TextInterval segment = (TextInterval) tier -> intervals -> item [iseg];
			autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes);
			Thing_setName (interval.peek(), segment -> text ? segment -> text : L"untitled");
			Collection_addItem (collection.peek(), interval.transfer()); 
		}
		return collection.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", sound, ": intervals not extracted.");
	}
}

Collection TextGrid_Sound_extractNonemptyIntervals (TextGrid me, Sound sound, long tierNumber, int preserveTimes) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoCollection collection = Collection_create (NULL, tier -> intervals -> size);
		for (long iseg = 1; iseg <= tier -> intervals -> size; iseg ++) {
			TextInterval segment = (TextInterval) tier -> intervals -> item [iseg];
			if (segment -> text != NULL && segment -> text [0] != '\0') {
				autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes);
				Thing_setName (interval.peek(), segment -> text ? segment -> text : L"untitled");
				Collection_addItem (collection.peek(), interval.transfer());
			}
		}
		if (collection -> size == 0) Melder_warning (L"No non-empty intervals were found.");
		return collection.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", sound, ": non-empty intervals not extracted.");
	}
}

Collection TextGrid_Sound_extractIntervalsWhere (TextGrid me, Sound sound, long tierNumber,
	int comparison_Melder_STRING, const wchar *text, int preserveTimes)
{
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoCollection collection = Collection_create (NULL, tier -> intervals -> size);
		long count = 0;
		for (long iseg = 1; iseg <= tier -> intervals -> size; iseg ++) {
			TextInterval segment = (TextInterval) tier -> intervals -> item [iseg];
			if (Melder_stringMatchesCriterion (segment -> text, comparison_Melder_STRING, text)) {
				autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes);
				wchar name [1000];
				swprintf (name, 1000, L"%ls_%ls_%ld", sound -> name ? sound -> name : L"", text, ++ count);
				Thing_setName (interval.peek(), name);
				Collection_addItem (collection.peek(), interval.transfer());
			}
		}
		if (collection -> size == 0)
			Melder_warning5 (L"No label that ", kMelder_string_getText (comparison_Melder_STRING), L" the text \"", text, L"\" was found.");
		return collection.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", sound, ": intervals not extracted.");
	}
}

static void autoMarks (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	double dy = ymax - ymin;
	if (dy < 26) {
		long imin = ceil ((ymin + 2.0) / 5.0), imax = floor ((ymax - 2.0) / 5.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 5, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 110) {
		long imin = ceil ((ymin + 8.0) / 20.0), imax = floor ((ymax - 8.0) / 20.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 20, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 260) {
		long imin = ceil ((ymin + 20.0) / 50.0), imax = floor ((ymax - 20.0) / 50.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 50, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 510) {
		long imin = ceil ((ymin + 40.0) / 100.0), imax = floor ((ymax - 40.0) / 100.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 100, TRUE, TRUE, haveDottedLines, NULL);
	}
}

static void autoMarks_logarithmic (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	double fy = ymax / ymin;
	for (int i = -12; i <= 12; i ++) {
		double power = pow (10, i), y = power;
		if (y > ymin * 1.2 && y < ymax / 1.2)
			Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
		if (fy > 2100) {
			;   /* Enough. */
		} else if (fy > 210) {
			y = 3.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
		} else {
			y = 2.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
			y = 5.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
			if (fy < 21) {
				y = 3.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
				y = 7.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
			}
			if (fy < 4.1) {
				y = 1.5 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
				y = 4.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, TRUE, TRUE, haveDottedLines, NULL);
			}
		}
	}
}

static void autoMarks_semitones (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	double dy = ymax - ymin;
	if (dy < 16) {
		long imin = ceil ((ymin + 1.2) / 3.0), imax = floor ((ymax - 1.2) / 3.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 3, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 32) {
		long imin = ceil ((ymin + 2.4) / 6.0), imax = floor ((ymax - 2.4) / 6.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 6, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 64) {
		long imin = ceil ((ymin + 4.8) / 12.0), imax = floor ((ymax - 4.8) / 12.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 12, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 128) {
		long imin = ceil ((ymin + 9.6) / 24.0), imax = floor ((ymax - 9.6) / 24.0);
		for (long i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 24, TRUE, TRUE, haveDottedLines, NULL);
	}
}

void TextGrid_Pitch_drawSeparately (TextGrid grid, Pitch pitch, Graphics g, double tmin, double tmax,
	double fmin, double fmax, int showBoundaries, int useTextStyles, int garnish, int speckle, int unit)
{
	int ntier = grid -> tiers -> size;
	if (tmax <= tmin) tmin = grid -> xmin, tmax = grid -> xmax;
	if (Function_isUnitLogarithmic (pitch, Pitch_LEVEL_FREQUENCY, unit)) {
		fmin = Function_convertStandardToSpecialUnit (pitch, fmin, Pitch_LEVEL_FREQUENCY, unit);
		fmax = Function_convertStandardToSpecialUnit (pitch, fmax, Pitch_LEVEL_FREQUENCY, unit);
	}
	if (unit == kPitch_unit_HERTZ_LOGARITHMIC)
		Pitch_draw (pitch, g, tmin, tmax, pow (10, fmin - 0.25 * (fmax - fmin) * ntier), pow (10, fmax), FALSE, speckle, unit);
	else
		Pitch_draw (pitch, g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * ntier, fmax, FALSE, speckle, unit);
	TextGrid_Sound_draw (grid, NULL, g, tmin, tmax, showBoundaries, useTextStyles, FALSE);
	/*
	 * Restore window for the sake of margin drawing.
	 */
	Graphics_setWindow (g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * ntier, fmax);
	if (unit == kPitch_unit_HERTZ_LOGARITHMIC)
		fmin = pow (10, fmin), fmax = pow (10, fmax);
	if (garnish) {
		Graphics_drawInnerBox (g);
		if (unit == kPitch_unit_HERTZ_LOGARITHMIC) {
			Graphics_markLeftLogarithmic (g, fmin, TRUE, TRUE, FALSE, NULL);
			Graphics_markLeftLogarithmic (g, fmax, TRUE, TRUE, FALSE, NULL);
			autoMarks_logarithmic (g, fmin, fmax, FALSE);
		} else if (unit == kPitch_unit_SEMITONES_100) {
			Graphics_markLeft (g, fmin, TRUE, TRUE, FALSE, NULL);
			Graphics_markLeft (g, fmax, TRUE, TRUE, FALSE, NULL);
			autoMarks_semitones (g, fmin, fmax, FALSE);
		} else {
			Graphics_markLeft (g, fmin, TRUE, TRUE, FALSE, NULL);
			Graphics_markLeft (g, fmax, TRUE, TRUE, FALSE, NULL);
			autoMarks (g, fmin, fmax, FALSE);
		}
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		MelderString_append (& buffer, L"Pitch (", Function_getUnitText (pitch, Pitch_LEVEL_FREQUENCY, unit, Function_UNIT_TEXT_GRAPHICAL), L")");
		Graphics_textLeft (g, true, buffer.string);
		Graphics_textBottom (g, true, L"Time (s)");
		Graphics_marksBottom (g, 2, true, true, true);
	}
}

void TextGrid_Pitch_draw (TextGrid grid, Pitch pitch, Graphics g,
	long tierNumber, double tmin, double tmax, double fmin, double fmax,
	double fontSize, int useTextStyles, int horizontalAlignment, int garnish, int speckle, int unit)
{
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (grid, tierNumber);
		double oldFontSize = Graphics_inqFontSize (g);
		Pitch_draw (pitch, g, tmin, tmax, fmin, fmax, garnish, speckle, unit);
		if (tmax <= tmin) tmin = grid -> xmin, tmax = grid -> xmax;
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch);
		if (Function_isUnitLogarithmic (pitch, Pitch_LEVEL_FREQUENCY, unit)) {
			fmin = Function_convertStandardToSpecialUnit (pitch, fmin, Pitch_LEVEL_FREQUENCY, unit);
			fmax = Function_convertStandardToSpecialUnit (pitch, fmax, Pitch_LEVEL_FREQUENCY, unit);
		}
		Graphics_setTextAlignment (g, horizontalAlignment, Graphics_BOTTOM);
		Graphics_setInner (g);
		Graphics_setFontSize (g, fontSize);
		Graphics_setPercentSignIsItalic (g, useTextStyles);
		Graphics_setNumberSignIsBold (g, useTextStyles);
		Graphics_setCircumflexIsSuperscript (g, useTextStyles);
		Graphics_setUnderscoreIsSubscript (g, useTextStyles);
		Function anyTier = (Function) grid -> tiers -> item [tierNumber];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			for (long i = 1; i <= tier -> intervals -> size; i ++) {
				TextInterval interval = (TextInterval) tier -> intervals -> item [i];
				double tleft = interval -> xmin, tright = interval -> xmax, tmid, f0;
				if (! interval -> text || ! interval -> text [0]) continue;
				if (tleft < pitch -> xmin) tleft = pitch -> xmin;
				if (tright > pitch -> xmax) tright = pitch -> xmax;
				tmid = (tleft + tright) / 2;
				if (tmid < tmin || tmid > tmax) continue;
				f0 = Function_convertStandardToSpecialUnit (pitch, RealTier_getValueAtTime (pitchTier.peek(), tmid), Pitch_LEVEL_FREQUENCY, unit);
				if (f0 < fmin || f0 > fmax) continue;
				Graphics_text (g,
					horizontalAlignment == Graphics_LEFT ? tleft : horizontalAlignment == Graphics_RIGHT ? tright : tmid,
					f0, interval -> text);
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			for (long i = 1; i <= tier -> points -> size; i ++) {
				TextPoint point = (TextPoint) tier -> points -> item [i];
				double t = point -> number, f0;
				if (! point -> mark || ! point -> mark [0]) continue;
				if (t < tmin || t > tmax) continue;
				f0 = Function_convertStandardToSpecialUnit (pitch, RealTier_getValueAtTime (pitchTier.peek(), t), Pitch_LEVEL_FREQUENCY, unit);
				if (f0 < fmin || f0 > fmax) continue;
				Graphics_text (g, t, f0, point -> mark);
			}
		}
		Graphics_setPercentSignIsItalic (g, TRUE);
		Graphics_setNumberSignIsBold (g, TRUE);
		Graphics_setCircumflexIsSuperscript (g, TRUE);
		Graphics_setUnderscoreIsSubscript (g, TRUE);
		Graphics_setFontSize (g, oldFontSize);
		Graphics_unsetInner (g);
	} catch (MelderError) {
		Melder_throw (grid, " & ", pitch, ": not drawn.");
	}
}

/* End of file TextGrid_Sound.cpp */
