/* TextGrid.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2001/08/07
 * pb 2002/07/16 GPL
 * pb 2003/03/12 corrected TextGrid_extractPart (could crash)
 * pb 2003/05/09 added extractNonemptyIntervals
 * pb 2004/04/14 TextGrid_merge supports more than two TextGrids
 */

#include "TextGrid.h"
#include "Pitch_to_PitchTier.h"
#include "longchar.h"

#include "oo_DESTROY.h"
#include "TextGrid_def.h"
#include "oo_COPY.h"
#include "TextGrid_def.h"
#include "oo_EQUAL.h"
#include "TextGrid_def.h"
#include "oo_WRITE_ASCII.h"
#include "TextGrid_def.h"
#include "oo_READ_ASCII.h"
#include "TextGrid_def.h"
#include "oo_WRITE_BINARY.h"
#include "TextGrid_def.h"
#include "oo_READ_BINARY.h"
#include "TextGrid_def.h"
#include "oo_DESCRIPTION.h"
#include "TextGrid_def.h"

class_methods (TextPoint, AnyPoint)
	class_method_local (TextPoint, destroy)
	class_method_local (TextPoint, copy)
	class_method_local (TextPoint, equal)
	class_method_local (TextPoint, writeAscii)
	class_method_local (TextPoint, readAscii)
	class_method_local (TextPoint, writeBinary)
	class_method_local (TextPoint, readBinary)
	class_method_local (TextPoint, description)
class_methods_end

TextPoint TextPoint_create (double time, const char *mark) {
	TextPoint me = new (TextPoint);
	if (! me) return NULL;
	my time = time;
	my mark = Melder_strdup (mark);
	if (Melder_hasError ()) forget (me);
	return me;
}

int TextPoint_setText (TextPoint me, const char *text) {
	char *newText;
	/*
	 * Be fast if the string pointers are equal.
	 */
	if (text == my mark) return 1;
	/*
	 * Create a temporary variable (i.e. a temporary pointer to the final string),
	 * in order that 'my text' does not change in case of error.
	 */
	newText = Melder_strdup (text);
	if (! newText) return 0;
	Melder_free (my mark);
	my mark = newText;
	return 1;
}

class_methods (TextInterval, Function)
	class_method_local (TextInterval, destroy)
	class_method_local (TextInterval, copy)
	class_method_local (TextInterval, equal)
	class_method_local (TextInterval, writeAscii)
	class_method_local (TextInterval, readAscii)
	class_method_local (TextInterval, writeBinary)
	class_method_local (TextInterval, readBinary)
	class_method_local (TextInterval, description)
class_methods_end

TextInterval TextInterval_create (double tmin, double tmax, const char *text) {
	TextInterval me = new (TextInterval);
	if (! me) return NULL;
	my xmin = tmin;
	my xmax = tmax;
	my text = Melder_strdup (text);
	if (Melder_hasError ()) forget (me);
	return me;
}

int TextInterval_setText (TextInterval me, const char *text) {
	char *newText;
	/*
	 * Be fast if the string pointers are equal.
	 */
	if (text == my text) return 1;
	/*
	 * Create a temporary variable (i.e. a temporary pointer to the final string),
	 * in order that 'my text' does not change in case of error.
	 */
	newText = Melder_strdup (text);
	if (! newText) return 0;
	Melder_free (my text);
	my text = newText;
	return 1;
}

class_methods (TextTier, Function)
	class_method_local (TextTier, destroy)
	class_method_local (TextTier, copy)
	class_method_local (TextTier, equal)
	class_method_local (TextTier, writeAscii)
	class_method_local (TextTier, readAscii)
	class_method_local (TextTier, writeBinary)
	class_method_local (TextTier, readBinary)
	class_method_local (TextTier, description)
class_methods_end

TextTier TextTier_create (double tmin, double tmax) {
	TextTier me = new (TextTier);
	if (! me || ! (my points = SortedSetOfDouble_create ()))
		{ forget (me); return NULL; }
	my xmin = tmin;
	my xmax = tmax;
	return me;
}

int TextTier_addPoint (TextTier me, double time, const char *mark) {
	TextPoint point = TextPoint_create (time, mark);
	if (! point || ! Collection_addItem (my points, point)) return 0;
	return 1;
}

class_methods (IntervalTier, Function)
	class_method_local (IntervalTier, destroy)
	class_method_local (IntervalTier, copy)
	class_method_local (IntervalTier, equal)
	class_method_local (IntervalTier, writeAscii)
	class_method_local (IntervalTier, readAscii)
	class_method_local (IntervalTier, writeBinary)
	class_method_local (IntervalTier, readBinary)
	class_method_local (IntervalTier, description)
class_methods_end

IntervalTier IntervalTier_create (double tmin, double tmax) {
	IntervalTier me = new (IntervalTier);
	TextInterval interval;
	if (! me || ! (my intervals = SortedSetOfDouble_create ()))
		{ forget (me); return NULL; }
	my xmin = tmin;
	my xmax = tmax;
	if (! (interval = TextInterval_create (tmin, tmax, NULL)))
		{ forget (me); return NULL; }
	if (! Collection_addItem (my intervals, interval))
		{ forget (me); return NULL; }
	return me;
}

long IntervalTier_timeToLowIndex (IntervalTier me, double t) {
	long i; 
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (t >= interval -> xmin && t < interval -> xmax)
			return i;
	}
	return 0;   /* Empty tier or very small or large t. */
}

long IntervalTier_timeToIndex (IntervalTier me, double t) {
	return t == my xmax ? my intervals -> size : IntervalTier_timeToLowIndex (me, t);
}

long IntervalTier_timeToHighIndex (IntervalTier me, double t) {
	long i; 
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (t > interval -> xmin && t <= interval -> xmax)
			return i;
	}
	return 0;   /* Empty tier or very small or large t. */
}

long IntervalTier_hasTime (IntervalTier me, double t) {
	long iinterval;
	for (iinterval = 1; iinterval <= my intervals -> size; iinterval ++) {
		TextInterval interval = my intervals -> item [iinterval];
		if (interval -> xmin == t || iinterval == my intervals -> size && interval -> xmax == t) {
			return iinterval;   /* Time found. */
		}
	}
	return 0;   /* Time not found. */
}

long IntervalTier_hasBoundary (IntervalTier me, double t) {
	long iinterval;
	for (iinterval = 2; iinterval <= my intervals -> size; iinterval ++) {
		TextInterval interval = my intervals -> item [iinterval];
		if (interval -> xmin == t) {
			return iinterval;   /* Boundary found. */
		}
	}
	return 0;   /* Boundary not found. */
}

static void classTextGrid_info (I) {
	iam (TextGrid);
	long ntier = my tiers -> size;
	long numberOfIntervalTiers = 0, numberOfPointTiers = 0, numberOfIntervals = 0, numberOfPoints = 0, itier;
	for (itier = 1; itier <= ntier; itier ++) {
		Data anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			numberOfIntervalTiers += 1;
			numberOfIntervals += tier -> intervals -> size;
		} else {
			TextTier tier = (TextTier) anyTier;
			numberOfPointTiers += 1;
			numberOfPoints += tier -> points -> size;
		}
	}
	Melder_info ("%ld interval tier%s", numberOfIntervalTiers, numberOfIntervalTiers == 1 ? "" : "s");
	Melder_info ("%ld point tier%s", numberOfPointTiers, numberOfPointTiers == 1 ? "" : "s");
	Melder_info ("%ld interval%s", numberOfIntervals, numberOfIntervals == 1 ? "" : "s");
	Melder_info ("%ld point%s", numberOfPoints, numberOfPoints == 1 ? "" : "s");
}

