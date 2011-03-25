/* TextGrid.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/03/12 corrected TextGrid_extractPart (could crash)
 * pb 2003/05/09 added extractNonemptyIntervals
 * pb 2004/04/14 TextGrid_merge supports more than two TextGrids
 * pb 2005/02/16 TextGrid_removeBoundaryAtTime
 * pb 2005/03/04 TextGrid_Sound_extractIntervalsWhere
 * pb 2005/06/16 enums -> ints
 * pb 2005/06/22 corrected log scale bug
 * pb 2005/10/07 alignment in TextGrid_Pitch_draw
 * pb 2006/01/01 IntervalTier_removeLeftBoundary, TextTier_removePoint
 * pb 2006/01/25 IntervalTier_writeToXwaves
 * pb 2006/01/29 IntervalTier_readFromXwaves more forgiving
 * pb 2006/02/20 TextGrid_create guards against zero tiers
 * pb 2006/02/24 split TextGrid_create into TextGrid_createWithTiers and TextGrid_createWithoutTiers
 * pb 2007/03/17 domain quantity
 * pb 2007/06/21 tex
 * pb 2007/06/24 canWriteAsAscii
 * pb 2007/07/03 canWriteAsEncoding
 * pb 2007/07/21 corrected chronologicalTextFile
 * pb 2007/08/08 wchar_t
 * pb 2008/09/23 shiftX, scaleX
 * pb 2010/12/08 split off TextGrid_Sound.c
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

class_methods (TextPoint, AnyPoint) {
	class_method_local (TextPoint, destroy)
	class_method_local (TextPoint, copy)
	class_method_local (TextPoint, equal)
	class_method_local (TextPoint, canWriteAsEncoding)
	class_method_local (TextPoint, writeText)
	class_method_local (TextPoint, readText)
	class_method_local (TextPoint, writeBinary)
	class_method_local (TextPoint, readBinary)
	class_method_local (TextPoint, description)
	class_methods_end
}

TextPoint TextPoint_create (double time, const wchar_t *mark) {
	TextPoint me = NULL;
//start:
	me = Thing_new (TextPoint); cherror
	my time = time;
	my mark = Melder_wcsdup_e (mark); cherror
end:
	iferror forget (me);
	return me;
}

int TextPoint_setText (TextPoint me, const wchar_t *text) {
	wchar_t *newText = NULL;
//start:
	/*
	 * Be fast if the string pointers are equal.
	 */
	if (text == my mark) return 1;
	/*
	 * Create a temporary variable (i.e. a temporary pointer to the final string),
	 * in order that 'my text' does not change in case of error.
	 */
	newText = Melder_wcsdup_e (text); cherror
	Melder_free (my mark);
	my mark = newText;
end:
	iferror return 0;
	return 1;
}

class_methods (TextInterval, Function)
	class_method_local (TextInterval, destroy)
	class_method_local (TextInterval, copy)
	class_method_local (TextInterval, equal)
	class_method_local (TextInterval, canWriteAsEncoding)
	class_method_local (TextInterval, writeText)
	class_method_local (TextInterval, readText)
	class_method_local (TextInterval, writeBinary)
	class_method_local (TextInterval, readBinary)
	class_method_local (TextInterval, description)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
class_methods_end

TextInterval TextInterval_create (double tmin, double tmax, const wchar_t *text) {
	TextInterval me = NULL;
//start:
	me = Thing_new (TextInterval); cherror
	my xmin = tmin;
	my xmax = tmax;
	my text = Melder_wcsdup_e (text); cherror
end:
	iferror forget (me);
	return me;
}

int TextInterval_setText (TextInterval me, const wchar_t *text) {
	wchar_t *newText = NULL;
//start:
	/*
	 * Be fast if the string pointers are equal.
	 */
	if (text == my text) return 1;
	/*
	 * Create a temporary variable (i.e. a temporary pointer to the final string),
	 * in order that 'my text' does not change in case of error.
	 */
	newText = Melder_wcsdup_e (text); cherror
	Melder_free (my text);
	my text = newText;
end:
	iferror return 0;
	return 1;
}

static void classTextTier_shiftX (I, double xfrom, double xto) {
	iam (TextTier);
	inherited (TextTier) shiftX (me, xfrom, xto);
	for (long i = 1; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		NUMshift (& point -> time, xfrom, xto);
	}
}

