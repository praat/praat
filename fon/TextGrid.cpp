/* TextGrid.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

#include "TextGrid.h"
#include "longchar.h"

#include "oo_DESTROY.h"
#include "TextGrid_def.h"
#include "oo_COPY.h"
#include "TextGrid_def.h"
#include "oo_EQUAL.h"
#include "TextGrid_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "TextGrid_def.h"
#include "oo_WRITE_TEXT.h"
#include "TextGrid_def.h"
#include "oo_READ_TEXT.h"
#include "TextGrid_def.h"
#include "oo_WRITE_BINARY.h"
#include "TextGrid_def.h"
#include "oo_READ_BINARY.h"
#include "TextGrid_def.h"
#include "oo_DESCRIPTION.h"
#include "TextGrid_def.h"

#include "TextGrid_extensions.h"

Thing_implement (TextPoint, AnyPoint, 0);

TextPoint TextPoint_create (double time, const wchar_t *mark) {
	try {
		autoTextPoint me = Thing_new (TextPoint);
		my number = time;
		my mark = Melder_wcsdup (mark);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Text point not created.");
	}
}

void TextPoint_setText (TextPoint me, const wchar_t *text) {
	try {
		/*
		 * Be fast if the string pointers are equal.
		 */
		if (text == my mark) return;
		/*
		 * Create a temporary variable (i.e. a temporary pointer to the final string),
		 * in order that 'my text' does not change in case of error.
		 */
		autostring newText = Melder_wcsdup (text);
		Melder_free (my mark);
		my mark = newText.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": text not set.");
	}
}

Thing_implement (TextInterval, Function, 0);

TextInterval TextInterval_create (double tmin, double tmax, const wchar_t *text) {
	try {
		autoTextInterval me = Thing_new (TextInterval);
		my xmin = tmin;
		my xmax = tmax;
		my text = Melder_wcsdup (text);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Text interval not created.");
	}
}

void TextInterval_setText (TextInterval me, const wchar_t *text) {
	try {
		/*
		 * Be fast if the string pointers are equal.
		 */
		if (text == my text) return;
		/*
		 * Create without change.
		 */
		autostring newText = Melder_wcsdup (text);
		/*
		 * Change without error.
		 */
		Melder_free (my text);
		my text = newText.transfer();
	} catch (MelderError) {
		Melder_throw ("Text interval: text not set.");
	}
}

Thing_implement (TextTier, Function, 0);

void structTextTier :: v_shiftX (double xfrom, double xto) {
	TextTier_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= points -> size; i ++) {
		TextPoint point = (TextPoint) points -> item [i];
		NUMshift (& point -> number, xfrom, xto);
	}
}

void structTextTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	TextTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= points -> size; i ++) {
		TextPoint point = (TextPoint) points -> item [i];
		NUMscale (& point -> number, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

TextTier TextTier_create (double tmin, double tmax) {
	try {
		autoTextTier me = Thing_new (TextTier);
		my points = SortedSetOfDouble_create ();
		my xmin = tmin;
		my xmax = tmax;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Point tier not created.");
	}
}

void TextTier_addPoint (TextTier me, double time, const wchar_t *mark) {
	try {
		Collection_addItem (my points, TextPoint_create (time, mark));
	} catch (MelderError) {
		Melder_throw ("Point tier: point not added.");
	}
}

Thing_implement (IntervalTier, Function, 0);

void structIntervalTier :: v_shiftX (double xfrom, double xto) {
	IntervalTier_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= intervals -> size; i ++) {
		TextInterval interval = (TextInterval) intervals -> item [i];
		interval -> v_shiftX (xfrom, xto);
	}
}

void structIntervalTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	IntervalTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= intervals -> size; i ++) {
		TextInterval interval = (TextInterval) intervals -> item [i];
		interval -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

IntervalTier IntervalTier_create (double tmin, double tmax) {
	try {
		autoIntervalTier me = Thing_new (IntervalTier);
		my intervals = SortedSetOfDouble_create ();
		my xmin = tmin;
		my xmax = tmax;
		autoTextInterval interval = TextInterval_create (tmin, tmax, NULL);
		Collection_addItem (my intervals, interval.transfer());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Interval tier not created.");
	}
}

long IntervalTier_timeToLowIndex (IntervalTier me, double t) {
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = (TextInterval) my intervals -> item [i];
		if (t >= interval -> xmin && t < interval -> xmax)
			return i;
	}
	return 0;   // empty tier or very small or large t
}

long IntervalTier_timeToIndex (IntervalTier me, double t) {
	return t == my xmax ? my intervals -> size : IntervalTier_timeToLowIndex (me, t);
}

long IntervalTier_timeToHighIndex (IntervalTier me, double t) {
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = (TextInterval) my intervals -> item [i];
		if (t > interval -> xmin && t <= interval -> xmax)
			return i;
	}
	return 0;   // empty tier or very small or large t
}

long IntervalTier_hasTime (IntervalTier me, double t) {
	for (long iinterval = 1; iinterval <= my intervals -> size; iinterval ++) {
		TextInterval interval = (TextInterval) my intervals -> item [iinterval];
		if (interval -> xmin == t || (iinterval == my intervals -> size && interval -> xmax == t)) {
			return iinterval;   // time found
		}
	}
	return 0;   // time not found
}

long IntervalTier_hasBoundary (IntervalTier me, double t) {
	for (long iinterval = 2; iinterval <= my intervals -> size; iinterval ++) {
		TextInterval interval = (TextInterval) my intervals -> item [iinterval];
		if (interval -> xmin == t) {
			return iinterval;   // boundary found
		}
	}
	return 0;   // boundary not found
}

void structTextGrid :: v_info () {
	long ntier = tiers -> size;
	long numberOfIntervalTiers = 0, numberOfPointTiers = 0, numberOfIntervals = 0, numberOfPoints = 0;
	structData :: v_info ();
	for (long itier = 1; itier <= ntier; itier ++) {
		Function anyTier = (Function) tiers -> item [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			numberOfIntervalTiers += 1;
			numberOfIntervals += tier -> intervals -> size;
		} else {
			TextTier tier = (TextTier) anyTier;
			numberOfPointTiers += 1;
			numberOfPoints += tier -> points -> size;
		}
	}
	MelderInfo_writeLine (L"Number of interval tiers: ", Melder_integer (numberOfIntervalTiers));
	MelderInfo_writeLine (L"Number of point tiers: ", Melder_integer (numberOfPointTiers));
	MelderInfo_writeLine (L"Number of intervals: ", Melder_integer (numberOfIntervals));
	MelderInfo_writeLine (L"Number of points: ", Melder_integer (numberOfPoints));
}

void structTextGrid :: v_shiftX (double xfrom, double xto) {
	TextGrid_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= tiers -> size; i ++) {
		Function tier = (Function) tiers -> item [i];
		tier -> v_shiftX (xfrom, xto);
	}
}