class_methods (TextGrid, Function)
	class_method_local (TextGrid, destroy)
	class_method_local (TextGrid, copy)
	class_method_local (TextGrid, equal)
	class_method_local (TextGrid, writeAscii)
	class_method_local (TextGrid, readAscii)
	class_method_local (TextGrid, writeBinary)
	class_method_local (TextGrid, readBinary)
	class_method_local (TextGrid, description)
	class_method_local (TextGrid, info)
class_methods_end

TextGrid TextGrid_create (double tmin, double tmax, const char *tierNames, const char *pointTiers) {
	TextGrid me = new (TextGrid);
	char nameBuffer [400], *tierName;
	if (! me || ! (my tiers = Ordered_create ()))
		{ forget (me); return NULL; }
	my xmin = tmin;
	my xmax = tmax;

	/*
	 * Create a number of IntervalTier objects.
	 */
	if (tierNames && tierNames [0]) {
		strcpy (nameBuffer, tierNames);
		for (tierName = strtok (nameBuffer, " "); tierName != NULL; tierName = strtok (NULL, " ")) {
			IntervalTier tier = IntervalTier_create (tmin, tmax);
			if (! tier || ! Collection_addItem (my tiers, tier)) { forget (me); return NULL; }
			Thing_setName (tier, tierName);
		}
	}

	/*
	 * Replace some IntervalTier objects with TextTier objects.
	 */
	if (pointTiers && pointTiers [0]) {
		strcpy (nameBuffer, pointTiers);
		for (tierName = strtok (nameBuffer, " "); tierName != NULL; tierName = strtok (NULL, " ")) {
			long itier;
			for (itier = 1; itier <= my tiers -> size; itier ++) {
				if (strequ (tierName, Thing_getName (my tiers -> item [itier]))) {
					TextTier tier = TextTier_create (tmin, tmax);
					if (! tier) { forget (me); return NULL; }
					forget (my tiers -> item [itier]);
					my tiers -> item [itier] = tier;
					Thing_setName (tier, tierName);
				}
			}
		}
	}
	return me;
}

TextTier TextTier_readFromXwaves (MelderFile file) {
	TextTier me = NULL;
	char *line;

	me = TextTier_create (0, 100);
	if (! me) goto end;

	MelderFile_open (file); cherror

	/*
	 * Search for a line that starts with '#'.
	 */
	for (;;) {
		line = MelderFile_readLine (file); cherror
		if (line == NULL) { Melder_error ("No '#' line."); goto end; }
		if (line [0] == '#') break;
	}

	/*
	 * Read a mark from every line.
	 */
	for (;;) {
		double time;
		long colour;
		char mark [300];

		line = MelderFile_readLine (file); cherror
		if (line == NULL) break;   /* Normal end-of-file. */
		if (sscanf (line, "%lf%ld%s", & time, & colour, mark) < 3) {
			Melder_error ("Line too short: \"%s\".", line);
			goto end;
		}
		if (! TextTier_addPoint (me, time, mark)) goto end;
	}

	/*
	 * Fix domain.
	 */
	if (my points -> size) {
		TextPoint point = my points -> item [1];
		if (point -> time < 0.0) my xmin = point -> time - 1.0;
		point = my points -> item [my points -> size];
		my xmax = point -> time + 1.0;
	}

end:
	MelderFile_close (file);
	if (Melder_hasError ()) {
		forget (me);
		return Melder_errorp ("(TextTier_readFromXwaves:) Not read.");
	}
	return me;
}

long TextGrid_countLabels (TextGrid me, long itier, const char *text) {
	Data anyTier;
	long i, count = 0;
	if (itier < 1 || itier > my tiers -> size) return 0;
	anyTier = my tiers -> item [itier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		for (i = 1; i <= tier -> intervals -> size; i ++) {
			TextInterval segment = tier -> intervals -> item [i];
			if (segment -> text && strequ (segment -> text, text))
				count ++;
		}
	} else {
		TextTier tier = (TextTier) anyTier;
		for (i = 1; i <= tier -> points -> size; i ++) {
			TextPoint point = tier -> points -> item [i];
			if (point -> mark && strequ (point -> mark, text))
				count ++;
		}
	}
	return count;
}

int TextGrid_add (TextGrid me, Any anyTier) {
	Function tier = Data_copy (anyTier);
	if (! tier) return 0;
	if (tier -> xmin < my xmin) my xmin = tier -> xmin;
	if (tier -> xmax > my xmax) my xmax = tier -> xmax;
	return Collection_addItem (my tiers, tier);
}

TextGrid TextGrid_merge (Collection textGrids) {
	TextGrid thee = NULL;
	long igrid, itier;
	if (textGrids -> size < 1) { Melder_error ("Cannot merge zero TextGrid objects."); goto end; }
	thee = Data_copy (textGrids -> item [1]); cherror
	for (igrid = 2; igrid <= textGrids -> size; igrid ++) {
		TextGrid textGrid = textGrids -> item [igrid];
		for (itier = 1; itier <= textGrid -> tiers -> size; itier ++) {
			TextGrid_add (thee, textGrid -> tiers -> item [itier]); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

static TextGrid _Label_to_TextGrid (Label me, double tmin, double tmax) {
	long itier, iinterval;
	TextGrid thee = TextGrid_create (tmin, tmax, NULL, NULL);
	if (! thee) return NULL;
	for (itier = 1; itier <= my size; itier ++) {
		Tier tier = my item [itier];
		IntervalTier intervalTier = IntervalTier_create (tmin, tmax);
		if (! tier || ! Collection_addItem (thy tiers, intervalTier))
			{ forget (thee); return NULL; }
		Collection_removeItem (intervalTier -> intervals, 1);
		for (iinterval = 1; iinterval <= tier -> size; iinterval ++) {
			Autosegment autosegment = tier -> item [iinterval];
			TextInterval textInterval = TextInterval_create (
				iinterval == 1 ? tmin : autosegment -> xmin,
				iinterval == tier -> size ? tmax : autosegment -> xmax,
				autosegment -> name);
			if (! textInterval || ! Collection_addItem (intervalTier -> intervals, textInterval))
				{ forget (thee); return NULL; }
		}
	}
	return thee;
}

void TextGrid_shiftTimes (TextGrid me, double shift) {
	int itier, ntier = my tiers -> size;
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Function_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = ninterval; iinterval >= 1; iinterval --) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				interval -> xmin += shift;
				interval -> xmax += shift;
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long i, n = tier -> points -> size;
			for (i = n; i >= 1; i --) {
				TextPoint point = tier -> points -> item [i];
				point -> time += shift;
			}
		}
		anyTier -> xmin += shift;
		anyTier -> xmax += shift;
	}
	my xmin += shift;
	my xmax += shift;
}

void TextGrid_shiftToZero (TextGrid me) {
	TextGrid_shiftTimes (me, - my xmin);
}

TextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, int preserveTimes) {
	TextGrid thee = Data_copy (me);
	int itier, ntier = my tiers -> size;
	if (! thee) return NULL;
	if (tmax <= tmin) return thee;

	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = thy tiers -> item [itier];
		if (anyTier -> methods == (Function_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = ninterval; iinterval >= 1; iinterval --) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				if (interval -> xmin >= tmax || interval -> xmax <= tmin) {
					Collection_removeItem (tier -> intervals, iinterval);
				} else {
					if (interval -> xmin < tmin) interval -> xmin = tmin;
					if (interval -> xmax > tmax) interval -> xmax = tmax;
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long i, n = tier -> points -> size;
			for (i = n; i >= 1; i --) {
				TextPoint point = tier -> points -> item [i];
				if (point -> time < tmin || point -> time > tmax) Collection_removeItem (tier -> points, i);
			}
		}
		anyTier -> xmin = tmin;
		anyTier -> xmax = tmax;
	}
	thy xmin = tmin;
	thy xmax = tmax;
	if (! preserveTimes) TextGrid_shiftToZero (thee);
	return thee;
}