static void classTextTier_scaleX (I, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	iam (TextTier);
	inherited (TextTier) scaleX (me, xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		NUMscale (& point -> time, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

class_methods (TextTier, Function)
	class_method_local (TextTier, destroy)
	class_method_local (TextTier, copy)
	class_method_local (TextTier, equal)
	class_method_local (TextTier, canWriteAsEncoding)
	class_method_local (TextTier, writeText)
	class_method_local (TextTier, readText)
	class_method_local (TextTier, writeBinary)
	class_method_local (TextTier, readBinary)
	class_method_local (TextTier, description)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_method_local (TextTier, shiftX)
	class_method_local (TextTier, scaleX)
class_methods_end

TextTier TextTier_create (double tmin, double tmax) {
	TextTier me = Thing_new (TextTier);
	if (! me || ! (my points = SortedSetOfDouble_create ()))
		{ forget (me); return NULL; }
	my xmin = tmin;
	my xmax = tmax;
	return me;
}

int TextTier_addPoint (TextTier me, double time, const wchar_t *mark) {
	TextPoint point = TextPoint_create (time, mark);
	if (! point || ! Collection_addItem (my points, point)) return 0;
	return 1;
}

static void classIntervalTier_shiftX (I, double xfrom, double xto) {
	iam (IntervalTier);
	inherited (IntervalTier) shiftX (me, xfrom, xto);
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		interval -> methods -> shiftX (interval, xfrom, xto);
	}
}

static void classIntervalTier_scaleX (I, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	iam (IntervalTier);
	inherited (IntervalTier) scaleX (me, xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		interval -> methods -> scaleX (interval, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

class_methods (IntervalTier, Function)
	class_method_local (IntervalTier, destroy)
	class_method_local (IntervalTier, copy)
	class_method_local (IntervalTier, equal)
	class_method_local (IntervalTier, canWriteAsEncoding)
	class_method_local (IntervalTier, writeText)
	class_method_local (IntervalTier, readText)
	class_method_local (IntervalTier, writeBinary)
	class_method_local (IntervalTier, readBinary)
	class_method_local (IntervalTier, description)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_method_local (IntervalTier, shiftX)
	class_method_local (IntervalTier, scaleX)
class_methods_end

IntervalTier IntervalTier_create (double tmin, double tmax) {
	IntervalTier me = Thing_new (IntervalTier);
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
		if (interval -> xmin == t || (iinterval == my intervals -> size && interval -> xmax == t)) {
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
	classData -> info (me);
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
	MelderInfo_writeLine2 (L"Number of interval tiers: ", Melder_integer (numberOfIntervalTiers));
	MelderInfo_writeLine2 (L"Number of point tiers: ", Melder_integer (numberOfPointTiers));
	MelderInfo_writeLine2 (L"Number of intervals: ", Melder_integer (numberOfIntervals));
	MelderInfo_writeLine2 (L"Number of points: ", Melder_integer (numberOfPoints));
}

static void classTextGrid_shiftX (I, double xfrom, double xto) {
	iam (TextGrid);
	inherited (TextGrid) shiftX (me, xfrom, xto);
	for (long i = 1; i <= my tiers -> size; i ++) {
		Function tier = my tiers -> item [i];
		tier -> methods -> shiftX (tier, xfrom, xto);
	}
}

static void classTextGrid_scaleX (I, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	iam (TextGrid);
	inherited (TextGrid) scaleX (me, xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= my tiers -> size; i ++) {
		Function tier = my tiers -> item [i];
		tier -> methods -> scaleX (tier, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

class_methods (TextGrid, Function) {
	class_method_local (TextGrid, destroy)
	class_method_local (TextGrid, copy)
	class_method_local (TextGrid, equal)
	class_method_local (TextGrid, canWriteAsEncoding)
	class_method_local (TextGrid, writeText)
	class_method_local (TextGrid, readText)
	class_method_local (TextGrid, writeBinary)
	class_method_local (TextGrid, readBinary)
	class_method_local (TextGrid, description)
	class_method_local (TextGrid, info)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_method_local (TextGrid, shiftX)
	class_method_local (TextGrid, scaleX)
	class_methods_end
}

TextGrid TextGrid_createWithoutTiers (double tmin, double tmax) {
	TextGrid me = Thing_new (TextGrid);
	if (me == NULL || (my tiers = Ordered_create ()) == NULL)
		{ forget (me); return NULL; }
	my xmin = tmin;
	my xmax = tmax;
	return me;
}

TextGrid TextGrid_create (double tmin, double tmax, const wchar_t *tierNames, const wchar_t *pointTiers) {
	TextGrid me = TextGrid_createWithoutTiers (tmin, tmax);
	wchar_t nameBuffer [400], *tierName, *last;
	if (me == NULL) return NULL;

	/*
	 * Create a number of IntervalTier objects.
	 */
	if (tierNames && tierNames [0]) {
		wcscpy (nameBuffer, tierNames);
		for (tierName = Melder_wcstok (nameBuffer, L" ", & last); tierName != NULL; tierName = Melder_wcstok (NULL, L" ", & last)) {
			IntervalTier tier = IntervalTier_create (tmin, tmax);
			if (! tier || ! Collection_addItem (my tiers, tier)) { forget (me); return NULL; }
			Thing_setName (tier, tierName);
		}
	}

	/*
	 * Replace some IntervalTier objects with TextTier objects.
	 */
	if (pointTiers && pointTiers [0]) {
		wcscpy (nameBuffer, pointTiers);
		for (tierName = Melder_wcstok (nameBuffer, L" ", & last); tierName != NULL; tierName = Melder_wcstok (NULL, L" ", & last)) {
			for (long itier = 1; itier <= my tiers -> size; itier ++) {
				if (wcsequ (tierName, Thing_getName (my tiers -> item [itier]))) {
					TextTier tier = TextTier_create (tmin, tmax);
					if (! tier) { forget (me); return NULL; }
					forget (my tiers -> item [itier]);
					my tiers -> item [itier] = tier;
					Thing_setName (tier, tierName);
				}
			}
		}
	}
	if (my tiers -> size == 0) {
		forget (me);
		return Melder_errorp ("Cannot create a TextGrid without tiers. Supply at least one tier name.");
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
		if (line == NULL)
			error1 (L"No '#' line.")
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
			Melder_error3 (L"Line too short: \"", Melder_peekUtf8ToWcs (line), L"\".");
			goto end;
		}
		if (! TextTier_addPoint (me, time, Melder_peekUtf8ToWcs (mark))) goto end;
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

long TextGrid_countLabels (TextGrid me, long itier, const wchar_t *text) {
	Data anyTier;
	long i, count = 0;
	if (itier < 1 || itier > my tiers -> size) return 0;
	anyTier = my tiers -> item [itier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		for (i = 1; i <= tier -> intervals -> size; i ++) {
			TextInterval segment = tier -> intervals -> item [i];
			if (segment -> text && wcsequ (segment -> text, text))
				count ++;
		}
	} else {
		TextTier tier = (TextTier) anyTier;
		for (i = 1; i <= tier -> points -> size; i ++) {
			TextPoint point = tier -> points -> item [i];
			if (point -> mark && wcsequ (point -> mark, text))
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
	if (textGrids -> size < 1)
		error1 (L"Cannot merge zero TextGrid objects.")
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
	TextGrid thee = TextGrid_createWithoutTiers (tmin, tmax);
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
	if (! preserveTimes) Function_shiftXTo (thee, thy xmin, 0.0);
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

TextTier PointProcess_upto_TextTier (PointProcess me, const wchar_t *text) {
	long i;
	TextTier thee = TextTier_create (my xmin, my xmax);
	if (! thee) return NULL;
	for (i = 1; i <= my nt; i ++)
		if (! TextTier_addPoint (thee, my t [i], text)) { forget (thee); return NULL; }
	return thee;
}

PointProcess TextTier_getPoints (TextTier me, const wchar_t *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	for (long i = 1; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		if (text && text [0]) {
			if (point -> mark && wcsequ (text, point -> mark))
				PointProcess_addPoint (thee, point -> time);
		} else {
			if (! point -> mark || ! point -> mark [0])
				PointProcess_addPoint (thee, point -> time);
		}
	}
	return thee;
}

PointProcess IntervalTier_getStartingPoints (IntervalTier me, const wchar_t *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (text && text [0]) {
			if (interval -> text && wcsequ (text, interval -> text))
				PointProcess_addPoint (thee, interval -> xmin);
		} else {
			if (! interval -> text || ! interval -> text [0])
				PointProcess_addPoint (thee, interval -> xmin);
		}
	}
	return thee;
}

PointProcess IntervalTier_getEndPoints (IntervalTier me, const wchar_t *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (text && text [0]) {
			if (interval -> text && wcsequ (text, interval -> text))
				PointProcess_addPoint (thee, interval -> xmax);
		} else {
			if (! interval -> text || ! interval -> text [0])
				PointProcess_addPoint (thee, interval -> xmax);
		}
	}
	return thee;
}

PointProcess IntervalTier_getCentrePoints (IntervalTier me, const wchar_t *text) {
	PointProcess thee = PointProcess_create (my xmin, my xmax, 10);
	for (long i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (text && text [0]) {
			if (interval -> text && wcsequ (text, interval -> text))
				PointProcess_addPoint (thee, 0.5 * (interval -> xmin + interval -> xmax));
		} else {
			if (! interval -> text || ! interval -> text [0])
				PointProcess_addPoint (thee, 0.5 * (interval -> xmin + interval -> xmax));
		}
	}
	return thee;
}

static int checkTierNumber (TextGrid me, long itier) {
	if (itier < 1)
		error3 (L"The tier number (", Melder_integer (itier), L") should not be smaller than 1.")
	if (itier > my tiers -> size)
		error5 (L"The tier number (", Melder_integer (itier),
			L") should not be larger than the number of tiers (", Melder_integer (my tiers -> size), L").");
end:
	iferror return 0;
	return 1;
}

static IntervalTier checkIntervalTier (TextGrid me, long itier) {
	AnyTier tier = NULL;
	checkTierNumber (me, itier); cherror
	tier = my tiers -> item [itier];
	if (tier -> methods != (AnyTier_Table) classIntervalTier)
		error3 (L"Tier ", Melder_integer (itier), L" should be an interval tier.")
end:
	iferror return NULL;
	return (IntervalTier) tier;
}

static TextTier checkPointTier (TextGrid me, long itier) {
	AnyTier tier = NULL;
	checkTierNumber (me, itier); cherror
	tier = my tiers -> item [itier];
	if (tier -> methods != (AnyTier_Table) classTextTier)
		error3 (L"Tier ", Melder_integer (itier), L" should be a point tier.")
end:
	iferror return NULL;
	return (TextTier) tier;
}

PointProcess TextGrid_getStartingPoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion) {
	PointProcess thee = NULL;
//start:
	IntervalTier tier = checkIntervalTier (me, itier); cherror
	thee = PointProcess_create (my xmin, my xmax, 10); cherror
	for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
		TextInterval interval = tier -> intervals -> item [iinterval];
		if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
			PointProcess_addPoint (thee, interval -> xmin); cherror
		}
	}
end:
	iferror {
		forget (thee);
		Melder_error1 (L"TextGrid: Get starting points...: PointProcess not created.");
	}
	return thee;
}

PointProcess TextGrid_getEndPoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion) {
	PointProcess thee = NULL;
//start:
	IntervalTier tier = checkIntervalTier (me, itier); cherror
	thee = PointProcess_create (my xmin, my xmax, 10); cherror
	for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
		TextInterval interval = tier -> intervals -> item [iinterval];
		if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
			PointProcess_addPoint (thee, interval -> xmax); cherror
		}
	}
end:
	iferror {
		forget (thee);
		Melder_error1 (L"TextGrid: Get end points...: PointProcess not created.");
	}
	return thee;
}

PointProcess TextGrid_getCentrePoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion) {
	PointProcess thee = NULL;
//start:
	IntervalTier tier = checkIntervalTier (me, itier); cherror
	thee = PointProcess_create (my xmin, my xmax, 10);
	for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
		TextInterval interval = tier -> intervals -> item [iinterval];
		if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
			PointProcess_addPoint (thee, 0.5 * (interval -> xmin + interval -> xmax)); cherror
		}
	}
end:
	iferror {
		forget (thee);
		Melder_error1 (L"TextGrid: Get centre points...: PointProcess not created.");
	}
	return thee;
}

PointProcess TextGrid_getPoints (TextGrid me, long itier, int which_Melder_STRING, const wchar_t *criterion) {
	PointProcess thee = NULL;
//start:
	TextTier tier = checkPointTier (me, itier); cherror
	thee = PointProcess_create (my xmin, my xmax, 10); cherror
	for (long ipoint = 1; ipoint <= tier -> points -> size; ipoint ++) {
		TextPoint point = tier -> points -> item [ipoint];
		if (Melder_stringMatchesCriterion (point -> mark, which_Melder_STRING, criterion)) {
			PointProcess_addPoint (thee, point -> time); cherror
		}
	}
end:
	iferror {
		forget (thee);
		Melder_error1 (L"TextGrid: Get end points...: PointProcess not created.");
	}
	return thee;
}

PointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase) {
	PointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
	if (! thee) return NULL;
	for (long i = 1; i <= point -> nt; i ++) {
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
	if (! thee) return NULL;
	for (long i = 1; i <= point -> nt; i ++) {
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

TableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, const wchar_t *label) {
	TableOfReal thee = NULL;
	long i, n = 0;
	for (i = 1; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (label == NULL || (label [0] == '\0' && ! interval -> text) ||
		    (interval -> text && wcsequ (interval -> text, label)))
			n ++;
	}
	thee = TableOfReal_create (n, 3); cherror
	TableOfReal_setColumnLabel (thee, 1, L"Start");
	TableOfReal_setColumnLabel (thee, 2, L"End");
	TableOfReal_setColumnLabel (thee, 3, L"Duration");
	for (i = 1, n = 0; i <= my intervals -> size; i ++) {
		TextInterval interval = my intervals -> item [i];
		if (label == NULL || (label [0] == '\0' && ! interval -> text) ||
		    (interval -> text && wcsequ (interval -> text, label)))
		{
			n ++;
			TableOfReal_setRowLabel (thee, n, interval -> text ? interval -> text : L"");
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

TableOfReal TextTier_downto_TableOfReal (TextTier me, const wchar_t *label) {
	TableOfReal thee = NULL;
	long i, n = 0;
	for (i = 1; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		if (label == NULL || (label [0] == '\0' && ! point -> mark) ||
		    (point -> mark && wcsequ (point -> mark, label)))
			n ++;
	}
	thee = TableOfReal_create (n, 1); cherror
	TableOfReal_setColumnLabel (thee, 1, L"Time");
	for (i = 1, n = 0; i <= my points -> size; i ++) {
		TextPoint point = my points -> item [i];
		if (label == NULL || (label [0] == '\0' && ! point -> mark) ||
		    (point -> mark && wcsequ (point -> mark, label)))
		{
			n ++;
			TableOfReal_setRowLabel (thee, n, point -> mark ? point -> mark : L"");
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

static int IntervalTier_add (IntervalTier me, double tmin, double tmax, const wchar_t *label) {
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
		if (line == NULL)
			error1 (L"No '#' line.")
		if (line [0] == '#') break;
	}

	/*
	 * Read a mark from every line.
	 */
	for (;;) {
		double time;
		long colour, numberOfElements;
		char mark [300];

		line = MelderFile_readLine (file); cherror
		if (line == NULL) break;   /* Normal end-of-file. */
		numberOfElements = sscanf (line, "%lf%ld%s", & time, & colour, mark);
		if (numberOfElements == 0) {
			break;   /* An empty line, hopefully at the end. */
		}
		if (numberOfElements == 1) {
			Melder_error3 (L"Line too short: \"", Melder_peekUtf8ToWcs (line), L"\".");
			goto end;
		}
		if (numberOfElements == 2) {
			mark [0] = '\0';
		}
		if (lastTime == 0.0) {
			TextInterval interval = my intervals -> item [1];
			interval -> xmax = time;
			if (! TextInterval_setText (interval, Melder_peekUtf8ToWcs (mark))) goto end;
		} else {
			if (! IntervalTier_add (me, lastTime, time, Melder_peekUtf8ToWcs (mark))) goto end;
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

int IntervalTier_writeToXwaves (IntervalTier me, MelderFile file) {
	FILE *f = Melder_fopen (file, "w");
	long iinterval;
	if (! f) return 0;
	fprintf (f, "separator ;\nnfields 1\n#\n");
	for (iinterval = 1; iinterval <= my intervals -> size; iinterval ++) {
		TextInterval interval = (TextInterval) my intervals -> item [iinterval];
		fprintf (f, "\t%.6f 26\t%s\n", interval -> xmax, Melder_peekWcsToUtf8 (interval -> text));
	}
	if (! Melder_fclose (file, f)) return 0;
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

TextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT) {
	TextGrid thee = TextGrid_create (my xmin, my xmax, L"vuv", NULL);
	long ipointleft, ipointright;
	double beginVoiceless = my xmin, endVoiceless, halfMeanT = 0.5 * meanT;
	Collection_removeItem (((IntervalTier) thy tiers -> item [1]) -> intervals, 1);
	for (ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
		endVoiceless = my t [ipointleft] - halfMeanT;
		if (endVoiceless <= beginVoiceless) {
			endVoiceless = beginVoiceless;   /* We will use for voiced interval. */
		} else {
			IntervalTier_add (thy tiers -> item [1], beginVoiceless, endVoiceless, L"U"); cherror
		}
		for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
			if (my t [ipointright] - my t [ipointright - 1] > maxT)
				break;
		ipointright --;
		beginVoiceless = my t [ipointright] + halfMeanT;
		if (beginVoiceless > my xmax)
			beginVoiceless = my xmax;
		IntervalTier_add (thy tiers -> item [1], endVoiceless, beginVoiceless, L"V"); cherror
	}
	endVoiceless = my xmax;
	if (endVoiceless > beginVoiceless) {
		IntervalTier_add (thy tiers -> item [1], beginVoiceless, endVoiceless, L"U"); cherror
	}
end:
	iferror forget (thee);
	return thee;
}

long TextInterval_labelLength (TextInterval me) {
	return my text ? wcslen (my text) : 0;
}

long TextPoint_labelLength (TextPoint me) {
	return my mark ? wcslen (my mark) : 0;
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

static int genericize (wchar_t **pstring, wchar_t *buffer) {
	if (*pstring) {
		const wchar_t *p = (const wchar_t *) *pstring;
		while (*p) {
			if (*p > 126) {   /* Only if necessary. */
				wchar_t *newString;
				Longchar_genericizeW (*pstring, buffer);
				newString = Melder_wcsdup_e (buffer); cherror
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
	wchar_t *buffer = NULL;
//start:
	long ntier = my tiers -> size;
	buffer = Melder_calloc_e (wchar_t, TextGrid_maximumLabelLength (me) * 3 + 1); cherror
	for (long itier = 1; itier <= ntier; itier ++) {
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
	wchar_t *buffer = NULL;
//start:
	long ntier = my tiers -> size;
	buffer = Melder_calloc_e (wchar_t, TextGrid_maximumLabelLength (me) + 1); cherror
	for (long itier = 1; itier <= ntier; itier ++) {
		Function anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Function_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			long iinterval, ninterval = tier -> intervals -> size;
			for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				if (interval -> text) {
					Longchar_nativizeW (interval -> text, buffer, FALSE);
					wcscpy (interval -> text, buffer);
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			long i, n = tier -> points -> size;
			for (i = 1; i <= n; i ++) {
				TextPoint point = tier -> points -> item [i];
				if (point -> mark) {
					Longchar_nativizeW (point -> mark, buffer, FALSE);
					wcscpy (point -> mark, buffer);
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
		return Melder_error3 (L"Cannot add a boundary on tier ", Melder_integer (itier), L", because that tier does not exist.");
	intervalTier = my tiers -> item [itier];
	if (intervalTier -> methods != classIntervalTier)
		return Melder_error3 (L"Cannot add a boundary on tier ", Melder_integer (itier), L", because that tier is not an interval tier.");
	if (IntervalTier_hasTime (intervalTier, t))
		return Melder_error3 (L"Cannot add a boundary at ", Melder_fixed (t, 6), L" seconds, because there is already a boundary there.");
	iinterval = IntervalTier_timeToIndex (intervalTier, t);
	if (iinterval == 0)
		return Melder_error ("Cannot add a boundary at %f seconds, because this is outside the time domain of the intervals.", t);
	interval = intervalTier -> intervals -> item [iinterval];
	/*
	 * Move the text to the left of the boundary.
	 */
	newInterval = TextInterval_create (t, interval -> xmax, L"");
	if (newInterval == NULL) return 0;
	interval -> xmax = t;
	return Collection_addItem (intervalTier -> intervals, newInterval);
}

int IntervalTier_removeLeftBoundary (IntervalTier me, long iinterval) {
	TextInterval left, right;
	Melder_assert (iinterval > 1);
	Melder_assert (iinterval <= my intervals -> size);
	left = my intervals -> item [iinterval - 1];
	right = my intervals -> item [iinterval];
	/*
	 * Move the text to the left of the boundary.
	 */
	left -> xmax = right -> xmax;   /* Collapse left and right intervals into left interval. */
	if (right -> text == NULL) {
		;
	} else if (left -> text == NULL) {
		TextInterval_setText (left, right -> text);
	} else {
		static MelderString buffer = { 0 };
		MelderString_empty (& buffer);
		MelderString_append2 (& buffer, left -> text, right -> text);
		if (! TextInterval_setText (left, buffer.string)) { return 0; }
	}
	Collection_removeItem (my intervals, iinterval);   /* Remove right interval. */
	return 1;
}

int TextGrid_removeBoundaryAtTime (TextGrid me, int itier, double t) {
	IntervalTier intervalTier;
	long iinterval;
	if (itier < 1 || itier > my tiers -> size)
		return Melder_error ("Cannot remove a boundary from tier %d, because that tier does not exist.", itier);
	intervalTier = my tiers -> item [itier];
	if (intervalTier -> methods != classIntervalTier)
		return Melder_error ("Cannot remove a boundary from tier %d, because that tier is not an interval tier.", itier);
	if (! IntervalTier_hasTime (intervalTier, t))
		return Melder_error ("Cannot remove a boundary at %f seconds, because there is no boundary there.", t);
	iinterval = IntervalTier_timeToIndex (intervalTier, t);
	if (iinterval == 0)
		return Melder_error ("Cannot remove a boundary at %f seconds, because this is outside the time domain of the intervals.", t);
	if (iinterval == 1)
		return Melder_error ("Cannot remove a boundary at %f seconds, because this is at the left edge of the tier.", t);
	return IntervalTier_removeLeftBoundary (intervalTier, iinterval);
}

int TextGrid_setIntervalText (TextGrid me, int itier, long iinterval, const wchar_t *text) {
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

int TextGrid_insertPoint (TextGrid me, int itier, double t, const wchar_t *mark) {
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

void TextTier_removePoint (TextTier me, long ipoint) {
	Melder_assert (ipoint <= my points -> size);
	Collection_removeItem (my points, ipoint);
}

int TextGrid_setPointText (TextGrid me, int itier, long ipoint, const wchar_t *text) {
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
					if (i == 0) return Melder_error1 (L"Empty SGML code.");
					sgml += i + 1;
					break;
				}
				sgmlCode [i] = sgmlChar;
			}
			if (i >= 200) return Melder_error1 (L"Unfinished SGML code.");
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
	wchar_t *tag = NULL;
	MelderReadText text = MelderReadText_createFromFile (file); cherror
	tag = texgetw2 (text); cherror
	if (! wcsequ (tag, L"Praat chronological TextGrid text file"))
		error1 (L"Not a chronological TextGrid text file.")
	me = Thing_new (TextGrid); cherror
	classFunction -> readText (me, text); cherror
	my tiers = Ordered_create (); cherror
	long numberOfTiers = texgeti4 (text); cherror
	for (long itier = 1; itier <= numberOfTiers; itier ++) {
		wchar_t *klas = texgetw2 (text); cherror
		if (wcsequ (klas, L"IntervalTier")) {
			IntervalTier tier = Thing_new (IntervalTier); cherror
			Collection_addItem (my tiers, tier); cherror
			tier -> name = texgetw2 (text); cherror
			classFunction -> readText (tier, text); cherror
			tier -> intervals = SortedSetOfDouble_create (); cherror
		} else if (wcsequ (klas, L"TextTier")) {
			TextTier tier = Thing_new (TextTier); cherror
			Collection_addItem (my tiers, tier); cherror
			tier -> name = texgetw2 (text); cherror
			classFunction -> readText (tier, text); cherror
			tier -> points = SortedSetOfDouble_create (); cherror
		} else {
			error3 (L"Unknown tier class \"", klas, L"\".")
		}
		Melder_free (klas);
	}
	for (;;) {
		long itier = texgeti4 (text);
		iferror {
			if (wcsstr (Melder_getError (), L"Early end of text")) {
				Melder_clearError ();
				break;
			} else {
				goto end;
			}
		}
		if (itier < 1 || itier > my tiers -> size)
			error3 (L"Wrong tier number ", Melder_integer (itier), L".")
		if (((Data) my tiers -> item [itier]) -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tier = my tiers -> item [itier];
			TextInterval interval = Thing_new (TextInterval); cherror
			classTextInterval -> readText (interval, text); cherror
			Collection_addItem (tier -> intervals, interval); cherror   /* Not earlier: sorting depends on contents of interval. */
		} else {
			TextTier tier = my tiers -> item [itier];
			TextPoint point = Thing_new (TextPoint); cherror
			classTextPoint -> readText (point, text); cherror
			Collection_addItem (tier -> points, point); cherror   /* Not earlier: sorting depends on contents of point. */
		}
	}
end:
	MelderReadText_delete (text);
	Melder_free (tag);
	iferror { Melder_error3 (L"(TextGrid_readFromChronologicalTextFile:) File ", MelderFile_messageName (file), L" not read."); forget (me); }
	return me;
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

int TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file) {
	Data_createTextFile (me, file, false); cherror
	/*
	 * The "elements" (intervals and points) are sorted primarily by time and secondarily by tier.
	 */
	double sortingTime = -1e308;
	long sortingTier = 0, itier, ielement;
	file -> verbose = false;
	texindent (file);
	MelderFile_write7 (file, L"\"Praat chronological TextGrid text file\"\n", Melder_double (my xmin), L" ", Melder_double (my xmax),
		L"   ! Time domain.\n", Melder_integer (my tiers -> size), L"   ! Number of tiers.");
	for (itier = 1; itier <= my tiers -> size; itier ++) {
		Function anyTier = (Function) my tiers -> item [itier];
		MelderFile_write1 (file, L"\n");
		writeQuotedString (file, Thing_className (anyTier));
		MelderFile_write1 (file, L" ");
		writeQuotedString (file, anyTier -> name);
		MelderFile_write4 (file, L" ", Melder_double (anyTier -> xmin), L" ", Melder_double (anyTier -> xmax));
	}
	for (;;) {
		double firstRemainingTime = +1e308;
		long firstRemainingTier = 2000000000, firstRemainingElement = 0;
		for (itier = 1; itier <= my tiers -> size; itier ++) {
			Data anyTier = my tiers -> item [itier];
			if (anyTier -> methods == (Data_Table) classIntervalTier) {
				IntervalTier tier = (IntervalTier) anyTier;
				for (ielement = 1; ielement <= tier -> intervals -> size; ielement ++) {
					TextInterval interval = tier -> intervals -> item [ielement];
					if ((interval -> xmin > sortingTime ||   /* Sort primarily by time. */
					     (interval -> xmin == sortingTime && itier > sortingTier)) &&   /* Sort secondarily by tier number. */
					    (interval -> xmin < firstRemainingTime ||   /* Sort primarily by time. */
					     (interval -> xmin == firstRemainingTime && itier < firstRemainingTier)))   /* Sort secondarily by tier number. */
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
					     (point -> time == sortingTime && itier > sortingTier)) &&   /* Sort secondarily by tier number. */
					    (point -> time < firstRemainingTime ||   /* Sort primarily by time. */
					     (point -> time == firstRemainingTime && itier < firstRemainingTier)))   /* Sort secondarily by tier number. */
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
				MelderFile_write6 (file, L"\n", Melder_integer (firstRemainingTier), L" ", Melder_double (interval -> xmin), L" ",
					Melder_double (interval -> xmax));
				texputw4 (file, interval -> text, L"", 0,0,0,0,0);
			} else {
				TextTier tier = (TextTier) anyTier;
				TextPoint point = tier -> points -> item [firstRemainingElement];
				MelderFile_write5 (file, L"\n", Melder_integer (firstRemainingTier), L" ", Melder_double (point -> time), L" ");
				texputw4 (file, point -> mark, L"", 0,0,0,0,0);
			}
			sortingTime = firstRemainingTime;
			sortingTier = firstRemainingTier;
		}
	}
	texexdent (file);
end:
	MelderFile_close (file);
	iferror return 0;
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

TextGrid TextGrid_readFromCgnSyntaxFile (MelderFile fs) {
	TextGrid me = Thing_new (TextGrid);
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
	if (! strequ (line, "<?xml version=\"1.0\"?>")) error1 (L"No CGN syntax file.")
	line = MelderFile_readLine (fs);
	if (! strequ (line, "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">")) error1 (L"No CGN syntax file.")
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
				error1 (L"Too few strings in tau line.")
			my xmax = atof (arg5 + 4);
		}
	}
	if (my xmax <= 0.0) error1 (L"Duration not greater than zero.")
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
				error1 (L"Too few strings in tau line.")
			length = strlen (arg3);
			if (length < 5 || ! strnequ (arg3, "s=\"", 3))
				error1 (L"Missing speaker name.")
			arg3 [length - 1] = '\0';   /* Truncate at double quote. */
			speakerName = arg3 + 3;   /* Truncate leading s=". */
			/*
			 * Does this speaker name occur in the tiers?
			 */
			for (itier = 1; itier <= my tiers -> size; itier ++) {
				IntervalTier tier = my tiers -> item [itier];
				if (wcsequ (tier -> name, Melder_peekUtf8ToWcs (speakerName))) {
					speakerTier = itier;
					break;
				}
			}
			if (speakerTier == 0) {
				/*
				 * Create two new tiers.
				 */
				sentenceTier = Thing_new (IntervalTier); cherror
				sentenceTier -> intervals = SortedSetOfDouble_create (); cherror
				sentenceTier -> xmin = 0.0;
				sentenceTier -> xmax = my xmax;
				Thing_setName (sentenceTier, Melder_peekUtf8ToWcs (speakerName));
				Collection_addItem (my tiers, sentenceTier); cherror
				phraseTier = Thing_new (IntervalTier); cherror
				phraseTier -> intervals = SortedSetOfDouble_create (); cherror
				phraseTier -> xmin = 0.0;
				phraseTier -> xmax = my xmax;
				Thing_setName (phraseTier, Melder_peekUtf8ToWcs (speakerName));
				Collection_addItem (my tiers, phraseTier); cherror
			} else {
				sentenceTier = my tiers -> item [speakerTier];
				phraseTier = my tiers -> item [speakerTier + 1];
			}
			tb = atof (arg4 + 4), te = atof (arg5 + 4);
			if (te <= tb)
				error1 (L"Zero duration for sentence.")
			/*
			 * We are going to add one or two intervals to the sentence tier.
			 */
			if (sentenceTier -> intervals -> size > 0) {
				TextInterval latestInterval = sentenceTier -> intervals -> item [sentenceTier -> intervals -> size];
				if (tb > latestInterval -> xmax) {
					TextInterval interval = TextInterval_create (latestInterval -> xmax, tb, L"");
					Collection_addItem (sentenceTier -> intervals, interval); cherror
				} else if (tb < latestInterval -> xmax) {
					error1 (L"Overlap on tier not allowed.")
				}
			} else {
				if (tb > 0.0) {
					TextInterval interval = TextInterval_create (0.0, tb, L"");
					Collection_addItem (sentenceTier -> intervals, interval); cherror
				} else if (tb < 0.0) {
					error1 (L"Negative times not allowed.")
				}
			}
			{
				TextInterval interval;
				char label [10];
				sprintf (label, "%ld", ++ sentenceNumber);
				interval = TextInterval_create (tb, te, Melder_peekUtf8ToWcs (label));
				Collection_addItem (sentenceTier -> intervals, interval); cherror
			}
		} else if (strnequ (line, "    <tw ref=\"", 13)) {
			int length;
			double tb, te;
			if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
				error1 (L"Too few strings in tw line.")
			length = strlen (arg3);
			if (length < 6 || ! strnequ (arg3, "tb=\"", 4))
				error1 (L"Missing tb.")
			tb = atof (arg3 + 4);
			length = strlen (arg4);
			if (length < 6 || ! strnequ (arg4, "te=\"", 4))
				error1 (L"Missing te.")
			te = atof (arg4 + 4);
			if (te <= tb)
				error1 (L"Zero duration for phrase.")
			if (tb == phraseBegin && te == phraseEnd) {
				/* Append a word. */
				strcat (phrase, " ");
				length = strlen (arg7);
				if (length < 6 || ! strnequ (arg7, "w=\"", 3))
					error1 (L"Missing word.")
				arg7 [length - 3] = '\0';   /* Truncate "/>. */
				strcat (phrase, arg7 + 3);
			} else {
				/* Begin a phrase. */
				if (lastInterval) {
					sgmlToPraat (phrase); cherror
					TextInterval_setText (lastInterval, Melder_peekUtf8ToWcs (phrase)); cherror
				}
				phrase [0] = '\0';
				length = strlen (arg7);
				if (length < 6 || ! strnequ (arg7, "w=\"", 3))
					error1 (L"Missing word.")
				arg7 [length - 3] = '\0';   /* Truncate "/>. */
				strcat (phrase, arg7 + 3);
				if (phraseTier -> intervals -> size > 0) {
					TextInterval latestInterval = phraseTier -> intervals -> item [phraseTier -> intervals -> size];
					if (tb > latestInterval -> xmax) {
						TextInterval interval = TextInterval_create (latestInterval -> xmax, tb, L"");
						Collection_addItem (phraseTier -> intervals, interval); cherror
					} else if (tb < latestInterval -> xmax) {
						error1 (L"Overlap on tier not allowed.")
					}
				} else {
					if (tb > 0.0) {
						TextInterval interval = TextInterval_create (0.0, tb, L"");
						Collection_addItem (phraseTier -> intervals, interval); cherror
					} else if (tb < 0.0) {
						error1 (L"Negative times not allowed.")
					}
				}
				lastInterval = TextInterval_create (tb, te, L"");
				if (! phraseTier)
					error1 (L"Phrase outside sentence.")
				Collection_addItem (phraseTier -> intervals, lastInterval);
				phraseBegin = tb;
				phraseEnd = te;
			}
		}
	}
	if (lastInterval) {
		sgmlToPraat (phrase); cherror
		TextInterval_setText (lastInterval, Melder_peekUtf8ToWcs (phrase)); cherror
	}
	for (itier = 1; itier <= my tiers -> size; itier ++) {
		IntervalTier tier = my tiers -> item [itier];
		if (tier -> intervals -> size > 0) {
			TextInterval latestInterval = tier -> intervals -> item [tier -> intervals -> size];
			if (my xmax > latestInterval -> xmax) {
				TextInterval interval = TextInterval_create (latestInterval -> xmax, my xmax, L"");
				Collection_addItem (tier -> intervals, interval); cherror
			}
		} else {
			TextInterval interval = TextInterval_create (my xmin, my xmax, L"");
			Collection_addItem (tier -> intervals, interval); cherror
		}
	}
end:
	MelderFile_close (fs);
	iferror forget (me);
	return me;
}

Table TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals)
{
	long numberOfRows = 0;
	for (long itier = 1; itier <= my tiers -> size; itier ++) {
		Data anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			if (includeEmptyIntervals) {
				numberOfRows += tier -> intervals -> size;
			} else {
				for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
					TextInterval interval = tier -> intervals -> item [iinterval];
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
	Table thee = Table_createWithoutColumnNames (numberOfRows,
		3 + includeLineNumbers + includeTierNames); cherror
	long icol = 0;
	if (includeLineNumbers) { Table_setColumnLabel (thee, ++ icol, L"line"); cherror }
	Table_setColumnLabel (thee, ++ icol, L"tmin"); cherror
	if (includeTierNames) { Table_setColumnLabel (thee, ++ icol, L"tier"); cherror }
	Table_setColumnLabel (thee, ++ icol, L"text"); cherror
	Table_setColumnLabel (thee, ++ icol, L"tmax"); cherror
	long irow = 0;
	for (long itier = 1; itier <= my tiers -> size; itier ++) {
		Data anyTier = my tiers -> item [itier];
		if (anyTier -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tier = (IntervalTier) anyTier;
			for (long iinterval = 1; iinterval <= tier -> intervals -> size; iinterval ++) {
				TextInterval interval = tier -> intervals -> item [iinterval];
				if (includeEmptyIntervals || (interval -> text != NULL && interval -> text [0] != '\0')) {
					++ irow;
					icol = 0;
					if (includeLineNumbers) { Table_setNumericValue (thee, irow, ++ icol, irow); cherror }
					Table_setStringValue (thee, irow, ++ icol, Melder_fixed (interval -> xmin, timeDecimals)); cherror
					if (includeTierNames) { Table_setStringValue (thee, irow, ++ icol, tier -> name); cherror }
					Table_setStringValue (thee, irow, ++ icol, interval -> text); cherror
					Table_setStringValue (thee, irow, ++ icol, Melder_fixed (interval -> xmax, timeDecimals)); cherror
				}
			}
		} else {
			TextTier tier = (TextTier) anyTier;
			for (long ipoint = 1; ipoint <= tier -> points -> size; ipoint ++) {
				TextPoint point = tier -> points -> item [ipoint];
				++ irow;
				icol = 0;
				if (includeLineNumbers) { Table_setNumericValue (thee, irow, ++ icol, irow); cherror }
				Table_setStringValue (thee, irow, ++ icol, Melder_fixed (point -> time, timeDecimals)); cherror
				if (includeTierNames) { Table_setStringValue (thee, irow, ++ icol, tier -> name); cherror }
				Table_setStringValue (thee, irow, ++ icol, point -> mark); cherror
				Table_setStringValue (thee, irow, ++ icol, Melder_fixed (point -> time, timeDecimals)); cherror
			}
		}
	}
	long columns [1+2] = { 0, 1 + includeLineNumbers, 3 + includeLineNumbers + includeTierNames };   // sort by tmin and tmax
	Table_sortRows_Assert (thee, columns, 2);
end:
	iferror forget (thee);
	return thee;
}

void TextGrid_list (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals) {
	Table table = TextGrid_downto_Table (me, includeLineNumbers, timeDecimals, includeTierNames, includeEmptyIntervals); cherror
	Table_list (table, false);
end:
	iferror { Melder_clearError (); Melder_information1 (L"Nothing to list."); }
	forget (table);
}

/* End of file TextGrid.c */