void structTextGrid :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	TextGrid_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= tiers -> size; i ++) {
		Function tier = (Function) tiers -> item [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

Thing_implement (TextGrid, Function, 0);

TextGrid TextGrid_createWithoutTiers (double tmin, double tmax) {
	try {
		autoTextGrid me = Thing_new (TextGrid);
		my tiers = Ordered_create ();
		my xmin = tmin;
		my xmax = tmax;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not created.");
	}
}

TextGrid TextGrid_create (double tmin, double tmax, const wchar_t *tierNames, const wchar_t *pointTiers) {
	try {
		autoTextGrid me = TextGrid_createWithoutTiers (tmin, tmax);
		wchar_t nameBuffer [400], *last;

		/*
		 * Create a number of IntervalTier objects.
		 */
		if (tierNames && tierNames [0]) {
			wcscpy (nameBuffer, tierNames);
			for (wchar_t *tierName = Melder_wcstok (nameBuffer, L" ", & last); tierName != NULL; tierName = Melder_wcstok (NULL, L" ", & last)) {
				autoIntervalTier tier = IntervalTier_create (tmin, tmax);
				Thing_setName (tier.peek(), tierName);
				Collection_addItem (my tiers, tier.transfer());
			}
		}

		/*
		 * Replace some IntervalTier objects with TextTier objects.
		 */
		if (pointTiers && pointTiers [0]) {
			wcscpy (nameBuffer, pointTiers);
			for (wchar_t *tierName = Melder_wcstok (nameBuffer, L" ", & last); tierName != NULL; tierName = Melder_wcstok (NULL, L" ", & last)) {
				for (long itier = 1; itier <= my tiers -> size; itier ++) {
					if (wcsequ (tierName, Thing_getName ((Thing) my tiers -> item [itier]))) {
						autoTextTier tier = TextTier_create (tmin, tmax);
						Thing_setName (tier.peek(), tierName);
						forget (((Function *) my tiers -> item) [itier]);
						my tiers -> item [itier] = tier.transfer();
					}
				}
			}
		}
		if (my tiers -> size == 0)
			Melder_throw ("Cannot create a TextGrid without tiers. Supply at least one tier name.");
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not created.");
	}
}

TextTier TextTier_readFromXwaves (MelderFile file) {
	try {
		char *line;

		autoTextTier me = TextTier_create (0, 100);
		autoMelderFile mfile = MelderFile_open (file);

		/*
		 * Search for a line that starts with '#'.
		 */
		for (;;) {
			line = MelderFile_readLine (file);
			if (line == NULL)
				Melder_throw ("Missing '#' line.");
			if (line [0] == '#') break;
		}

		/*
		 * Read a mark from every line.
		 */
		for (;;) {
			line = MelderFile_readLine (file);
			if (line == NULL) break;   // normal end-of-file
			double time;
			long colour;
			char mark [300];
			if (sscanf (line, "%lf%ld%s", & time, & colour, mark) < 3)   // BUG: buffer overflow
				Melder_throw ("Line too short: \"", line, "\".");
			TextTier_addPoint (me.peek(), time, Melder_peekUtf8ToWcs (mark));
		}

		/*
		 * Fix domain.
		 */
		if (my points -> size) {
			TextPoint point = (TextPoint) my points -> item [1];
			if (point -> number < 0.0) my xmin = point -> number - 1.0;
			point = (TextPoint) my points -> item [my points -> size];
			my xmax = point -> number + 1.0;
		}
		mfile.close ();
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextTier not read from Xwaves file.");
	}
}

void TextGrid_checkSpecifiedTierNumberWithinRange (TextGrid me, long tierNumber) {
	if (tierNumber < 1)
		Melder_throw (me, ": the specified tier number is ", tierNumber, ", but should be at least 1.");
	if (tierNumber > my tiers -> size)
		Melder_throw (me, ": the specified tier number (", tierNumber, ") exceeds my number of tiers (", my tiers -> size, ").");	
}

IntervalTier TextGrid_checkSpecifiedTierIsIntervalTier (TextGrid me, long tierNumber) {
	TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
	Function tier = (Function) my tiers -> item [tierNumber];
	if (tier -> classInfo != classIntervalTier)
		Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
	return (IntervalTier) tier;
}

TextTier TextGrid_checkSpecifiedTierIsPointTier (TextGrid me, long tierNumber) {
	TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
	Function tier = (Function) my tiers -> item [tierNumber];
	if (tier -> classInfo != classTextTier)
		Melder_throw ("Tier ", tierNumber, " is not a point tier.");
	return (TextTier) tier;
}

long TextGrid_countLabels (TextGrid me, long tierNumber, const wchar_t *text) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		long count = 0;
		Function anyTier = (Function) my tiers -> item [tierNumber];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			for (long i = 1; i <= tier -> intervals -> size; i ++) {
				TextInterval segment = (TextInterval) tier -> intervals -> item [i];
				if (segment -> text && wcsequ (segment -> text, text))
					count ++;
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			for (long i = 1; i <= tier -> points -> size; i ++) {
				TextPoint point = (TextPoint) tier -> points -> item [i];
				if (point -> mark && wcsequ (point -> mark, text))
					count ++;
			}
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, ": labels not counted.");
	}
}

void TextGrid_addTier (TextGrid me, Function anyTier) {
	try {
		autoFunction tier = Data_copy (anyTier);
		if (tier -> xmin < my xmin) my xmin = tier -> xmin;
		if (tier -> xmax > my xmax) my xmax = tier -> xmax;
		Collection_addItem (my tiers, tier.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": tier not added.");
	}
}

TextGrid TextGrid_merge (Collection textGrids) {
	try {
		if (textGrids -> size < 1)
			Melder_throw ("Cannot merge zero TextGrid objects.");
		autoTextGrid thee = Data_copy ((TextGrid) textGrids -> item [1]);
		for (long igrid = 2; igrid <= textGrids -> size; igrid ++) {
			TextGrid textGrid = (TextGrid) textGrids -> item [igrid];
			for (long itier = 1; itier <= textGrid -> tiers -> size; itier ++) {
				TextGrid_addTier (thee.peek(), (TextGrid) textGrid -> tiers -> item [itier]);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrids not merged.");
	}
}

static TextGrid _Label_to_TextGrid (Label me, double tmin, double tmax) {
	autoTextGrid thee = TextGrid_createWithoutTiers (tmin, tmax);
	for (long itier = 1; itier <= my size; itier ++) {
		Tier tier = (Tier) my item [itier];
		autoIntervalTier intervalTier = IntervalTier_create (tmin, tmax);
		Collection_addItem (thy tiers, intervalTier.transfer());
		Collection_removeItem (intervalTier -> intervals, 1);
		for (long iinterval = 1; iinterval <= tier -> size; iinterval ++) {
			Autosegment autosegment = (Autosegment) tier -> item [iinterval];
			autoTextInterval textInterval = TextInterval_create (
				iinterval == 1 ? tmin : autosegment -> xmin,
				iinterval == tier -> size ? tmax : autosegment -> xmax,
				autosegment -> name);
			Collection_addItem (intervalTier -> intervals, textInterval.transfer());
		}
	}
	return thee.transfer();
}

TextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, int preserveTimes) {
	try {
		autoTextGrid thee = (TextGrid) Data_copy (me);
		int ntier = my tiers -> size;
		if (tmax <= tmin) return thee.transfer();

		for (long itier = 1; itier <= ntier; itier ++) {
			Function anyTier = (Function) thy tiers -> item [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				long ninterval = tier -> intervals -> size;
				for (long iinterval = ninterval; iinterval >= 1; iinterval --) {
					TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
					if (interval -> xmin >= tmax || interval -> xmax <= tmin) {
						Collection_removeItem (tier -> intervals, iinterval);
					} else {
						if (interval -> xmin < tmin) interval -> xmin = tmin;
						if (interval -> xmax > tmax) interval -> xmax = tmax;
					}
				}
			} else {
				TextTier tier = (TextTier) anyTier;
				long n = tier -> points -> size;
				for (long i = n; i >= 1; i --) {
					TextPoint point = (TextPoint) tier -> points -> item [i];
					if (point -> number < tmin || point -> number > tmax) Collection_removeItem (tier -> points, i);
				}
			}
			anyTier -> xmin = tmin;
			anyTier -> xmax = tmax;
		}
		thy xmin = tmin;
		thy xmax = tmax;
		if (! preserveTimes) Function_shiftXTo (thee.peek(), thy xmin, 0.0);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": part not extracted.");
	}
}