TextGrid Label_to_TextGrid (Label me, double duration) {
	double tmin = 0.0, tmax = duration;
	if (duration == 0.0) Label_suggestDomain (me, & tmin, & tmax);
Melder_casual ("%g %g %g", duration,tmin,tmax);
	return _Label_to_TextGrid (me, tmin, tmax);
}

TextGrid Label_Function_to_TextGrid (Label me, Any function) {
	return _Label_to_TextGrid (me, ((Function) function) -> xmin, ((Function) function) -> xmax);
}

void TextGrid_Sound_draw (TextGrid me, Sound sound, Graphics g, double tmin, double tmax,
	int showBoundaries, int useTextStyles, int garnish)
{
	long first, last;
	int itier, ntier = my tiers -> size;

	/*
	 * Automatic windowing:
	 */
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;

	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, -1.0 - 0.5 * ntier, 1.0);

	/*
	 * Draw sound in upper part.
	 */
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
	Graphics_setTextAlignment (g, Graphics_CENTER, Graphics_HALF);
	Graphics_setPercentSignIsItalic (g, useTextStyles);
	Graphics_setNumberSignIsBold (g, useTextStyles);
	Graphics_setCircumflexIsSuperscript (g, useTextStyles);
	Graphics_setUnderscoreIsSubscript (g, useTextStyles);
	for (itier = 1; itier <= ntier; itier ++) {
		Data anyTier = my tiers -> item [itier];
		double ymin = -1.0 - 0.5 * itier, ymax = ymin + 0.5;
		Graphics_rectangle (g, tmin, tmax, ymin, ymax);
		if (anyTier -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals -> item [iinterval];
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
			long i, n = tier -> points -> size;
			for (i = 1; i <= n; i ++) {
				TextPoint point = tier -> points -> item [i];
				double t = point -> time;
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
		Graphics_textBottom (g, 1, "Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 1);
	}
}

Collection TextGrid_Sound_extractAllIntervals (TextGrid me, Sound sound, long itier, int preserveTimes) {
	IntervalTier tier;
	long iseg;
	Collection collection;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_errorp ("Tier number %ld out of range 1..%ld.", itier, my tiers -> size);
	tier = my tiers -> item [itier];
	if (tier -> methods != classIntervalTier)
		return Melder_errorp ("Tier %ld is not an interval tier.", itier);
	collection = Collection_create (NULL, tier -> intervals -> size); cherror
	for (iseg = 1; iseg <= tier -> intervals -> size; iseg ++) {
		TextInterval segment = tier -> intervals -> item [iseg];
		Sound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, 0, 1.0, preserveTimes); cherror
		Thing_setName (interval, segment -> text ? segment -> text : "untitled");
		Collection_addItem (collection, interval); cherror
	}
end:
	iferror forget (collection);
	return collection;
}

Collection TextGrid_Sound_extractNonemptyIntervals (TextGrid me, Sound sound, long itier, int preserveTimes) {
	IntervalTier tier;
	long iseg;
	Collection collection;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_errorp ("Tier number %ld out of range 1..%ld.", itier, my tiers -> size);
	tier = my tiers -> item [itier];
	if (tier -> methods != classIntervalTier)
		return Melder_errorp ("Tier %ld is not an interval tier.", itier);
	collection = Collection_create (NULL, tier -> intervals -> size); cherror
	for (iseg = 1; iseg <= tier -> intervals -> size; iseg ++) {
		TextInterval segment = tier -> intervals -> item [iseg];
		if (segment -> text != NULL && segment -> text [0] != '\0') {
			Sound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, 0, 1.0, preserveTimes); cherror
			Thing_setName (interval, segment -> text ? segment -> text : "untitled");
			Collection_addItem (collection, interval); cherror
		}
	}
	if (collection -> size == 0) Melder_warning ("No non-empty intervals were found.");
end:
	iferror forget (collection);
	return collection;
}

Collection TextGrid_Sound_extractIntervals (TextGrid me, Sound sound, long itier, const char *text, int preserveTimes) {
	IntervalTier tier;
	long iseg, count = 0;
	Collection collection;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_errorp ("Tier number %ld out of range 1..%ld.", itier, my tiers -> size);
	tier = my tiers -> item [itier];
	if (tier -> methods != classIntervalTier)
		return Melder_errorp ("Tier %ld is not an interval tier.", itier);
	collection = Collection_create (NULL, 10);
	if (! collection) goto error;
	for (iseg = 1; iseg <= tier -> intervals -> size; iseg ++) {
		TextInterval segment = tier -> intervals -> item [iseg];
		if (segment -> text && strequ (segment -> text, text)) {
			Sound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax,
				0, 1.0, preserveTimes);
			char name [1000];
			if (! interval) goto error;
			sprintf (name, "%s_%s_%ld", sound -> name ? sound -> name : "", text, ++ count);
			Thing_setName (interval, name);
			if (! Collection_addItem (collection, interval)) goto error;
		}
	}
	if (collection -> size == 0) Melder_warning ("No labels \"%s\" were found.", text);
	return collection;
error:
	forget (collection);
	return NULL;
}

void TextGrid_Pitch_draw (TextGrid grid, Pitch pitch, Graphics g,
	long itier, double tmin, double tmax, double fmin, double fmax,
	double fontSize, int useTextStyles, int garnish, int speckle, int yscale)
{
	Data anyTier;
	long i;
	PitchTier pitchTier = NULL;
	double oldFontSize = Graphics_inqFontSize (g);
	Pitch_draw (pitch, g, tmin, tmax, fmin, fmax, garnish, speckle, yscale);
	if (tmax <= tmin) tmin = grid -> xmin, tmax = grid -> xmax;
	if (itier < 1 || itier > grid -> tiers -> size) goto end;
	pitchTier = Pitch_to_PitchTier (pitch);
	if (! pitchTier) goto end;
	Pitch_convertYscale (& fmin, & fmax, yscale);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_setInner (g);
	Graphics_setFontSize (g, fontSize);
	Graphics_setPercentSignIsItalic (g, useTextStyles);
	Graphics_setNumberSignIsBold (g, useTextStyles);
	Graphics_setCircumflexIsSuperscript (g, useTextStyles);
	Graphics_setUnderscoreIsSubscript (g, useTextStyles);
	anyTier = grid -> tiers -> item [itier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		for (i = 1; i <= tier -> intervals -> size; i ++) {
			TextInterval interval = tier -> intervals -> item [i];
			double tleft = interval -> xmin, tright = interval -> xmax, tmid, f0;
			if (! interval -> text || ! interval -> text [0]) continue;
			if (tleft < pitch -> xmin) tleft = pitch -> xmin;
			if (tright > pitch -> xmax) tright = pitch -> xmax;
			tmid = (tleft + tright) / 2;
			if (tmid < tmin || tmid > tmax) continue;
			f0 = Pitch_convertFrequency (RealTier_getValueAtTime (pitchTier, tmid), yscale);
			if (f0 < fmin || f0 > fmax) continue;
			Graphics_text (g, tmid, f0, interval -> text);
		}
	} else {
		TextTier tier = (TextTier) anyTier;
		for (i = 1; i <= tier -> points -> size; i ++) {
			TextPoint point = tier -> points -> item [i];
			double t = point -> time, f0;
			if (! point -> mark || ! point -> mark [0]) continue;
			if (t < tmin || t > tmax) continue;
			f0 = Pitch_convertFrequency (RealTier_getValueAtTime (pitchTier, t), yscale);
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
end:
	forget (pitchTier);
}

static void autoMarks (Graphics g, double ymin, double ymax, int haveDottedLines) {
	double dy = ymax - ymin;
	if (dy < 26) {
		long imin = ceil ((ymin + 2.0) / 5.0), imax = floor ((ymax - 2.0) / 5.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 5, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 110) {
		long imin = ceil ((ymin + 8.0) / 20.0), imax = floor ((ymax - 8.0) / 20.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 20, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 260) {
		long imin = ceil ((ymin + 20.0) / 50.0), imax = floor ((ymax - 20.0) / 50.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 50, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 510) {
		long imin = ceil ((ymin + 40.0) / 100.0), imax = floor ((ymax - 40.0) / 100.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 100, TRUE, TRUE, haveDottedLines, NULL);
	}
}

static void autoMarks_logarithmic (Graphics g, double ymin, double ymax, int haveDottedLines) {
	double fy = ymax / ymin;
	int i;
	for (i = -12; i <= 12; i ++) {
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

static void autoMarks_semitones (Graphics g, double ymin, double ymax, int haveDottedLines) {
	double dy = ymax - ymin;
	if (dy < 16) {
		long imin = ceil ((ymin + 1.2) / 3.0), imax = floor ((ymax - 1.2) / 3.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 3, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 32) {
		long imin = ceil ((ymin + 2.4) / 6.0), imax = floor ((ymax - 2.4) / 6.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 6, TRUE, TRUE, haveDottedLines, NULL), i;
	} else if (dy < 64) {
		long imin = ceil ((ymin + 4.8) / 12.0), imax = floor ((ymax - 4.8) / 12.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 12, TRUE, TRUE, haveDottedLines, NULL);
	} else if (dy < 128) {
		long imin = ceil ((ymin + 9.6) / 24.0), imax = floor ((ymax - 9.6) / 24.0), i;
		for (i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 24, TRUE, TRUE, haveDottedLines, NULL);
	}
}

void TextGrid_Pitch_drawSeparately (TextGrid grid, Pitch pitch, Graphics g, double tmin, double tmax,
	double fmin, double fmax, int showBoundaries, int useTextStyles, int garnish, int speckle, int yscale)
{
	int ntier = grid -> tiers -> size;
	if (tmax <= tmin) tmin = grid -> xmin, tmax = grid -> xmax;
	Pitch_convertYscale (& fmin, & fmax, yscale);
	if (yscale == Pitch_yscale_LOGARITHMIC)
		Pitch_draw (pitch, g, tmin, tmax, pow (10, fmin - 0.25 * (fmax - fmin) * ntier), pow (10, fmax), FALSE, speckle, yscale);
	else
		Pitch_draw (pitch, g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * ntier, fmax, FALSE, speckle, yscale);
	TextGrid_Sound_draw (grid, NULL, g, tmin, tmax, showBoundaries, useTextStyles, FALSE);
	/*
	 * Restore window for the sake of margin drawing.
	 */
	Graphics_setWindow (g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * ntier, fmax);
	if (yscale == Pitch_yscale_LOGARITHMIC)
		fmin = pow (10, fmin), fmax = pow (10, fmax);
	if (garnish) {
		Graphics_drawInnerBox (g);
		if (yscale == Pitch_yscale_LOGARITHMIC) {
			Graphics_markLeftLogarithmic (g, fmin, TRUE, TRUE, FALSE, NULL);
			Graphics_markLeftLogarithmic (g, fmax, TRUE, TRUE, FALSE, NULL);
			autoMarks_logarithmic (g, fmin, fmax, FALSE);
		} else if (yscale == Pitch_yscale_SEMITONES) {
			Graphics_markLeft (g, fmin, TRUE, TRUE, FALSE, NULL);
			Graphics_markLeft (g, fmax, TRUE, TRUE, FALSE, NULL);
			autoMarks_semitones (g, fmin, fmax, FALSE);
		} else {
			Graphics_markLeft (g, fmin, TRUE, TRUE, FALSE, NULL);
			Graphics_markLeft (g, fmax, TRUE, TRUE, FALSE, NULL);
			autoMarks (g, fmin, fmax, FALSE);
		}
		Graphics_textLeft (g, TRUE, Pitch_yscaleText (yscale));
		Graphics_textBottom (g, TRUE, "Time (s)");
		Graphics_marksBottom (g, 2, TRUE, TRUE, TRUE);
	}
}

TextTier PointProcess_upto_TextTier (PointProcess me, const char *text) {
	long i;
	TextTier thee = TextTier_create (my xmin, my xmax);
	if (! thee) return NULL;
	for (i = 1; i <= my nt; i ++)
		if (! TextTier_addPoint (thee, my t [i], text)) { forget (thee); return NULL; }
	return thee;
}

PointProcess TextTier_getPoints (TextTier me, const char *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	long i;
	for (i = 1; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		if (text && text [0]) {
			if (point -> mark && strequ (text, point -> mark))
				PointProcess_addPoint (thee, point -> time);
		} else {
			if (! point -> mark || ! point -> mark [0])
				PointProcess_addPoint (thee, point -> time);
		}
	}
	return thee;
}

PointProcess IntervalTier_getStartingPoints (IntervalTier me, const char *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	long i;
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (text && text [0]) {
			if (interval -> text && strequ (text, interval -> text))
				PointProcess_addPoint (thee, interval -> xmin);
		} else {
			if (! interval -> text || ! interval -> text [0])
				PointProcess_addPoint (thee, interval -> xmin);
		}
	}
	return thee;
}

PointProcess IntervalTier_getEndPoints (IntervalTier me, const char *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	long i;
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (text && text [0]) {
			if (interval -> text && strequ (text, interval -> text))
				PointProcess_addPoint (thee, interval -> xmax);
		} else {
			if (! interval -> text || ! interval -> text [0])
				PointProcess_addPoint (thee, interval -> xmax);
		}
	}
	return thee;
}

PointProcess IntervalTier_getCentrePoints (IntervalTier me, const char *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	long i;
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (text && text [0]) {
			if (interval -> text && strequ (text, interval -> text))
				PointProcess_addPoint (thee, 0.5 * (interval -> xmin + interval -> xmax));
		} else {
			if (! interval -> text || ! interval -> text [0])
				PointProcess_addPoint (thee, 0.5 * (interval -> xmin + interval -> xmax));
		}
	}
	return thee;
}

PointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase) {
	PointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
	long i;
	if (! thee) return NULL;
	for (i = 1; i <= point -> nt; i ++) {
		double t = point -> t [i];
		long index = IntervalTier_timeToLowIndex (tier, t);
		if (index) {
			TextInterval interval = tier -> intervals -> item [index];
			if (interval -> xmin == t && ! PointProcess_addPoint (thee, (1 - phase) * interval -> xmin + phase * interval -> xmax))
					{ forget (thee); return NULL; }
		}
	}
	return thee;
}

PointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase) {
	PointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
	long i;
	if (! thee) return NULL;
	for (i = 1; i <= point -> nt; i ++) {
		double t = point -> t [i];
		long index = IntervalTier_timeToHighIndex (tier, t);
		if (index) {
			TextInterval interval = tier -> intervals -> item [index];
			if (interval -> xmax == t && ! PointProcess_addPoint (thee, (1 - phase) * interval -> xmin + phase * interval -> xmax))
					{ forget (thee); return NULL; }
		}
	}
	return thee;
}

TableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, const char *label) {
	TableOfReal thee = NULL;
	long i, n = 0;
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (label == NULL || label [0] == '\0' && ! interval -> text ||
		    interval -> text && strequ (interval -> text, label))
			n ++;
	}
	thee = TableOfReal_create (n, 3); cherror
	TableOfReal_setColumnLabel (thee, 1, "Start");
	TableOfReal_setColumnLabel (thee, 2, "End");
	TableOfReal_setColumnLabel (thee, 3, "Duration");
	for (i = 1, n = 0; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (label == NULL || label [0] == '\0' && ! interval -> text ||
		    interval -> text && strequ (interval -> text, label))
		{
			n ++;
			TableOfReal_setRowLabel (thee, n, interval -> text ? interval -> text : "");
			thy data [n] [1] = interval -> xmin;
			thy data [n] [2] = interval -> xmax;
			thy data [n] [3] = interval -> xmax - interval -> xmin;
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me) {
	return IntervalTier_downto_TableOfReal (me, NULL);
}

TableOfReal TextTier_downto_TableOfReal (TextTier me, const char *label) {
	TableOfReal thee = NULL;
	long i, n = 0;
	for (i = 1; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		if (label == NULL || label [0] == '\0' && ! point -> mark ||
		    point -> mark && strequ (point -> mark, label))
			n ++;
	}
	thee = TableOfReal_create (n, 1); cherror
	TableOfReal_setColumnLabel (thee, 1, "Time");
	for (i = 1, n = 0; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		if (label == NULL || label [0] == '\0' && ! point -> mark ||
		    point -> mark && strequ (point -> mark, label))
		{
			n ++;
			TableOfReal_setRowLabel (thee, n, point -> mark ? point -> mark : "");
			thy data [n] [1] = point -> time;
		}
	}
end:
	iferror forget (thee);
	return thee;
}

TableOfReal TextTier_downto_TableOfReal_any (TextTier me) {
	return TextTier_downto_TableOfReal (me, NULL);
}

static int IntervalTier_add (IntervalTier me, double tmin, double tmax, const char *label) {
	TextInterval interval = TextInterval_create (tmin, tmax, label);
	if (! interval || ! Collection_addItem (my intervals, interval)) return 0;
	return 1;
}

IntervalTier IntervalTier_readFromXwaves (MelderFile file) {
	IntervalTier me = NULL;
	char *line;
	double lastTime = 0.0;

	me = IntervalTier_create (0, 100);
	if (! me) goto end;

	MelderFile_open (file); cherror

	/*
	 * Search for a line that starts with '#'.
	 */
	for (;;) {
		line = MelderFile_readLine (file); cherror
		if (line == NULL) { Melder_error ("No '#' line."); goto end; }
		if (line [0] == '#') break;
	}

	/*
	 * Read a mark from every line.
	 */
	for (;;) {
		double time;
		long colour;
		char mark [300];

		line = MelderFile_readLine (file); cherror
		if (line == NULL) break;   /* Normal end-of-file. */
		if (sscanf (line, "%lf%ld%s", & time, & colour, mark) < 3) {
			Melder_error ("Line too short: \"%s\".", line);
			goto end;
		}
		if (lastTime == 0.0) {
			TextInterval interval = my intervals -> item [1];
			interval -> xmax = time;
			if (! TextInterval_setText (interval, mark)) goto end;
		} else {
			if (! IntervalTier_add (me, lastTime, time, mark)) goto end;
		}
		lastTime = time;
	}

	/*
	 * Fix domain.
	 */
	if (lastTime > 0.0) {
		TextInterval lastInterval = my intervals -> item [my intervals -> size];
		my xmax = lastInterval -> xmax = lastTime;
	}

end:
	MelderFile_close (file);
	if (Melder_hasError ()) {
		forget (me);
		return Melder_errorp ("(IntervalTier_readFromXwaves:) Not read.");
	}
	return me;
}

TextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT) {
	TextGrid thee = TextGrid_create (my xmin, my xmax, "vuv", NULL);
	long ipointleft, ipointright;
	double beginVoiceless = my xmin, endVoiceless, halfMeanT = 0.5 * meanT;
	Collection_removeItem (((IntervalTier) thy tiers -> item [1]) -> intervals, 1);
	for (ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
		endVoiceless = my t [ipointleft] - halfMeanT;
		if (endVoiceless <= beginVoiceless) {
			endVoiceless = beginVoiceless;   /* We will use for voiced interval. */
		} else {
			IntervalTier_add (thy tiers -> item [1], beginVoiceless, endVoiceless, "U"); cherror
		}
		for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
			if (my t [ipointright] - my t [ipointright - 1] > maxT)
				break;
		ipointright --;
		beginVoiceless = my t [ipointright] + halfMeanT;
		if (beginVoiceless > my xmax)
			beginVoiceless = my xmax;
		IntervalTier_add (thy tiers -> item [1], endVoiceless, beginVoiceless, "V"); cherror
	}
	endVoiceless = my xmax;
	if (endVoiceless > beginVoiceless) {
		IntervalTier_add (thy tiers -> item [1], beginVoiceless, endVoiceless, "U"); cherror
	}
end:
	iferror forget (thee);
	return thee;
}

static double TextGrid_scaleTime (TextGrid me, double tmin, double tmax, double t) {
	if (t == my xmin) return tmin;
	if (t == my xmax) return tmax;
	return (tmax - tmin) / (my xmax - my xmin) * (t - my xmin) + tmin;
}

void TextGrid_scaleTimes (TextGrid me, double tmin, double tmax) {
	long itier, ntier;
	if (my xmin == tmin && my xmax == tmax) return;
	ntier = my tiers -> size;
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Function_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				interval -> xmin = TextGrid_scaleTime (me, tmin, tmax, interval -> xmin);
				interval -> xmax = TextGrid_scaleTime (me, tmin, tmax, interval -> xmax);
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long i, n = tier -> points -> size;
			for (i = 1; i <= n; i ++) {
				TextPoint point = tier -> points -> item [i];
				point -> time = TextGrid_scaleTime (me, tmin, tmax, point -> time);
			}
		}
		anyTier -> xmin = TextGrid_scaleTime (me, tmin, tmax, anyTier -> xmin);
		anyTier -> xmax = TextGrid_scaleTime (me, tmin, tmax, anyTier -> xmax);
	}
	my xmin = tmin;
	my xmax = tmax;
}