TextGrid Label_to_TextGrid (Label me, double duration) {
	try {
		double tmin = 0.0, tmax = duration;
		if (duration == 0.0) Label_suggestDomain (me, & tmin, & tmax);
	Melder_casual ("%g %g %g", duration,tmin,tmax);
		return _Label_to_TextGrid (me, tmin, tmax);
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TextGrid.");
	}
}

TextGrid Label_Function_to_TextGrid (Label me, Any function) {
	try {
		return _Label_to_TextGrid (me, ((Function) function) -> xmin, ((Function) function) -> xmax);
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TextGrid.");
	}
}

TextTier PointProcess_upto_TextTier (PointProcess me, const wchar_t *text) {
	try {
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		for (long i = 1; i <= my nt; i ++) {
			TextTier_addPoint (thee.peek(), my t [i], text);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TextTier.");
	}
}

PointProcess TextTier_getPoints (TextTier me, const wchar_t *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my points -> size; i ++) {
			TextPoint point = (TextPoint) my points -> item [i];
			if (text && text [0]) {
				if (point -> mark && wcsequ (text, point -> mark))
					PointProcess_addPoint (thee.peek(), point -> number);
			} else {
				if (! point -> mark || ! point -> mark [0])
					PointProcess_addPoint (thee.peek(), point -> number);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": points not converted to PointProcess.");
	}
}

PointProcess IntervalTier_getStartingPoints (IntervalTier me, const wchar_t *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my intervals -> size; i ++) {
			TextInterval interval = (TextInterval) my intervals -> item [i];
			if (text && text [0]) {
				if (interval -> text && wcsequ (text, interval -> text))
					PointProcess_addPoint (thee.peek(), interval -> xmin);
			} else {
				if (! interval -> text || ! interval -> text [0])
					PointProcess_addPoint (thee.peek(), interval -> xmin);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": starting points not gotten.");
	}
}

PointProcess IntervalTier_getEndPoints (IntervalTier me, const wchar_t *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my intervals -> size; i ++) {
			TextInterval interval = (TextInterval) my intervals -> item [i];
			if (text && text [0]) {
				if (interval -> text && wcsequ (text, interval -> text))
					PointProcess_addPoint (thee.peek(), interval -> xmax);
			} else {
				if (! interval -> text || ! interval -> text [0])
					PointProcess_addPoint (thee.peek(), interval -> xmax);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": end points not gotten.");
	}
}

PointProcess IntervalTier_getCentrePoints (IntervalTier me, const wchar_t *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my intervals -> size; i ++) {
			TextInterval interval = (TextInterval) my intervals -> item [i];
			if (text && text [0]) {
				if (interval -> text && wcsequ (text, interval -> text))
					PointProcess_addPoint (thee.peek(), 0.5 * (interval -> xmin + interval -> xmax));
			} else {
				if (! interval -> text || ! interval -> text [0])
					PointProcess_addPoint (thee.peek(), 0.5 * (interval -> xmin + interval -> xmax));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": centre points not gotten.");
	}
}

PointProcess TextGrid_getStartingPoints (TextGrid me, long tierNumber, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
			TextInterval interval = tier -> f_item (iinterval);
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.peek(), interval -> xmin);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": starting points not converted to PointProcess.");
	}
}

PointProcess TextGrid_getEndPoints (TextGrid me, long tierNumber, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
			TextInterval interval = tier -> f_item (iinterval);
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.peek(), interval -> xmax);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": end points not converted to PointProcess.");
	}
}

PointProcess TextGrid_getCentrePoints (TextGrid me, long tierNumber, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
			TextInterval interval = tier -> f_item (iinterval);
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.peek(), 0.5 * (interval -> xmin + interval -> xmax));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": centre points not converted to PointProcess.");
	}
}

PointProcess TextGrid_getPoints (TextGrid me, long tierNumber, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long ipoint = 1; ipoint <= tier -> points -> size; ipoint ++) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.peek(), point -> number);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": points not converted to PointProcess.");
	}
}

PointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase) {
	try {
		autoPointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
		for (long i = 1; i <= point -> nt; i ++) {
			double t = point -> t [i];
			long index = IntervalTier_timeToLowIndex (tier, t);
			if (index) {
				TextInterval interval = (TextInterval) tier -> intervals -> item [index];
				if (interval -> xmin == t)
					PointProcess_addPoint (thee.peek(), (1 - phase) * interval -> xmin + phase * interval -> xmax);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (tier, " & ", point, ": starts of intervals not converted to PointProcess.");
	}
}

PointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase) {
	try {
		autoPointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
		for (long i = 1; i <= point -> nt; i ++) {
			double t = point -> t [i];
			long index = IntervalTier_timeToHighIndex (tier, t);
			if (index) {
				TextInterval interval = (TextInterval) tier -> intervals -> item [index];
				if (interval -> xmax == t)
					PointProcess_addPoint (thee.peek(), (1 - phase) * interval -> xmin + phase * interval -> xmax);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (tier, " & ", point, ": ends of intervals not converted to PointProcess.");
	}
}

TableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, const wchar_t *label) {
	try {
		long n = 0;
		for (long i = 1; i <= my intervals -> size; i ++) {
			TextInterval interval = (TextInterval) my intervals -> item [i];
			if (label == NULL || (label [0] == '\0' && ! interval -> text) || (interval -> text && wcsequ (interval -> text, label)))
				n ++;
		}
		autoTableOfReal thee = TableOfReal_create (n, 3);
		TableOfReal_setColumnLabel (thee.peek(), 1, L"Start");
		TableOfReal_setColumnLabel (thee.peek(), 2, L"End");
		TableOfReal_setColumnLabel (thee.peek(), 3, L"Duration");
		n = 0;
		for (long i = 1; i <= my intervals -> size; i ++) {
			TextInterval interval = (TextInterval) my intervals -> item [i];
			if (label == NULL || (label [0] == '\0' && ! interval -> text) || (interval -> text && wcsequ (interval -> text, label))) {
				n ++;
				TableOfReal_setRowLabel (thee.peek(), n, interval -> text ? interval -> text : L"");
				thy data [n] [1] = interval -> xmin;
				thy data [n] [2] = interval -> xmax;
				thy data [n] [3] = interval -> xmax - interval -> xmin;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TableOfReal.");
	}
}

TableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me) {
	return IntervalTier_downto_TableOfReal (me, NULL);
}

TableOfReal TextTier_downto_TableOfReal (TextTier me, const wchar_t *label) {
	try {
		long n = 0;
		for (long i = 1; i <= my points -> size; i ++) {
			TextPoint point = (TextPoint) my points -> item [i];
			if (label == NULL || (label [0] == '\0' && ! point -> mark) || (point -> mark && wcsequ (point -> mark, label)))
				n ++;
		}
		autoTableOfReal thee = TableOfReal_create (n, 1);
		TableOfReal_setColumnLabel (thee.peek(), 1, L"Time");
		n = 0;
		for (long i = 1; i <= my points -> size; i ++) {
			TextPoint point = (TextPoint) my points -> item [i];
			if (label == NULL || (label [0] == '\0' && ! point -> mark) || (point -> mark && wcsequ (point -> mark, label))) {
				n ++;
				TableOfReal_setRowLabel (thee.peek(), n, point -> mark ? point -> mark : L"");
				thy data [n] [1] = point -> number;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TableOfReal.");
	}
}

TableOfReal TextTier_downto_TableOfReal_any (TextTier me) {
	return TextTier_downto_TableOfReal (me, NULL);
}

static void IntervalTier_addInterval_unsafe (IntervalTier me, double tmin, double tmax, const wchar_t *label) {
	autoTextInterval interval = TextInterval_create (tmin, tmax, label);
	Collection_addItem (my intervals, interval.transfer());
}

IntervalTier IntervalTier_readFromXwaves (MelderFile file) {
	try {
		char *line;
		double lastTime = 0.0;

		autoIntervalTier me = IntervalTier_create (0, 100);
		autoMelderFile mfile = MelderFile_open (file);

		/*
		 * Search for a line that starts with '#'.
		 */
		for (;;) {
			line = MelderFile_readLine (file);
			if (line == NULL)
				Melder_throw ("Missing '#' line.");
			if (line [0] == '#') break;
		}

		/*
		 * Read a mark from every line.
		 */
		for (;;) {
			double time;
			long colour, numberOfElements;
			char mark [300];

			line = MelderFile_readLine (file);
			if (line == NULL) break;   // normal end-of-file
			numberOfElements = sscanf (line, "%lf%ld%s", & time, & colour, mark);
			if (numberOfElements == 0) {
				break;   // an empty line, hopefully at the end
			}
			if (numberOfElements == 1)
				Melder_throw ("Line too short: \"", line, "\".");
			if (numberOfElements == 2)
				mark [0] = '\0';
			if (lastTime == 0.0) {
				TextInterval interval = (TextInterval) my intervals -> item [1];
				interval -> xmax = time;
				TextInterval_setText (interval, Melder_peekUtf8ToWcs (mark));
			} else {
				IntervalTier_addInterval_unsafe (me.peek(), lastTime, time, Melder_peekUtf8ToWcs (mark));
			}
			lastTime = time;
		}

		/*
		 * Fix domain.
		 */
		if (lastTime > 0.0) {
			TextInterval lastInterval = (TextInterval) my intervals -> item [my intervals -> size];
			my xmax = lastInterval -> xmax = lastTime;
		}

		mfile.close ();
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("IntervalTier not read from file ", file, ".");
	}
}

void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		fprintf (f, "separator ;\nnfields 1\n#\n");
		for (long iinterval = 1; iinterval <= my intervals -> size; iinterval ++) {
			TextInterval interval = (TextInterval) my intervals -> item [iinterval];
			fprintf (f, "\t%.6f 26\t%s\n", interval -> xmax, Melder_peekWcsToUtf8 (interval -> text));
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, ": not written to Xwaves file ", file, ".");
	}
}

TextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT) {
	try {
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, L"vuv", NULL);
		Collection_removeItem (((IntervalTier) thy tiers -> item [1]) -> intervals, 1);
		long ipointright;
		double beginVoiceless = my xmin, endVoiceless, halfMeanT = 0.5 * meanT;
		for (long ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
			endVoiceless = my t [ipointleft] - halfMeanT;
			if (endVoiceless <= beginVoiceless) {
				endVoiceless = beginVoiceless;   /* We will use for voiced interval. */
			} else {
				IntervalTier_addInterval_unsafe ((IntervalTier) thy tiers -> item [1], beginVoiceless, endVoiceless, L"U");
			}
			for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
				if (my t [ipointright] - my t [ipointright - 1] > maxT)
					break;
			ipointright --;
			beginVoiceless = my t [ipointright] + halfMeanT;
			if (beginVoiceless > my xmax)
				beginVoiceless = my xmax;
			IntervalTier_addInterval_unsafe ((IntervalTier) thy tiers -> item [1], endVoiceless, beginVoiceless, L"V");
		}
		endVoiceless = my xmax;
		if (endVoiceless > beginVoiceless) {
			IntervalTier_addInterval_unsafe ((IntervalTier) thy tiers -> item [1], beginVoiceless, endVoiceless, L"U");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TextGrid (vuv).");
	}
}

long TextInterval_labelLength (TextInterval me) {
	return my text ? wcslen (my text) : 0;
}

long TextPoint_labelLength (TextPoint me) {
	return my mark ? wcslen (my mark) : 0;
}

long IntervalTier_maximumLabelLength (IntervalTier me) {
	long maximum = 0, ninterval = my intervals -> size;
	for (long iinterval = 1; iinterval <= ninterval; iinterval ++) {
		long length = TextInterval_labelLength ((TextInterval) my intervals -> item [iinterval]);
		if (length > maximum) maximum = length;
	}
	return maximum;
}

long TextTier_maximumLabelLength (TextTier me) {
	long maximum = 0, npoint = my points -> size;
	for (long ipoint= 1; ipoint <= npoint; ipoint ++) {
		long length = TextPoint_labelLength ((TextPoint) my points -> item [ipoint]);
		if (length > maximum) maximum = length;
	}
	return maximum;
}

long TextGrid_maximumLabelLength (TextGrid me) {
	long maximum = 0, ntier = my tiers -> size;
	for (long itier = 1; itier <= ntier; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		long length = anyTier -> classInfo == classIntervalTier ?
			IntervalTier_maximumLabelLength ((IntervalTier) anyTier) :
			TextTier_maximumLabelLength ((TextTier) anyTier);
		if (length > maximum) maximum = length;
	}
	return maximum;
}

static void genericize (wchar_t **pstring, wchar_t *buffer) {
	if (*pstring) {
		const wchar_t *p = (const wchar_t *) *pstring;
		while (*p) {
			if (*p > 126) {   /* Only if necessary. */
				wchar_t *newString;
				Longchar_genericizeW (*pstring, buffer);
				newString = Melder_wcsdup (buffer);
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
}

void TextGrid_genericize (TextGrid me) {
	try {
		long ntier = my tiers -> size;
		autostring buffer = Melder_calloc (wchar_t, TextGrid_maximumLabelLength (me) * 3 + 1);
		for (long itier = 1; itier <= ntier; itier ++) {
			Function anyTier = (Function) my tiers -> item [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				long ninterval = tier -> intervals -> size;
				for (long iinterval = 1; iinterval <= ninterval; iinterval ++) {
					TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
					genericize (& interval -> text, buffer.peek());
				}
			} else {
				TextTier tier = (TextTier) anyTier;
				long n = tier -> points -> size;
				for (long ipoint = 1; ipoint <= n; ipoint ++) {
					TextPoint point = (TextPoint) tier -> points -> item [ipoint];
					genericize (& point -> mark, buffer.peek());
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not converted to backslash trigraphs.");
	}
}

void TextGrid_nativize (TextGrid me) {
	try {
		long ntier = my tiers -> size;
		autostring buffer = Melder_calloc (wchar_t, TextGrid_maximumLabelLength (me) + 1);
		for (long itier = 1; itier <= ntier; itier ++) {
			Function anyTier = (Function) my tiers -> item [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				long ninterval = tier -> intervals -> size;
				for (long iinterval = 1; iinterval <= ninterval; iinterval ++) {
					TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
					if (interval -> text) {
						Longchar_nativizeW (interval -> text, buffer.peek(), FALSE);
						wcscpy (interval -> text, buffer.peek());
					}
				}
			} else {
				TextTier tier = (TextTier) anyTier;
				long n = tier -> points -> size;
				for (long ipoint = 1; ipoint <= n; ipoint ++) {
					TextPoint point = (TextPoint) tier -> points -> item [ipoint];
					if (point -> mark) {
						Longchar_nativizeW (point -> mark, buffer.peek(), FALSE);
						wcscpy (point -> mark, buffer.peek());
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": backslash trigraphs not converted to Unicode.");
	}
}

void TextInterval_removeText (TextInterval me) {
	Melder_free (my text);
}

void TextPoint_removeText (TextPoint me) {
	Melder_free (my mark);
}

void IntervalTier_removeText (IntervalTier me) {
	long ninterval = my intervals -> size;
	for (long iinterval = 1; iinterval <= ninterval; iinterval ++)
		TextInterval_removeText ((TextInterval) my intervals -> item [iinterval]);
}

void TextTier_removeText (TextTier me) {
	long npoint = my points -> size;
	for (long ipoint = 1; ipoint <= npoint; ipoint ++)
		TextPoint_removeText ((TextPoint) my points -> item [ipoint]);
}

void TextGrid_insertBoundary (TextGrid me, int tierNumber, double t) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		IntervalTier intervalTier = (IntervalTier) my tiers -> item [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw ("Cannot add a boundary on tier ", tierNumber, ", because that tier is not an interval tier.");
		if (IntervalTier_hasTime (intervalTier, t))
			Melder_throw ("Cannot add a boundary at ", Melder_fixed (t, 6), " seconds, because there is already a boundary there.");
		long intervalNumber = IntervalTier_timeToIndex (intervalTier, t);
		if (intervalNumber == 0)
			Melder_throw ("Cannot add a boundary at ", Melder_fixed (t, 6), " seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = (TextInterval) intervalTier -> intervals -> item [intervalNumber];
		/*
		 * Move the text to the left of the boundary.
		 */
		autoTextInterval newInterval = TextInterval_create (t, interval -> xmax, L"");
		interval -> xmax = t;
		Collection_addItem (intervalTier -> intervals, newInterval.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": boundary not inserted.");
	}
}

void IntervalTier_removeLeftBoundary (IntervalTier me, long iinterval) {
	try {
		Melder_assert (iinterval > 1);
		Melder_assert (iinterval <= my intervals -> size);
		TextInterval left = (TextInterval) my intervals -> item [iinterval - 1];
		TextInterval right = (TextInterval) my intervals -> item [iinterval];
		/*
		 * Move the text to the left of the boundary.
		 */
		left -> xmax = right -> xmax;   // collapse left and right intervals into left interval
		if (right -> text == NULL) {
			;
		} else if (left -> text == NULL) {
			TextInterval_setText (left, right -> text);
		} else {
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			MelderString_append (& buffer, left -> text, right -> text);
			TextInterval_setText (left, buffer.string);
		}
		Collection_removeItem (my intervals, iinterval);   // remove right interval
	} catch (MelderError) {
		Melder_throw (me, ": left boundary not removed.");
	}
}

void TextGrid_removeBoundaryAtTime (TextGrid me, int tierNumber, double t) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		IntervalTier intervalTier = (IntervalTier) my tiers -> item [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
		if (! IntervalTier_hasTime (intervalTier, t))
			Melder_throw ("There is no boundary at ", t, " seconds.");
		long iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval == 0)
			Melder_throw ("The time of ", t, " seconds is outside the time domain of the intervals.");
		if (iinterval == 1)
			Melder_throw ("The time of ", t, " seconds is at the left edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, iinterval);
	} catch (MelderError) {
		Melder_throw (me, ": boundary not removed.");
	}
}

void TextGrid_setIntervalText (TextGrid me, int tierNumber, long iinterval, const wchar_t *text) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		IntervalTier intervalTier = (IntervalTier) my tiers -> item [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw ("Tier ", tierNumber, " is not an interval tier.");
		if (iinterval < 1 || iinterval > intervalTier -> intervals -> size)
			Melder_throw ("Interval ", iinterval, " does not exist on tier ", tierNumber, ".");
		TextInterval interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
		TextInterval_setText (interval, text);
	} catch (MelderError) {
		Melder_throw (me, ": interval text not set.");
	}
}

void TextGrid_insertPoint (TextGrid me, int tierNumber, double t, const wchar_t *mark) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		TextTier textTier = (TextTier) my tiers -> item [tierNumber];
		if (textTier -> classInfo != classTextTier)
			Melder_throw ("Tier ", tierNumber, " is not a point tier.");
		if (AnyTier_hasPoint (textTier, t))
			Melder_throw ("There is already a point at ", t, " seconds.");
		autoTextPoint newPoint = TextPoint_create (t, mark);
		Collection_addItem (textTier -> points, newPoint.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": point not inserted.");
	}
}

void TextTier_removePoint (TextTier me, long ipoint) {
	Melder_assert (ipoint <= my points -> size);
	Collection_removeItem (my points, ipoint);
}

void TextGrid_setPointText (TextGrid me, int tierNumber, long ipoint, const wchar_t *text) {
	try {
		TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		TextTier textTier = (TextTier) my tiers -> item [tierNumber];
		if (textTier -> classInfo != classTextTier)
			Melder_throw ("Tier ", tierNumber, " is not a point tier.");
		if (ipoint < 1 || ipoint > textTier -> points -> size)
			Melder_throw ("Point ", ipoint, " does not exist on tier ", tierNumber, ".");
		TextPoint point = (TextPoint) textTier -> points -> item [ipoint];
		TextPoint_setText (point, text);
	} catch (MelderError) {
		Melder_throw (me, ": point text not set.");
	}
}

static void sgmlToPraat (char *text) {
	char *sgml = text, *praat = text;
	for (;;) {
		if (*sgml == '\0') break;
		if (*sgml == '&') {
			static struct { const char *sgml, *praat; } translations [] = {
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
					if (i == 0) Melder_throw ("Empty SGML code.");
					sgml += i + 1;
					break;
				}
				sgmlCode [i] = sgmlChar;
			}
			if (i >= 200) Melder_throw ("Unfinished SGML code.");
			sgmlCode [i] = '\0';
			for (i = 0; translations [i]. sgml != NULL; i ++) {
				if (strequ (sgmlCode, translations [i]. sgml)) {
					memcpy (praat, translations [i]. praat, strlen (translations [i]. praat));
					praat += strlen (translations [i]. praat);
					break;
				}
			}
			if (translations [i]. sgml == NULL) Melder_throw ("Unknown SGML code &", sgmlCode, ";.");
		} else {
			* praat ++ = * sgml ++;
		}
	}
	*praat = '\0';
}

TextGrid TextGrid_readFromChronologicalTextFile (MelderFile file) {
	try {
		Thing_version = 0;
		autoMelderReadText text = MelderReadText_createFromFile (file);
		autostring tag = texgetw2 (text.peek());
		if (! wcsequ (tag.peek(), L"Praat chronological TextGrid text file"))
			Melder_throw ("This is not a chronological TextGrid text file.");
		autoTextGrid me = Thing_new (TextGrid);
		my structFunction :: v_readText (text.peek());
		my tiers = Ordered_create ();
		long numberOfTiers = texgeti4 (text.peek());
		for (long itier = 1; itier <= numberOfTiers; itier ++) {
			autostring klas = texgetw2 (text.peek());
			if (wcsequ (klas.peek(), L"IntervalTier")) {
				autoIntervalTier tier = Thing_new (IntervalTier);
				tier -> name = texgetw2 (text.peek());
				tier -> structFunction :: v_readText (text.peek());
				tier -> intervals = SortedSetOfDouble_create ();
				Collection_addItem (my tiers, tier.transfer());
			} else if (wcsequ (klas.peek(), L"TextTier")) {
				autoTextTier tier = Thing_new (TextTier);
				tier -> name = texgetw2 (text.peek());
				tier -> structFunction :: v_readText (text.peek());
				tier -> points = SortedSetOfDouble_create ();
				Collection_addItem (my tiers, tier.transfer());
			} else {
				Melder_throw ("Unknown tier class \"", klas.peek(), L"\".");
			}
		}
		for (;;) {
			long tierNumber;
			try {
				tierNumber = texgeti4 (text.peek());
			} catch (MelderError) {
				if (wcsstr (Melder_getError (), L"Early end of text")) {
					Melder_clearError ();
					break;
				} else {
					throw;
				}
			}
			TextGrid_checkSpecifiedTierNumberWithinRange (me.peek(), tierNumber);
			if (((Function) my tiers -> item [tierNumber]) -> classInfo == classIntervalTier) {
				IntervalTier tier = (IntervalTier) my tiers -> item [tierNumber];
				autoTextInterval interval = Thing_new (TextInterval);
				interval -> v_readText (text.peek());
				Collection_addItem (tier -> intervals, interval.transfer());   // not earlier: sorting depends on contents of interval
			} else {
				TextTier tier = (TextTier) my tiers -> item [tierNumber];
				autoTextPoint point = Thing_new (TextPoint);
				point -> v_readText (text.peek());
				Collection_addItem (tier -> points, point.transfer());   // not earlier: sorting depends on contents of point
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not read from chronological text file ", file, ".");
	}
}

static void writeQuotedString (MelderFile file, const wchar_t *string) {
	MelderFile_writeCharacter (file, '\"');
	if (string) {
		wchar_t kar;
		while ((kar = *string ++) != '\0') {
			MelderFile_writeCharacter (file, kar);
			if (kar == '\"') MelderFile_writeCharacter (file, kar);
		}
	}   // BUG
	MelderFile_writeCharacter (file, '\"');
}

void TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file) {
	try {
		Data_createTextFile (me, file, false);
		autoMelderFile mfile = file;
		/*
		 * The "elements" (intervals and points) are sorted primarily by time and secondarily by tier.
		 */
		double sortingTime = -1e308;
		long sortingTier = 0;
		file -> verbose = false;
		texindent (file);
		MelderFile_write (file, L"\"Praat chronological TextGrid text file\"\n", Melder_double (my xmin), L" ", Melder_double (my xmax),
			L"   ! Time domain.\n", Melder_integer (my tiers -> size), L"   ! Number of tiers.");
		for (long itier = 1; itier <= my tiers -> size; itier ++) {
			Function anyTier = (Function) my tiers -> item [itier];
			MelderFile_write (file, L"\n");
			writeQuotedString (file, Thing_className (anyTier));
			MelderFile_write (file, L" ");
			writeQuotedString (file, anyTier -> name);
			MelderFile_write (file, L" ", Melder_double (anyTier -> xmin), L" ", Melder_double (anyTier -> xmax));
		}
		for (;;) {
			double firstRemainingTime = +1e308;
			long firstRemainingTier = 2000000000, firstRemainingElement = 0;
			for (long itier = 1; itier <= my tiers -> size; itier ++) {
				Function anyTier = (Function) my tiers -> item [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = (IntervalTier) anyTier;
					for (long ielement = 1; ielement <= tier -> intervals -> size; ielement ++) {
						TextInterval interval = (TextInterval) tier -> intervals -> item [ielement];
						if ((interval -> xmin > sortingTime ||   // sort primarily by time
							 (interval -> xmin == sortingTime && itier > sortingTier)) &&   // sort secondarily by tier number
							(interval -> xmin < firstRemainingTime ||   // sort primarily by time
							 (interval -> xmin == firstRemainingTime && itier < firstRemainingTier)))   // sort secondarily by tier number
						{
							firstRemainingTime = interval -> xmin;
							firstRemainingTier = itier;
							firstRemainingElement = ielement;
						}
					}
				} else {
					TextTier tier = (TextTier) anyTier;
					for (long ielement = 1; ielement <= tier -> points -> size; ielement ++) {
						TextPoint point = (TextPoint) tier -> points -> item [ielement];
						if ((point -> number > sortingTime ||   // sort primarily by time
							 (point -> number == sortingTime && itier > sortingTier)) &&   // sort secondarily by tier number
							(point -> number < firstRemainingTime ||   // sort primarily by time
							 (point -> number == firstRemainingTime && itier < firstRemainingTier)))   // sort secondarily by tier number
						{
							firstRemainingTime = point -> number;
							firstRemainingTier = itier;
							firstRemainingElement = ielement;
						}
					}
				}
			}
			if (firstRemainingElement == 0) {
				break;
			} else {
				Function anyTier = (Function) my tiers -> item [firstRemainingTier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = (IntervalTier) anyTier;
					TextInterval interval = (TextInterval) tier -> intervals -> item [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, L"\n\n! ", tier -> name, L":");
					MelderFile_write (file, L"\n", Melder_integer (firstRemainingTier), L" ", Melder_double (interval -> xmin), L" ",
						Melder_double (interval -> xmax));
					texputw4 (file, interval -> text, L"", 0,0,0,0,0);
				} else {
					TextTier tier = (TextTier) anyTier;
					TextPoint point = (TextPoint) tier -> points -> item [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, L"\n\n! ", tier -> name, L":");
					MelderFile_write (file, L"\n", Melder_integer (firstRemainingTier), L" ", Melder_double (point -> number), L" ");
					texputw4 (file, point -> mark, L"", 0,0,0,0,0);
				}
				sortingTime = firstRemainingTime;
				sortingTier = firstRemainingTier;
			}
		}
		texexdent (file);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, ": not written to chronological text file ", file, ".");
	}
}

TextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file) {
	try {
		autoTextGrid me = Thing_new (TextGrid);
		long sentenceNumber = 0;
		double phraseBegin = 0.0, phraseEnd = 0.0;
		IntervalTier sentenceTier = NULL, phraseTier = NULL;
		TextInterval lastInterval = NULL;
		static char phrase [1000];
		my tiers = Ordered_create ();
		autoMelderFile mfile = MelderFile_open (file);
		char *line = MelderFile_readLine (file);
		if (! strequ (line, "<?xml version=\"1.0\"?>"))
			Melder_throw ("This is not a CGN syntax file.");
		line = MelderFile_readLine (file);
		if (! strequ (line, "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">"))
			Melder_throw ("This is not a CGN syntax file.");
		line = MelderFile_readLine (file);
		long startOfData = MelderFile_tell (file);
		/*
		 * Get duration.
		 */
		my xmin = 0.0;
		char arg1 [41], arg2 [41], arg3 [41], arg4 [41], arg5 [41], arg6 [41], arg7 [201];
		for (;;) {
			line = MelderFile_readLine (file);
			if (! line) break;
			if (strnequ (line, "  <tau ref=\"", 12)) {
				if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
					Melder_throw ("Too few strings in tau line.");
				my xmax = atof (arg5 + 4);
			}
		}
		if (my xmax <= 0.0) Melder_throw ("Duration (", my xmax, " seconds) should be greater than zero.");
		/*
		 * Get number and names of tiers.
		 */
		MelderFile_seek (file, startOfData, SEEK_SET);
		for (;;) {
			line = MelderFile_readLine (file);
			if (! line) break;
			if (strnequ (line, "  <tau ref=\"", 12)) {
				char *speakerName;
				int length, speakerTier = 0;
				double tb, te;
				if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
					Melder_throw ("Too few strings in tau line.");
				length = strlen (arg3);
				if (length < 5 || ! strnequ (arg3, "s=\"", 3))
					Melder_throw ("Missing speaker name.");
				arg3 [length - 1] = '\0';   // truncate at double quote
				speakerName = arg3 + 3;   // truncate leading s="
				/*
				 * Does this speaker name occur in the tiers?
				 */
				for (long itier = 1; itier <= my tiers -> size; itier ++) {
					IntervalTier tier = (IntervalTier) my tiers -> item [itier];
					if (wcsequ (tier -> name, Melder_peekUtf8ToWcs (speakerName))) {
						speakerTier = itier;
						break;
					}
				}
				if (speakerTier == 0) {
					/*
					 * Create two new tiers.
					 */
					autoIntervalTier newSentenceTier = Thing_new (IntervalTier);
					newSentenceTier -> intervals = SortedSetOfDouble_create ();
					newSentenceTier -> xmin = 0.0;
					newSentenceTier -> xmax = my xmax;
					Thing_setName (newSentenceTier.peek(), Melder_peekUtf8ToWcs (speakerName));
					sentenceTier = newSentenceTier.peek();   // for later use; this seems safe
					Collection_addItem (my tiers, newSentenceTier.transfer());
					autoIntervalTier newPhraseTier = Thing_new (IntervalTier);
					newPhraseTier -> intervals = SortedSetOfDouble_create ();
					newPhraseTier -> xmin = 0.0;
					newPhraseTier -> xmax = my xmax;
					Thing_setName (newPhraseTier.peek(), Melder_peekUtf8ToWcs (speakerName));
					phraseTier = newPhraseTier.peek();
					Collection_addItem (my tiers, newPhraseTier.transfer());
				} else {
					sentenceTier = (IntervalTier) my tiers -> item [speakerTier];
					phraseTier = (IntervalTier) my tiers -> item [speakerTier + 1];
				}
				tb = atof (arg4 + 4), te = atof (arg5 + 4);
				if (te <= tb)
					Melder_throw ("Zero duration for sentence.");
				/*
				 * We are going to add one or two intervals to the sentence tier.
				 */
				if (sentenceTier -> intervals -> size > 0) {
					TextInterval latestInterval = (TextInterval) sentenceTier -> intervals -> item [sentenceTier -> intervals -> size];
					if (tb > latestInterval -> xmax) {
						autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, L"");
						Collection_addItem (sentenceTier -> intervals, interval.transfer());
					} else if (tb < latestInterval -> xmax) {
						Melder_throw ("Overlap on tier not allowed.");
					}
				} else {
					if (tb > 0.0) {
						TextInterval interval = TextInterval_create (0.0, tb, L"");
						Collection_addItem (sentenceTier -> intervals, interval);
					} else if (tb < 0.0) {
						Melder_throw ("Negative times not allowed.");
					}
				}
				char label [10];
				sprintf (label, "%ld", ++ sentenceNumber);
				autoTextInterval interval = TextInterval_create (tb, te, Melder_peekUtf8ToWcs (label));
				Collection_addItem (sentenceTier -> intervals, interval.transfer());
			} else if (strnequ (line, "    <tw ref=\"", 13)) {
				int length;
				double tb, te;
				if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
					Melder_throw ("Too few strings in tw line.");
				length = strlen (arg3);
				if (length < 6 || ! strnequ (arg3, "tb=\"", 4))
					Melder_throw ("Missing tb.");
				tb = atof (arg3 + 4);
				length = strlen (arg4);
				if (length < 6 || ! strnequ (arg4, "te=\"", 4))
					Melder_throw ("Missing te.");
				te = atof (arg4 + 4);
				if (te <= tb)
					Melder_throw ("Zero duration for phrase.");
				if (tb == phraseBegin && te == phraseEnd) {
					/* Append a word. */
					strcat (phrase, " ");
					length = strlen (arg7);
					if (length < 6 || ! strnequ (arg7, "w=\"", 3))
						Melder_throw ("Missing word.");
					arg7 [length - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
				} else {
					/* Begin a phrase. */
					if (lastInterval) {
						sgmlToPraat (phrase);
						TextInterval_setText (lastInterval, Melder_peekUtf8ToWcs (phrase));
					}
					phrase [0] = '\0';
					length = strlen (arg7);
					if (length < 6 || ! strnequ (arg7, "w=\"", 3))
						Melder_throw ("Missing word.");
					arg7 [length - 3] = '\0';   /* Truncate "/>. */
					strcat (phrase, arg7 + 3);
					if (phraseTier -> intervals -> size > 0) {
						TextInterval latestInterval = (TextInterval) phraseTier -> intervals -> item [phraseTier -> intervals -> size];
						if (tb > latestInterval -> xmax) {
							autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, L"");
							Collection_addItem (phraseTier -> intervals, interval.transfer());
						} else if (tb < latestInterval -> xmax) {
							Melder_throw ("Overlap on tier not allowed.");
						}
					} else {
						if (tb > 0.0) {
							autoTextInterval interval = TextInterval_create (0.0, tb, L"");
							Collection_addItem (phraseTier -> intervals, interval.transfer());
						} else if (tb < 0.0) {
							Melder_throw ("Negative times not allowed.");
						}
					}
					if (! phraseTier)
						Melder_throw ("Phrase outside sentence.");
					autoTextInterval newLastInterval = TextInterval_create (tb, te, L"");
					lastInterval = newLastInterval.peek();
					Collection_addItem (phraseTier -> intervals, newLastInterval.transfer());
					phraseBegin = tb;
					phraseEnd = te;
				}
			}
		}
		if (lastInterval) {
			sgmlToPraat (phrase);
			TextInterval_setText (lastInterval, Melder_peekUtf8ToWcs (phrase));
		}
		for (long itier = 1; itier <= my tiers -> size; itier ++) {
			IntervalTier tier = (IntervalTier) my tiers -> item [itier];
			if (tier -> intervals -> size > 0) {
				TextInterval latestInterval = (TextInterval) tier -> intervals -> item [tier -> intervals -> size];
				if (my xmax > latestInterval -> xmax) {
					autoTextInterval interval = TextInterval_create (latestInterval -> xmax, my xmax, L"");
					Collection_addItem (tier -> intervals, interval.transfer());
				}
			} else {
				autoTextInterval interval = TextInterval_create (my xmin, my xmax, L"");
				Collection_addItem (tier -> intervals, interval.transfer());
			}
		}
		mfile.close ();
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not read from CGN syntax file ", file, ".");
	}
}

Table TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals) {
	long numberOfRows = 0;
	for (long itier = 1; itier <= my tiers -> size; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			if (includeEmptyIntervals) {
				numberOfRows += tier -> intervals -> size;
			} else {
				for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
					TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
					if (interval -> text != NULL && interval -> text [0] != '\0') {
						numberOfRows ++;
					}
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			numberOfRows += tier -> points -> size;
		}
	}
	autoTable thee = Table_createWithoutColumnNames (numberOfRows, 3 + includeLineNumbers + includeTierNames);
	long icol = 0;
	if (includeLineNumbers)
		Table_setColumnLabel (thee.peek(), ++ icol, L"line");
	Table_setColumnLabel (thee.peek(), ++ icol, L"tmin");
	if (includeTierNames)
		Table_setColumnLabel (thee.peek(), ++ icol, L"tier");
	Table_setColumnLabel (thee.peek(), ++ icol, L"text");
	Table_setColumnLabel (thee.peek(), ++ icol, L"tmax");
	long irow = 0;
	for (long itier = 1; itier <= my tiers -> size; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
				TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
				if (includeEmptyIntervals || (interval -> text != NULL && interval -> text [0] != '\0')) {
					++ irow;
					icol = 0;
					if (includeLineNumbers)
						Table_setNumericValue (thee.peek(), irow, ++ icol, irow);
					Table_setStringValue (thee.peek(), irow, ++ icol, Melder_fixed (interval -> xmin, timeDecimals));
					if (includeTierNames)
						Table_setStringValue (thee.peek(), irow, ++ icol, tier -> name);
					Table_setStringValue (thee.peek(), irow, ++ icol, interval -> text);
					Table_setStringValue (thee.peek(), irow, ++ icol, Melder_fixed (interval -> xmax, timeDecimals));
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			for (long ipoint = 1; ipoint <= tier -> points -> size; ipoint ++) {
				TextPoint point = (TextPoint) tier -> points -> item [ipoint];
				++ irow;
				icol = 0;
				if (includeLineNumbers)
					Table_setNumericValue (thee.peek(), irow, ++ icol, irow);
				Table_setStringValue (thee.peek(), irow, ++ icol, Melder_fixed (point -> number, timeDecimals));
				if (includeTierNames)
					Table_setStringValue (thee.peek(), irow, ++ icol, tier -> name);
				Table_setStringValue (thee.peek(), irow, ++ icol, point -> mark);
				Table_setStringValue (thee.peek(), irow, ++ icol, Melder_fixed (point -> number, timeDecimals));
			}
		}
	}
	long columns [1+2] = { 0, 1 + includeLineNumbers, 3 + includeLineNumbers + includeTierNames };   // sort by tmin and tmax
	Table_sortRows_Assert (thee.peek(), columns, 2);
	return thee.transfer();
}

void TextGrid_list (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals) {
	try {
		autoTable table = TextGrid_downto_Table (me, includeLineNumbers, timeDecimals, includeTierNames, includeEmptyIntervals);
		Table_list (table.peek(), false);
	} catch (MelderError) {
		Melder_throw (me, ": not listed.");
	}
}

void TextGrid_correctRoundingErrors (TextGrid me) {
	for (long itier = 1; itier <= my tiers -> size; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			TextInterval first = (TextInterval) tier -> intervals -> item [1];
			first -> xmin = my xmin;
			Melder_assert (first -> xmin < first -> xmax);
			for (long iinterval = 1; iinterval < tier -> intervals -> size; iinterval ++) {
				TextInterval left = (TextInterval) tier -> intervals -> item [iinterval];
				TextInterval right = (TextInterval) tier -> intervals -> item [iinterval + 1];
				right -> xmin = left -> xmax;
				trace ("tier %ld, interval %ld, %17g %17g", itier, iinterval, right -> xmin, right -> xmax);
				Melder_assert (right -> xmin < right -> xmax);
			}
			TextInterval last = (TextInterval) tier -> intervals -> item [tier -> intervals -> size];
			last -> xmax = my xmax;
			Melder_assert (last -> xmax > last -> xmin);
		}
		anyTier -> xmin = my xmin;
		anyTier -> xmax = my xmax;
	}
}

TextGrid TextGrids_concatenate (Collection me) {
	try {
		autoTextGrid thee = TextGrids_to_TextGrid_appendContinuous (me, false);
		TextGrid_correctRoundingErrors (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrids not concatenated.");
	}
}

/* End of file TextGrid.cpp */