void TextGrid_Function_scaleTimes (TextGrid me, Function thee) {
	TextGrid_scaleTimes (me, thy xmin, thy xmax);
}

long TextInterval_labelLength (TextInterval me) {
	return my text ? strlen (my text) : 0;
}

long TextPoint_labelLength (TextPoint me) {
	return my mark ? strlen (my mark) : 0;
}

long IntervalTier_maximumLabelLength (IntervalTier me) {
	long maximum = 0, iinterval, ninterval = my intervals -> size;
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		long length = TextInterval_labelLength (my intervals -> item [iinterval]);
		if (length > maximum) maximum = length;
	}
	return maximum;
}

long TextTier_maximumLabelLength (TextTier me) {
	long maximum = 0, ipoint, npoint = my points -> size;
	for (ipoint= 1; ipoint <= npoint; ipoint ++) {
		long length = TextPoint_labelLength (my points -> item [ipoint]);
		if (length > maximum) maximum = length;
	}
	return maximum;
}

long TextGrid_maximumLabelLength (TextGrid me) {
	long maximum = 0, itier, ntier = my tiers -> size;
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = my tiers -> item [itier];
		long length = anyTier -> methods == (Function_Table) classIntervalTier ?
			IntervalTier_maximumLabelLength ((IntervalTier) anyTier) :
			TextTier_maximumLabelLength ((TextTier) anyTier);
		if (length > maximum) maximum = length;
	}
	return maximum;
}

static int genericize (char **pstring, char *buffer) {
	if (*pstring) {
		const unsigned char *p = (const unsigned char *) *pstring;
		while (*p) {
			if (*p > 126) {   /* Only if necessary. */
				char *newString;
				Longchar_genericize (*pstring, buffer);
				newString = Melder_strdup (buffer); cherror
				/*
				 * Replace string only if copying was OK.
				 */
				Melder_free (*pstring);
				*pstring = newString;
				break;
			}
			p ++;
		}
	}
end:
	iferror return 0;
	return 1;
}

int TextGrid_genericize (TextGrid me) {
	long itier, ntier = my tiers -> size;
	char *buffer = Melder_malloc (TextGrid_maximumLabelLength (me) * 3 + 1); cherror
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Function_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				if (! genericize (& interval -> text, buffer)) goto end;
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long i, n = tier -> points -> size;
			for (i = 1; i <= n; i ++) {
				TextPoint point = tier -> points -> item [i];
				if (! genericize (& point -> mark, buffer)) goto end;
			}
		}
	}
end:
	Melder_free (buffer);
	iferror return 0;
	return 1;
}

int TextGrid_nativize (TextGrid me) {
	long itier, ntier = my tiers -> size;
	char *buffer = Melder_malloc (TextGrid_maximumLabelLength (me) + 1); cherror
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Function_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				if (interval -> text) {
					Longchar_nativize (interval -> text, buffer, FALSE);
					strcpy (interval -> text, buffer);
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long i, n = tier -> points -> size;
			for (i = 1; i <= n; i ++) {
				TextPoint point = tier -> points -> item [i];
				if (point -> mark) {
					Longchar_nativize (point -> mark, buffer, FALSE);
					strcpy (point -> mark, buffer);
				}
			}
		}
	}
end:
	Melder_free (buffer);
	iferror return 0;
	return 1;
}

void TextInterval_removeText (TextInterval me) {
	Melder_free (my text);
}

void TextPoint_removeText (TextPoint me) {
	Melder_free (my mark);
}

void IntervalTier_removeText (IntervalTier me) {
	long iinterval, ninterval = my intervals -> size;
	for (iinterval = 1; iinterval <= ninterval; iinterval ++)
		TextInterval_removeText (my intervals -> item [iinterval]);
}

void TextTier_removeText (TextTier me) {
	long ipoint, npoint = my points -> size;
	for (ipoint = 1; ipoint <= npoint; ipoint ++)
		TextPoint_removeText (my points -> item [ipoint]);
}

int TextGrid_insertBoundary (TextGrid me, int itier, double t) {
	IntervalTier intervalTier;
	TextInterval interval, newInterval;
	long iinterval;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_error ("Cannot add a boundary on tier %d, because that tier does not exist.", itier);
	intervalTier = my tiers -> item [itier];
	if (intervalTier -> methods != classIntervalTier)
		return Melder_error ("Cannot add a boundary on tier %d, because that tier is not an interval tier.", itier);
	if (IntervalTier_hasTime (intervalTier, t))
		return Melder_error ("Cannot add a boundary at %f seconds, because there is already a boundary there.", t);
	iinterval = IntervalTier_timeToIndex (intervalTier, t);
	if (iinterval == 0)
		return Melder_error ("Cannot add a boundary at %f seconds, because this is outside the time domain of the intervals.", t);
	interval = intervalTier -> intervals -> item [iinterval];
	/*
	 * Move the text to the left of the boundary.
	 */
	newInterval = TextInterval_create (t, interval -> xmax, "");
	if (newInterval == NULL) return 0;
	interval -> xmax = t;
	return Collection_addItem (intervalTier -> intervals, newInterval);
}

int TextGrid_setIntervalText (TextGrid me, int itier, long iinterval, const char *text) {
	IntervalTier intervalTier;
	TextInterval interval;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_error ("Cannot modify tier %d, because that tier does not exist.", itier);
	intervalTier = my tiers -> item [itier];
	if (intervalTier -> methods != classIntervalTier)
		return Melder_error ("Cannot modify tier %d, because that tier is not an interval tier.", itier);
	if (iinterval < 1 || iinterval > intervalTier -> intervals -> size)
		return Melder_error ("Cannot modify interval %ld on tier %d, because that interval does not exist.", iinterval, itier);
	interval = intervalTier -> intervals -> item [iinterval];
	return TextInterval_setText (interval, text);
}

int TextGrid_insertPoint (TextGrid me, int itier, double t, const char *mark) {
	TextTier textTier;
	TextPoint newPoint;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_error ("Cannot add a point on tier %d, because that tier does not exist.", itier);
	textTier = my tiers -> item [itier];
	if (textTier -> methods != classTextTier)
		return Melder_error ("Cannot add a point on tier %d, because that tier is not a point tier.", itier);
	if (AnyTier_hasPoint (textTier, t))
		return Melder_error ("Cannot add a point at %f seconds, because there is already a point there.", t);
	newPoint = TextPoint_create (t, mark);
	return Collection_addItem (textTier -> points, newPoint);
}

int TextGrid_setPointText (TextGrid me, int itier, long ipoint, const char *text) {
	TextTier textTier;
	TextPoint point;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_error ("Cannot modify tier %d, because that tier does not exist.", itier);
	textTier = my tiers -> item [itier];
	if (textTier -> methods != classTextTier)
		return Melder_error ("Cannot modify tier %d, because that tier is not a point tier.", itier);
	if (ipoint < 1 || ipoint > textTier -> points -> size)
		return Melder_error ("Cannot modify point %ld on tier %d, because that point does not exist.", ipoint, itier);
	point = textTier -> points -> item [ipoint];
	return TextPoint_setText (point, text);
}

static int sgmlToPraat (char *text) {
	char *sgml = text, *praat = text;
	for (;;) {
		if (*sgml == '\0') break;
		if (*sgml == '&') {
			static struct { char *sgml, *praat; } translations [] = {
				{ "auml", "\\a\"" }, { "euml", "\\e\"" }, { "iuml", "\\i\"" },
				{ "ouml", "\\o\"" }, { "ouml", "\\o\"" },
				{ "Auml", "\\A\"" }, { "Euml", "\\E\"" }, { "Iuml", "\\I\"" },
				{ "Ouml", "\\O\"" }, { "Uuml", "\\U\"" },
				{ "aacute", "\\a'" }, { "eacute", "\\e'" }, { "iacute", "\\i'" },
				{ "oacute", "\\o'" }, { "oacute", "\\o'" },
				{ "Aacute", "\\A'" }, { "Eacute", "\\E'" }, { "Iacute", "\\I'" },
				{ "Oacute", "\\O'" }, { "Uacute", "\\U'" },
				{ "agrave", "\\a`" }, { "egrave", "\\e`" }, { "igrave", "\\i`" },
				{ "ograve", "\\o`" }, { "ograve", "\\o`" },
				{ "Agrave", "\\A`" }, { "Egrave", "\\E`" }, { "Igrave", "\\I`" },
				{ "Ograve", "\\O`" }, { "Ugrave", "\\U`" },
				{ "acirc", "\\a^" }, { "ecirc", "\\e^" }, { "icirc", "\\i^" },
				{ "ocirc", "\\o^" }, { "ocirc", "\\o^" },
				{ "Acirc", "\\A^" }, { "Ecirc", "\\E^" }, { "Icirc", "\\I^" },
				{ "Ocirc", "\\O^" }, { "Ucirc", "\\U^" },
				{ NULL, NULL } };
			char sgmlCode [201];
			int i = 0;
			++ sgml;
			for (i = 0; i < 200; i ++) {
				char sgmlChar = sgml [i];
				if (sgmlChar == ';') {
					if (i == 0) return Melder_error ("Empty SGML code.");
					sgml += i + 1;
					break;
				}
				sgmlCode [i] = sgmlChar;
			}
			if (i >= 200) return Melder_error ("Unfinished SGML code.");
			sgmlCode [i] = '\0';
			for (i = 0; translations [i]. sgml != NULL; i ++) {
				if (strequ (sgmlCode, translations [i]. sgml)) {
					memcpy (praat, translations [i]. praat, strlen (translations [i]. praat));
					praat += strlen (translations [i]. praat);
					break;
				}
			}
			if (translations [i]. sgml == NULL) return Melder_error ("Unknown SGML code &%s;.", sgmlCode);
		} else {
			* praat ++ = * sgml ++;
		}
	}
	*praat = '\0';
	return 1;
}

TextGrid TextGrid_readFromChronologicalTextFile (MelderFile file) {
	TextGrid me = NULL;
	long itier, numberOfTiers;
	char *tag = NULL;
	FILE *f = Melder_fopen (file, "r"); cherror
	tag = ascgets1 (f); cherror
	if (! strequ (tag, "Praat chronological TextGrid text file")) {
		Melder_error ("Not a chronological TextGrid text file.");
		goto end;
	}
	me = new (TextGrid); cherror
	classFunction -> readAscii (me, f); cherror
	my tiers = Ordered_create (); cherror
	numberOfTiers = ascgeti4 (f); cherror
	for (itier = 1; itier <= numberOfTiers; itier ++) {
		char *klas = ascgets1 (f); cherror
		if (strequ (klas, "IntervalTier")) {
			IntervalTier tier = new (IntervalTier); cherror
			Collection_addItem (my tiers, tier); cherror
			tier -> name = ascgets1 (f); cherror
			classFunction -> readAscii (tier, f); cherror
			tier -> intervals = SortedSetOfDouble_create (); cherror
		} else if (strequ (klas, "TextTier")) {
			TextTier tier = new (TextTier); cherror
			Collection_addItem (my tiers, tier); cherror
			tier -> name = ascgets1 (f); cherror
			classFunction -> readAscii (tier, f); cherror
			tier -> points = SortedSetOfDouble_create (); cherror
		} else {
			Melder_error ("Unknown tier class \"%s\".", klas);
			goto end;
		}
		Melder_free (klas);
	}
	for (;;) {
		long itier = ascgeti4 (f);
		iferror {
			if (strstr (Melder_getError (), "Early end of file")) {
				Melder_clearError ();
				break;
			} else {
				goto end;
			}
		}
		if (itier < 1 || itier > my tiers -> size) {
			Melder_error ("Wrong tier number %ld.", itier);
			goto end;
		}
		if (((Data) my tiers -> item [itier]) -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tier = my tiers -> item [itier];
			TextInterval interval = new (TextInterval); cherror
			classTextInterval -> readAscii (interval, f); cherror
			Collection_addItem (tier -> intervals, interval); cherror   /* Not earlier: sorting depends on contents of interval. */
		} else {
			TextTier tier = my tiers -> item [itier];
			TextPoint point = new (TextPoint); cherror
			classTextPoint -> readAscii (point, f); cherror
			Collection_addItem (tier -> points, point); cherror   /* Not earlier: sorting depends on contents of point. */
		}
	}
end:
	Melder_fclose (file, f);
	Melder_free (tag);
	iferror { Melder_error ("(TextGrid_readFromChronologicalTextFile:) File %s not read.", MelderFile_messageName (file)); forget (me); }
	return me;
}

static void writeQuotedString (FILE *f, const char *string) {
	fputc ('\"', f);
	if (string) { char kar; while ((kar = *string ++) != '\0') { fputc (kar, f); if (kar == '\"') fputc (kar, f); } }
	fputc ('\"', f);
}

int TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file) {
	FILE *f = Melder_fopen (file, "w");
	/*
	 * The "elements" (intervals and points) are sorted primarily by time and secondarily by tier.
	 */
	double sortingTime = -1e308;
	long sortingTier = 0, itier, ielement;
	if (! f) return 0;
	ascio_verbose (FALSE);
	ascindent ();
	fprintf (f, "\"Praat chronological TextGrid text file\"");
	fprintf (f, "\n%s %s   ! Time domain.", Melder_double (my xmin), Melder_double (my xmax));
	fprintf (f, "\n%ld   ! Number of tiers.", my tiers -> size);
	for (itier = 1; itier <= my tiers -> size; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		fputc ('\n', f);
		writeQuotedString (f, Thing_className (anyTier));
		fputc (' ', f);
		writeQuotedString (f, anyTier -> name);
		fprintf (f, " %s %s", Melder_double (anyTier -> xmin), Melder_double (anyTier -> xmax));
	}
	for (;;) {
		double firstRemainingTime = +1e308;
		long firstRemainingTier = 2000000000, firstRemainingElement = 0;
		TextPoint firstRemainingPoint = NULL;
		for (itier = 1; itier <= my tiers -> size; itier ++) {
			Data anyTier = my tiers -> item [itier];
			if (anyTier -> methods == (Data_Table) classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				for (ielement = 1; ielement <= tier -> intervals -> size; ielement ++) {
					TextInterval interval = tier -> intervals -> item [ielement];
					if ((interval -> xmin > sortingTime ||   /* Sort primarily by time. */
					     interval -> xmin == sortingTime && itier > sortingTier) &&   /* Sort secondarily by tier number. */
					    (interval -> xmin < firstRemainingTime ||   /* Sort primarily by time. */
					     interval -> xmin == firstRemainingTime && itier < firstRemainingTier))   /* Sort secondarily by tier number. */
					{
						firstRemainingTime = interval -> xmin;
						firstRemainingTier = itier;
						firstRemainingElement = ielement;
					}
				}
			} else {
				TextTier tier = (TextTier) anyTier;
				for (ielement = 1; ielement <= tier -> points -> size; ielement ++) {
					TextPoint point = tier -> points -> item [ielement];
					if ((point -> time > sortingTime ||   /* Sort primarily by time. */
					     point -> time == sortingTime && itier > sortingTier) &&   /* Sort secondarily by tier number. */
					    (point -> time < firstRemainingTime ||   /* Sort primarily by time. */
					     point -> time == firstRemainingTime && itier < firstRemainingTier))   /* Sort secondarily by tier number. */
					{
						firstRemainingTime = point -> time;
						firstRemainingTier = itier;
						firstRemainingElement = ielement;
					}
				}
			}
		}
		if (firstRemainingElement == 0) {
			break;
		} else {
			Data anyTier = my tiers -> item [firstRemainingTier];
			if (anyTier -> methods == (Data_Table) classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				TextInterval interval = tier -> intervals -> item [firstRemainingElement];
				fprintf (f, "\n%ld %s %s ", firstRemainingTier, Melder_double (interval -> xmin), Melder_double (interval -> xmax));
				ascputs4 (interval -> text, f, "");
			} else {
				TextTier tier = (TextTier) anyTier;
				TextPoint point = tier -> points -> item [firstRemainingElement];
				fprintf (f, "\n%ld %s ", firstRemainingTier, Melder_double (point -> time));
				ascputs4 (point -> mark, f, "");
			}
			sortingTime = firstRemainingTime;
			sortingTier = firstRemainingTier;
		}
	}
	ascexdent ();
	if (! Melder_fclose (file, f)) return 0;
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

TextGrid TextGrid_readFromCgnSyntaxFile (MelderFile fs) {
	TextGrid me = new (TextGrid);
	char *line, arg1 [41], arg2 [41], arg3 [41], arg4 [41], arg5 [41], arg6 [41], arg7 [201];
	long startOfData, sentenceNumber = 0;
	int itier;
	double phraseBegin = 0.0, phraseEnd = 0.0;
	IntervalTier sentenceTier = NULL, phraseTier = NULL;
	TextInterval lastInterval = NULL;
	static char phrase [1000];
	if (! me || ! (my tiers = Ordered_create ()))
		{ forget (me); return NULL; }
	MelderFile_open (fs); cherror
	line = MelderFile_readLine (fs);
	if (! strequ (line, "<?xml version=\"1.0\"?>")) { Melder_error ("No CGN syntax file."); goto end; }
	line = MelderFile_readLine (fs);
	if (! strequ (line, "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">")) { Melder_error ("No CGN syntax file."); goto end; }
	line = MelderFile_readLine (fs);
	startOfData = MelderFile_tell (fs);
	/*
	 * Get duration.
	 */
	my xmin = 0.0;
	for (;;) {
		line = MelderFile_readLine (fs);
		if (! line) break;
		if (strnequ (line, "  <tau ref=\"", 12)) {
			if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
				{ Melder_error ("Too few strings in tau line."); goto end; }
			my xmax = atof (arg5 + 4);
		}
	}
	if (my xmax <= 0.0) { Melder_error ("Duration not greater than zero."); goto end; }
	/*
	 * Get number and names of tiers.
	 */
	MelderFile_seek (fs, startOfData, SEEK_SET);
	for (;;) {
		line = MelderFile_readLine (fs);
		if (! line) break;
		if (strnequ (line, "  <tau ref=\"", 12)) {
			char *speakerName;
			int length, speakerTier = 0;
			double tb, te;
			if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
				{ Melder_error ("Too few strings in tau line."); goto end; }
			length = strlen (arg3);
			if (length < 5 || ! strnequ (arg3, "s=\"", 3)) { Melder_error ("Missing speaker name."); goto end; }
			arg3 [length - 1] = '\0';   /* Truncate at double quote. */
			speakerName = arg3 + 3;   /* Truncate leading s=". */
			/*
			 * Does this speaker name occur in the tiers?
			 */
			for (itier = 1; itier <= my tiers -> size; itier ++) {
				IntervalTier tier = my tiers -> item [itier];
				if (strequ (tier -> name, speakerName)) {
					speakerTier = itier;
					break;
				}
			}
			if (speakerTier == 0) {
				/*
				 * Create two new tiers.
				 */
				sentenceTier = new (IntervalTier); cherror
				sentenceTier -> intervals = SortedSetOfDouble_create (); cherror
				sentenceTier -> xmin = 0.0;
				sentenceTier -> xmax = my xmax;
				Thing_setName (sentenceTier, speakerName);
				Collection_addItem (my tiers, sentenceTier); cherror
				phraseTier = new (IntervalTier); cherror
				phraseTier -> intervals = SortedSetOfDouble_create (); cherror
				phraseTier -> xmin = 0.0;
				phraseTier -> xmax = my xmax;
				Thing_setName (phraseTier, speakerName);
				Collection_addItem (my tiers, phraseTier); cherror
			} else {
				sentenceTier = my tiers -> item [speakerTier];
				phraseTier = my tiers -> item [speakerTier + 1];
			}
			tb = atof (arg4 + 4), te = atof (arg5 + 4);
			if (te <= tb) { Melder_error ("Zero duration for sentence."); goto end; }
			/*
			 * We are going to add one or two intervals to the sentence tier.
			 */
			if (sentenceTier -> intervals -> size > 0) {
				TextInterval latestInterval = sentenceTier -> intervals -> item [sentenceTier -> intervals -> size];
				if (tb > latestInterval -> xmax) {
					TextInterval interval = TextInterval_create (latestInterval -> xmax, tb, "");
					Collection_addItem (sentenceTier -> intervals, interval); cherror
				} else if (tb < latestInterval -> xmax) {
					Melder_error ("Overlap on tier not allowed."); goto end;
				}
			} else {
				if (tb > 0.0) {
					TextInterval interval = TextInterval_create (0.0, tb, "");
					Collection_addItem (sentenceTier -> intervals, interval); cherror
				} else if (tb < 0.0) {
					Melder_error ("Negative times not allowed."); goto end;
				}
			}
			{
				TextInterval interval;
				char label [10];
				sprintf (label, "%ld", ++ sentenceNumber);
				interval = TextInterval_create (tb, te, label);
				Collection_addItem (sentenceTier -> intervals, interval); cherror
			}
		} else if (strnequ (line, "    <tw ref=\"", 13)) {
			int length;
			double tb, te;
			if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
				{ Melder_error ("Too few strings in tw line."); goto end; }
			length = strlen (arg3);
			if (length < 6 || ! strnequ (arg3, "tb=\"", 4)) { Melder_error ("Missing tb."); goto end; }
			tb = atof (arg3 + 4);
			length = strlen (arg4);
			if (length < 6 || ! strnequ (arg4, "te=\"", 4)) { Melder_error ("Missing te."); goto end; }
			te = atof (arg4 + 4);
			if (te <= tb) { Melder_error ("Zero duration for phrase."); goto end; }
			if (tb == phraseBegin && te == phraseEnd) {
				/* Append a word. */
				strcat (phrase, " ");
				length = strlen (arg7);
				if (length < 6 || ! strnequ (arg7, "w=\"", 3)) { Melder_error ("Missing word."); goto end; }
				arg7 [length - 3] = '\0';   /* Truncate "/>. */
				strcat (phrase, arg7 + 3);
			} else {
				/* Begin a phrase. */
				if (lastInterval) {
					sgmlToPraat (phrase); cherror
					TextInterval_setText (lastInterval, phrase); cherror
				}
				phrase [0] = '\0';
				length = strlen (arg7);
				if (length < 6 || ! strnequ (arg7, "w=\"", 3)) { Melder_error ("Missing word."); goto end; }
				arg7 [length - 3] = '\0';   /* Truncate "/>. */
				strcat (phrase, arg7 + 3);
				if (phraseTier -> intervals -> size > 0) {
					TextInterval latestInterval = phraseTier -> intervals -> item [phraseTier -> intervals -> size];
					if (tb > latestInterval -> xmax) {
						TextInterval interval = TextInterval_create (latestInterval -> xmax, tb, "");
						Collection_addItem (phraseTier -> intervals, interval); cherror
					} else if (tb < latestInterval -> xmax) {
						Melder_error ("Overlap on tier not allowed."); goto end;
					}
				} else {
					if (tb > 0.0) {
						TextInterval interval = TextInterval_create (0.0, tb, "");
						Collection_addItem (phraseTier -> intervals, interval); cherror
					} else if (tb < 0.0) {
						Melder_error ("Negative times not allowed."); goto end;
					}
				}
				lastInterval = TextInterval_create (tb, te, "");
				if (! phraseTier) { Melder_error ("Phrase outside sentence."); goto end; }
				Collection_addItem (phraseTier -> intervals, lastInterval);
				phraseBegin = tb;
				phraseEnd = te;
			}
		}
	}
	if (lastInterval) {
		sgmlToPraat (phrase); cherror
		TextInterval_setText (lastInterval, phrase); cherror
	}
	for (itier = 1; itier <= my tiers -> size; itier ++) {
		IntervalTier tier = my tiers -> item [itier];
		if (tier -> intervals -> size > 0) {
			TextInterval latestInterval = tier -> intervals -> item [tier -> intervals -> size];
			if (my xmax > latestInterval -> xmax) {
				TextInterval interval = TextInterval_create (latestInterval -> xmax, my xmax, "");
				Collection_addItem (tier -> intervals, interval); cherror
			}
		} else {
			TextInterval interval = TextInterval_create (my xmin, my xmax, "");
			Collection_addItem (tier -> intervals, interval); cherror
		}
	}
end:
	MelderFile_close (fs);
	iferror forget (me);
	return me;
}

/* End of file TextGrid.c */
