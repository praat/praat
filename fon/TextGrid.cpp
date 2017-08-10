/* TextGrid.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2016,2017 Paul Boersma
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

autoTextPoint TextPoint_create (double time, const char32 *mark) {
	try {
		autoTextPoint me = Thing_new (TextPoint);
		my number = time;
		my mark = Melder_dup (mark);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Text point not created.");
	}
}

void TextPoint_setText (TextPoint me, const char32 *text) {
	try {
		/*
		 * Be fast if the string pointers are equal.
		 */
		if (text == my mark) return;
		/*
		 * Create a temporary variable (i.e. a temporary pointer to the final string),
		 * in order that 'my text' does not change in case of error.
		 */
		autostring32 newText = Melder_dup (text);
		Melder_free (my mark);
		my mark = newText.transfer();
	} catch (MelderError) {
		Melder_throw (me, U": text not set.");
	}
}

Thing_implement (TextInterval, Function, 0);

autoTextInterval TextInterval_create (double tmin, double tmax, const char32 *text) {
	try {
		autoTextInterval me = Thing_new (TextInterval);
		my xmin = tmin;
		my xmax = tmax;
		my text = Melder_dup (text);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Text interval not created.");
	}
}

void TextInterval_setText (TextInterval me, const char32 *text) {
	try {
		/*
		 * Be fast if the string pointers are equal.
		 */
		if (text == my text) return;
		/*
		 * Create without change.
		 */
		autostring32 newText = Melder_dup (text);
		/*
		 * Change without error.
		 */
		Melder_free (my text);
		my text = newText.transfer();
	} catch (MelderError) {
		Melder_throw (U"Text interval: text not set.");
	}
}

Thing_implement (TextTier, AnyTier, 0);

autoTextTier TextTier_create (double tmin, double tmax) {
	try {
		autoTextTier me = Thing_new (TextTier);
		my xmin = tmin;
		my xmax = tmax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Point tier not created.");
	}
}

void TextTier_addPoint (TextTier me, double time, const char32 *mark) {
	try {
		autoTextPoint point = TextPoint_create (time, mark);
		my points. addItem_move (point.move());
	} catch (MelderError) {
		Melder_throw (U"Point tier: point not added.");
	}
}

Thing_implement (IntervalTier, Function, 0);

void structIntervalTier :: v_shiftX (double xfrom, double xto) {
	IntervalTier_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= our intervals.size; i ++) {
		TextInterval interval = our intervals.at [i];
		interval -> v_shiftX (xfrom, xto);
	}
}

void structIntervalTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	IntervalTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= our intervals.size; i ++) {
		TextInterval interval = our intervals.at [i];
		interval -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

autoIntervalTier IntervalTier_create (double tmin, double tmax) {
	try {
		autoIntervalTier me = Thing_new (IntervalTier);
		my xmin = tmin;
		my xmax = tmax;
		autoTextInterval interval = TextInterval_create (tmin, tmax, nullptr);
		my intervals. addItem_move (interval.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interval tier not created.");
	}
}

long IntervalTier_timeToLowIndex (IntervalTier me, double t) {
	long ileft = 1, iright = my intervals.size;
	if (iright < 1) return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin) return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t >= rightInterval -> xmax) return 0;   // very large t
	while (ileft < iright) {
		long imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax) {
			ileft = imid + 1;
		} else {
			iright = imid;
		}
	}
	return ileft;
}

long IntervalTier_timeToIndex (IntervalTier me, double t) {
	long ileft = 1, iright = my intervals.size;
	if (iright < 1) return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin) return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t > rightInterval -> xmax) return 0;   // very large t
	while (ileft < iright) {
		long imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax) {
			ileft = imid + 1;
		} else {
			iright = imid;
		}
	}
	return ileft;
}

long IntervalTier_timeToHighIndex (IntervalTier me, double t) {
	long ileft = 1, iright = my intervals.size;
	if (iright < 1) return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t <= leftInterval -> xmin) return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t > rightInterval -> xmax) return 0;   // very large t
	while (ileft < iright) {
		long imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t > midInterval -> xmax) {
			ileft = imid + 1;
		} else {
			iright = imid;
		}
	}
	return ileft;
}

long IntervalTier_hasTime (IntervalTier me, double t) {
	long ileft = 1, iright = my intervals.size;
	if (iright < 1) return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin) return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t > rightInterval -> xmax) return 0;   // very large t
	while (ileft < iright) {
		long imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax) {
			ileft = imid + 1;
		} else {
			iright = imid;
		}
	}
	/*
	 * We now know that t is within interval ileft.
	 */
	leftInterval = my intervals.at [ileft];
	if (t == leftInterval -> xmin || t == leftInterval -> xmax) return ileft;
	return 0;   // not found
}

long IntervalTier_hasBoundary (IntervalTier me, double t) {
	long ileft = 2, iright = my intervals.size;
	if (iright < 2) return 0;   // tier without inner boundaries
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin) return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t >= rightInterval -> xmax) return 0;   // very large t
	while (ileft < iright) {
		long imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax) {
			ileft = imid + 1;
		} else {
			iright = imid;
		}
	}
	leftInterval = my intervals.at [ileft];
	if (t == leftInterval -> xmin) return ileft;
	return 0;   // not found
}

void structTextGrid :: v_info () {
	structDaata :: v_info ();

	long intervalTierCount = 0, pointTierCount = 0, intervalCount = 0, pointCount = 0;
	for (long itier = 1; itier <= our tiers->size; itier ++) {
		Function anyTier = our tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier intervalTier = static_cast <IntervalTier> (anyTier);
			intervalTierCount += 1;
			intervalCount += intervalTier -> intervals.size;
		} else {
			TextTier textTier = static_cast <TextTier> (anyTier);
			pointTierCount += 1;
			pointCount += textTier -> points.size;
		}
	}
	MelderInfo_writeLine (U"Number of interval tiers: ", intervalTierCount);
	MelderInfo_writeLine (U"Number of point tiers: ", pointTierCount);
	MelderInfo_writeLine (U"Number of intervals: ", intervalCount);
	MelderInfo_writeLine (U"Number of points: ", pointCount);
}

static void IntervalTier_addInterval_unsafe (IntervalTier me, double tmin, double tmax, const char32 *label) {
	autoTextInterval interval = TextInterval_create (tmin, tmax, label);
	my intervals.addItem_move (interval.move());
}

void structTextGrid :: v_repair () {
	for (long itier = 1; itier <= our tiers->size; itier ++) {
		Function anyTier = our tiers->at [itier];   // it's a triple indirection: * ((* (* us). tiers). at + itier)
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			if (tier -> intervals.size == 0) {
				IntervalTier_addInterval_unsafe (tier, tier -> xmin, tier -> xmax, U"");
			}
		}
	}
}

void structTextGrid :: v_shiftX (double xfrom, double xto) {
	TextGrid_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= our tiers->size; i ++) {
		Function tier = our tiers->at [i];
		tier -> v_shiftX (xfrom, xto);
	}
}

void structTextGrid :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	TextGrid_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= our tiers->size; i ++) {
		Function tier = our tiers->at [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

Thing_implement (FunctionList, Ordered, 0);

Thing_implement (TextGrid, Function, 0);

autoTextGrid TextGrid_createWithoutTiers (double tmin, double tmax) {
	try {
		autoTextGrid me = Thing_new (TextGrid);
		my tiers = FunctionList_create ();
		my xmin = tmin;
		my xmax = tmax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created.");
	}
}

autoTextGrid TextGrid_create (double tmin, double tmax, const char32 *tierNames, const char32 *pointTiers) {
	try {
		autoTextGrid me = TextGrid_createWithoutTiers (tmin, tmax);
		char32 nameBuffer [400];

		/*
		 * Create a number of IntervalTier objects.
		 */
		if (tierNames && tierNames [0]) {
			str32cpy (nameBuffer, tierNames);
			for (char32 *tierName = Melder_tok (nameBuffer, U" "); tierName; tierName = Melder_tok (nullptr, U" ")) {
				autoIntervalTier tier = IntervalTier_create (tmin, tmax);
				Thing_setName (tier.get(), tierName);
				my tiers -> addItem_move (tier.move());
			}
		}

		/*
		 * Replace some IntervalTier objects with TextTier objects.
		 */
		if (pointTiers && pointTiers [0]) {
			str32cpy (nameBuffer, pointTiers);
			for (char32 *tierName = Melder_tok (nameBuffer, U" "); tierName; tierName = Melder_tok (nullptr, U" ")) {
				for (long itier = 1; itier <= my tiers->size; itier ++) {
					if (str32equ (tierName, Thing_getName (my tiers->at [itier]))) {
						autoTextTier tier = TextTier_create (tmin, tmax);
						Thing_setName (tier.get(), tierName);
						my tiers -> replaceItem_move (tier.move(), itier);
					}
				}
			}
		}
		if (my tiers->size == 0)
			Melder_throw (U"Cannot create a TextGrid without tiers. Supply at least one tier name.");
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created.");
	}
}

autoTextTier TextTier_readFromXwaves (MelderFile file) {
	try {
		char *line;

		autoTextTier me = TextTier_create (0, 100);
		autoMelderFile mfile = MelderFile_open (file);

		/*
		 * Search for a line that starts with '#'.
		 */
		for (;;) {
			line = MelderFile_readLine (file);
			if (! line)
				Melder_throw (U"Missing '#' line.");
			if (line [0] == '#') break;
		}

		/*
		 * Read a mark from every line.
		 */
		for (;;) {
			line = MelderFile_readLine (file);
			if (! line) break;   // normal end-of-file
			double time;
			long colour;
			char mark [300];
			if (sscanf (line, "%lf%ld%299s", & time, & colour, mark) < 3)   // BUG: buffer overflow
				Melder_throw (U"Line too short: \"", Melder_peek8to32 (line), U"\".");
			TextTier_addPoint (me.get(), time, Melder_peek8to32 (mark));
		}

		/*
		 * Fix domain.
		 */
		if (my points.size > 0) {
			TextPoint point = my points.at [1];
			if (point -> number < 0.0) my xmin = point -> number - 1.0;
			point = my points.at [my points.size];
			my xmax = point -> number + 1.0;
		}
		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextTier not read from Xwaves file.");
	}
}

Function TextGrid_checkSpecifiedTierNumberWithinRange (TextGrid me, long tierNumber) {
	if (tierNumber < 1)
		Melder_throw (me, U": the specified tier number is ", tierNumber, U", but should be at least 1.");
	if (tierNumber > my tiers->size)
		Melder_throw (me, U": the specified tier number (", tierNumber, U") exceeds my number of tiers (", my tiers->size, U").");
	return my tiers->at [tierNumber];
}

IntervalTier TextGrid_checkSpecifiedTierIsIntervalTier (TextGrid me, long tierNumber) {
	Function tier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Tier ", tierNumber, U" is not an interval tier.");
	return static_cast <IntervalTier> (tier);
}

TextTier TextGrid_checkSpecifiedTierIsPointTier (TextGrid me, long tierNumber) {
	Function tier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
	if (tier -> classInfo != classTextTier)
		Melder_throw (U"Tier ", tierNumber, U" is not a point tier.");
	return static_cast <TextTier> (tier);
}

long TextGrid_countLabels (TextGrid me, long tierNumber, const char32 *text) {
	try {
		Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		long count = 0;
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (long i = 1; i <= tier -> intervals.size; i ++) {
				TextInterval segment = tier -> intervals.at [i];
				if (segment -> text && str32equ (segment -> text, text))
					count ++;
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (long i = 1; i <= tier -> points.size; i ++) {
				TextPoint point = tier -> points.at [i];
				if (point -> mark && str32equ (point -> mark, text))
					count ++;
			}
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, U": labels not counted.");
	}
}

long TextGrid_countIntervalsWhere (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		long count = 0;
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		for (long iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				count ++;
			}
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, U": intervals not counted.");
	}
}

long TextGrid_countPointsWhere (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		long count = 0;
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		for (long ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark, which_Melder_STRING, criterion)) {
				count ++;
			}
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, U": points not counted.");
	}
}

void TextGrid_addTier_copy (TextGrid me, Function anyTier) {
	try {
		autoFunction tier = Data_copy (anyTier);
		if (tier -> xmin < my xmin) my xmin = tier -> xmin;
		if (tier -> xmax > my xmax) my xmax = tier -> xmax;
		my tiers -> addItem_move (tier.move());
	} catch (MelderError) {
		Melder_throw (me, U": tier not added.");
	}
}

autoTextGrid TextGrids_merge (OrderedOf<structTextGrid>* textGrids) {
	try {
		if (textGrids->size < 1)
			Melder_throw (U"Cannot merge zero TextGrid objects.");
		autoTextGrid thee = Data_copy (textGrids->at [1]);
		for (long igrid = 2; igrid <= textGrids->size; igrid ++) {
			TextGrid textGrid = textGrids->at [igrid];
			for (long itier = 1; itier <= textGrid -> tiers->size; itier ++) {
				TextGrid_addTier_copy (thee.get(), textGrid -> tiers->at [itier]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrids not merged.");
	}
}

autoTextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, int preserveTimes) {
	try {
		autoTextGrid thee = Data_copy (me);
		if (tmax <= tmin) return thee;

		for (long itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = thy tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				for (long iinterval = tier -> intervals.size; iinterval >= 1; iinterval --) {
					TextInterval interval = tier -> intervals.at [iinterval];
					if (interval -> xmin >= tmax || interval -> xmax <= tmin) {
						tier -> intervals.removeItem (iinterval);
					} else {
						if (interval -> xmin < tmin) interval -> xmin = tmin;
						if (interval -> xmax > tmax) interval -> xmax = tmax;
					}
				}
			} else {
				TextTier textTier = static_cast <TextTier> (anyTier);
				for (long ipoint = textTier -> points.size; ipoint >= 1; ipoint --) {
					TextPoint point = textTier -> points.at [ipoint];
					if (point -> number < tmin || point -> number > tmax) {
						textTier -> points. removeItem (ipoint);
					}
				}
			}
			anyTier -> xmin = tmin;
			anyTier -> xmax = tmax;
		}
		thy xmin = tmin;
		thy xmax = tmax;
		if (! preserveTimes) Function_shiftXTo (thee.get(), thy xmin, 0.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

static autoTextGrid _Label_to_TextGrid (Label me, double tmin, double tmax) {
	autoTextGrid thee = TextGrid_createWithoutTiers (tmin, tmax);
	for (long itier = 1; itier <= my size; itier ++) {
		Tier tier = my at [itier];
		autoIntervalTier intervalTier = IntervalTier_create (tmin, tmax);
		thy tiers -> addItem_move (intervalTier.move());
		intervalTier -> intervals.removeItem (1);
		for (long iinterval = 1; iinterval <= tier->size; iinterval ++) {
			Autosegment autosegment = tier->at [iinterval];
			autoTextInterval textInterval = TextInterval_create (
				iinterval == 1 ? tmin : autosegment -> xmin,
				iinterval == tier->size ? tmax : autosegment -> xmax,
				autosegment -> name);
			intervalTier -> intervals. addItem_move (textInterval.move());
		}
	}
	return thee;
}

autoTextGrid Label_to_TextGrid (Label me, double duration) {
	try {
		double tmin = 0.0, tmax = duration;
		if (duration == 0.0) Label_suggestDomain (me, & tmin, & tmax);
		trace (duration, U" ", tmin, U" ", tmax);
		return _Label_to_TextGrid (me, tmin, tmax);
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TextGrid.");
	}
}

autoTextGrid Label_Function_to_TextGrid (Label me, Function function) {
	try {
		return _Label_to_TextGrid (me, function -> xmin, function -> xmax);
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TextGrid.");
	}
}

autoTextTier PointProcess_upto_TextTier (PointProcess me, const char32 *text) {
	try {
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		for (long i = 1; i <= my nt; i ++) {
			TextTier_addPoint (thee.get(), my t [i], text);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TextTier.");
	}
}

autoPointProcess TextTier_getPoints (TextTier me, const char32 *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my points.size; i ++) {
			TextPoint point = my points.at [i];
			if (text && text [0]) {
				if (point -> mark && str32equ (text, point -> mark))
					PointProcess_addPoint (thee.get(), point -> number);
			} else {
				if (! point -> mark || ! point -> mark [0])
					PointProcess_addPoint (thee.get(), point -> number);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": points not converted to PointProcess.");
	}
}

autoPointProcess IntervalTier_getStartingPoints (IntervalTier me, const char32 *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (text && text [0]) {
				if (interval -> text && str32equ (text, interval -> text))
					PointProcess_addPoint (thee.get(), interval -> xmin);
			} else {
				if (! interval -> text || ! interval -> text [0])
					PointProcess_addPoint (thee.get(), interval -> xmin);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": starting points not gotten.");
	}
}

autoPointProcess IntervalTier_getEndPoints (IntervalTier me, const char32 *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (text && text [0]) {
				if (interval -> text && str32equ (text, interval -> text))
					PointProcess_addPoint (thee.get(), interval -> xmax);
			} else {
				if (! interval -> text || ! interval -> text [0])
					PointProcess_addPoint (thee.get(), interval -> xmax);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": end points not gotten.");
	}
}

autoPointProcess IntervalTier_getCentrePoints (IntervalTier me, const char32 *text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (text && text [0]) {
				if (interval -> text && str32equ (text, interval -> text))
					PointProcess_addPoint (thee.get(), 0.5 * (interval -> xmin + interval -> xmax));
			} else {
				if (! interval -> text || ! interval -> text [0])
					PointProcess_addPoint (thee.get(), 0.5 * (interval -> xmin + interval -> xmax));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": centre points not gotten.");
	}
}

autoPointProcess TextGrid_getStartingPoints (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.get(), interval -> xmin);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": starting points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getEndPoints (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.get(), interval -> xmax);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": end points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getCentrePoints (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.get(), 0.5 * (interval -> xmin + interval -> xmax));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": centre points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getPoints (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark, which_Melder_STRING, criterion)) {
				PointProcess_addPoint (thee.get(), point -> number);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getPoints_preceded (TextGrid me, long tierNumber,
	int which_Melder_STRING, const char32 *criterion,
	int which_Melder_STRING_precededBy, const char32 *criterion_precededBy)
{
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark, which_Melder_STRING, criterion)) {
				TextPoint preceding = ( ipoint <= 1 ? nullptr : tier -> points.at [ipoint - 1] );
				if (Melder_stringMatchesCriterion (preceding -> mark, which_Melder_STRING_precededBy, criterion_precededBy)) {
					PointProcess_addPoint (thee.get(), point -> number);
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getPoints_followed (TextGrid me, long tierNumber,
	int which_Melder_STRING, const char32 *criterion,
	int which_Melder_STRING_followedBy, const char32 *criterion_followedBy)
{
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (long ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark, which_Melder_STRING, criterion)) {
				TextPoint following = ( ipoint >= tier -> points.size ? nullptr : tier -> points.at [ipoint + 1] );
				if (Melder_stringMatchesCriterion (following -> mark, which_Melder_STRING_followedBy, criterion_followedBy)) {
					PointProcess_addPoint (thee.get(), point -> number);
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": points not converted to PointProcess.");
	}
}

autoPointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase) {
	try {
		autoPointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
		for (long i = 1; i <= point -> nt; i ++) {
			double t = point -> t [i];
			long index = IntervalTier_timeToLowIndex (tier, t);
			if (index) {
				TextInterval interval = tier -> intervals.at [index];
				if (interval -> xmin == t)
					PointProcess_addPoint (thee.get(), (1 - phase) * interval -> xmin + phase * interval -> xmax);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (tier, U" & ", point, U": starts of intervals not converted to PointProcess.");
	}
}

autoPointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase) {
	try {
		autoPointProcess thee = PointProcess_create (tier -> xmin, tier -> xmax, 10);
		for (long i = 1; i <= point -> nt; i ++) {
			double t = point -> t [i];
			long index = IntervalTier_timeToHighIndex (tier, t);
			if (index) {
				TextInterval interval = tier -> intervals.at [index];
				if (interval -> xmax == t)
					PointProcess_addPoint (thee.get(), (1 - phase) * interval -> xmin + phase * interval -> xmax);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (tier, U" & ", point, U": ends of intervals not converted to PointProcess.");
	}
}

autoTableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, const char32 *label) {
	try {
		long n = 0;
		for (long i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (! label || (label [0] == U'\0' && ! interval -> text) || (interval -> text && str32equ (interval -> text, label)))
				n ++;
		}
		autoTableOfReal thee = TableOfReal_create (n, 3);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Start");
		TableOfReal_setColumnLabel (thee.get(), 2, U"End");
		TableOfReal_setColumnLabel (thee.get(), 3, U"Duration");
		n = 0;
		for (long i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (! label || (label [0] == U'\0' && ! interval -> text) || (interval -> text && str32equ (interval -> text, label))) {
				n ++;
				TableOfReal_setRowLabel (thee.get(), n, interval -> text ? interval -> text : U"");
				thy data [n] [1] = interval -> xmin;
				thy data [n] [2] = interval -> xmax;
				thy data [n] [3] = interval -> xmax - interval -> xmin;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

autoTableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me) {
	return IntervalTier_downto_TableOfReal (me, nullptr);
}

autoTableOfReal TextTier_downto_TableOfReal (TextTier me, const char32 *label) {
	try {
		long n = 0;
		for (long i = 1; i <= my points.size; i ++) {
			TextPoint point = my points.at [i];
			if (! label || (label [0] == U'\0' && ! point -> mark) || (point -> mark && str32equ (point -> mark, label)))
				n ++;
		}
		autoTableOfReal thee = TableOfReal_create (n, 1);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Time");
		n = 0;
		for (long i = 1; i <= my points.size; i ++) {
			TextPoint point = my points.at [i];
			if (! label || (label [0] == U'\0' && ! point -> mark) || (point -> mark && str32equ (point -> mark, label))) {
				n ++;
				TableOfReal_setRowLabel (thee.get(), n, point -> mark ? point -> mark : U"");
				thy data [n] [1] = point -> number;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

autoTableOfReal TextTier_downto_TableOfReal_any (TextTier me) {
	return TextTier_downto_TableOfReal (me, nullptr);
}

autoIntervalTier IntervalTier_readFromXwaves (MelderFile file) {
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
			if (! line)
				Melder_throw (U"Missing '#' line.");
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
			if (! line) break;   // normal end-of-file
			numberOfElements = sscanf (line, "%lf%ld%199s", & time, & colour, mark);
			if (numberOfElements == 0) {
				break;   // an empty line, hopefully at the end
			}
			if (numberOfElements == 1)
				Melder_throw (U"Line too short: \"", Melder_peek8to32 (line), U"\".");
			if (numberOfElements == 2)
				mark [0] = '\0';
			if (lastTime == 0.0) {
				TextInterval interval = my intervals.at [1];
				interval -> xmax = time;
				TextInterval_setText (interval, Melder_peek8to32 (mark));
			} else {
				IntervalTier_addInterval_unsafe (me.get(), lastTime, time, Melder_peek8to32 (mark));
			}
			lastTime = time;
		}

		/*
		 * Fix domain.
		 */
		if (lastTime > 0.0) {
			TextInterval lastInterval = my intervals.at [my intervals.size];
			my xmax = lastInterval -> xmax = lastTime;
		}

		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"IntervalTier not read from file ", file, U".");
	}
}

void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		fprintf (f, "separator ;\nnfields 1\n#\n");
		for (long iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
			TextInterval interval = my intervals.at [iinterval];
			fprintf (f, "\t%.6f 26\t%s\n", interval -> xmax, Melder_peek32to8 (interval -> text));
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": not written to Xwaves file ", file, U".");
	}
}

autoTextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT) {
	try {
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, U"vuv", nullptr);
		IntervalTier tier = static_cast <IntervalTier> (thy tiers->at [1]);
		tier -> intervals. removeItem (1);
		long ipointright;
		double beginVoiceless = my xmin, endVoiceless, halfMeanT = 0.5 * meanT;
		for (long ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
			endVoiceless = my t [ipointleft] - halfMeanT;
			if (endVoiceless <= beginVoiceless) {
				endVoiceless = beginVoiceless;   // we will use for voiced interval
			} else {
				IntervalTier_addInterval_unsafe (tier, beginVoiceless, endVoiceless, U"U");
			}
			for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
				if (my t [ipointright] - my t [ipointright - 1] > maxT)
					break;
			ipointright --;
			beginVoiceless = my t [ipointright] + halfMeanT;
			if (beginVoiceless > my xmax)
				beginVoiceless = my xmax;
			IntervalTier_addInterval_unsafe (tier, endVoiceless, beginVoiceless, U"V");
		}
		endVoiceless = my xmax;
		if (endVoiceless > beginVoiceless) {
			IntervalTier_addInterval_unsafe (tier, beginVoiceless, endVoiceless, U"U");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TextGrid (vuv).");
	}
}

long TextInterval_labelLength (TextInterval me) {
	return my text ? str32len (my text) : 0;
}

long TextPoint_labelLength (TextPoint me) {
	return my mark ? str32len (my mark) : 0;
}

long IntervalTier_maximumLabelLength (IntervalTier me) {
	long maximum = 0;
	for (long iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
		long length = TextInterval_labelLength (my intervals.at [iinterval]);
		if (length > maximum) {
			maximum = length;
		}
	}
	return maximum;
}

long TextTier_maximumLabelLength (TextTier me) {
	long maximum = 0;
	for (long ipoint = 1; ipoint <= my points.size; ipoint ++) {
		long length = TextPoint_labelLength (my points.at [ipoint]);
		if (length > maximum) {
			maximum = length;
		}
	}
	return maximum;
}

long TextGrid_maximumLabelLength (TextGrid me) {
	long maximum = 0;
	for (long itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		long length = anyTier -> classInfo == classIntervalTier ?
			IntervalTier_maximumLabelLength ((IntervalTier) anyTier) :
			TextTier_maximumLabelLength ((TextTier) anyTier);
		if (length > maximum) {
			maximum = length;
		}
	}
	return maximum;
}

static void genericize (char32 **pstring, char32 *buffer) {
	if (*pstring) {
		const char32 *p = (const char32 *) *pstring;
		while (*p) {
			if (*p > 126) {   // only if necessary
				char32 *newString;
				Longchar_genericize32 (*pstring, buffer);
				newString = Melder_dup (buffer);
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

void TextGrid_convertToBackslashTrigraphs (TextGrid me) {
	try {
		autostring32 buffer = Melder_calloc (char32, TextGrid_maximumLabelLength (me) * 3 + 1);
		for (long itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				for (long i = 1; i <= tier -> intervals.size; i ++) {
					TextInterval interval = tier -> intervals.at [i];
					genericize (& interval -> text, buffer.peek());
				}
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				for (long i = 1; i <= tier -> points.size; i ++) {
					TextPoint point = tier -> points.at [i];
					genericize (& point -> mark, buffer.peek());
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not converted to backslash trigraphs.");
	}
}

void TextGrid_convertToUnicode (TextGrid me) {
	try {
		autostring32 buffer = Melder_calloc (char32, TextGrid_maximumLabelLength (me) + 1);
		for (long itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				for (long i = 1; i <= tier -> intervals.size; i ++) {
					TextInterval interval = tier -> intervals.at [i];
					if (interval -> text) {
						Longchar_nativize32 (interval -> text, buffer.peek(), false);
						str32cpy (interval -> text, buffer.peek());
					}
				}
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				for (long i = 1; i <= tier -> points.size; i ++) {
					TextPoint point = tier -> points.at [i];
					if (point -> mark) {
						Longchar_nativize32 (point -> mark, buffer.peek(), false);
						str32cpy (point -> mark, buffer.peek());
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": backslash trigraphs not converted to Unicode.");
	}
}

void TextInterval_removeText (TextInterval me) {
	Melder_free (my text);
}

void TextPoint_removeText (TextPoint me) {
	Melder_free (my mark);
}

void IntervalTier_removeText (IntervalTier me) {
	long ninterval = my intervals.size;
	for (long iinterval = 1; iinterval <= ninterval; iinterval ++)
		TextInterval_removeText (my intervals.at [iinterval]);
}

void TextTier_removeText (TextTier me) {
	long npoint = my points.size;
	for (long ipoint = 1; ipoint <= npoint; ipoint ++)
		TextPoint_removeText (my points.at [ipoint]);
}

void TextGrid_insertBoundary (TextGrid me, int tierNumber, double t) {
	try {
		Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		if (anyTier -> classInfo != classIntervalTier)
			Melder_throw (U"Cannot add a boundary on tier ", tierNumber, U", because that tier is not an interval tier.");
		IntervalTier intervalTier = static_cast <IntervalTier> (anyTier);
		if (IntervalTier_hasTime (intervalTier, t))
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because there is already a boundary there.");
		long intervalNumber = IntervalTier_timeToIndex (intervalTier, t);
		if (intervalNumber == 0)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = intervalTier -> intervals.at [intervalNumber];
		/*
		 * Move the text to the left of the boundary.
		 */
		autoTextInterval newInterval = TextInterval_create (t, interval -> xmax, U"");
		interval -> xmax = t;
		intervalTier -> intervals. addItem_move (newInterval.move());
	} catch (MelderError) {
		Melder_throw (me, U": boundary not inserted.");
	}
}

void IntervalTier_removeLeftBoundary (IntervalTier me, long intervalNumber) {
	try {
		Melder_assert (intervalNumber > 1);
		Melder_assert (intervalNumber <= my intervals.size);
		TextInterval left = my intervals.at [intervalNumber - 1];
		TextInterval right = my intervals.at [intervalNumber];
		/*
		 * Move the text to the left of the boundary.
		 */
		left -> xmax = right -> xmax;   // collapse left and right intervals into left interval
		if (! right -> text) {
			;
		} else if (! left -> text) {
			TextInterval_setText (left, right -> text);
		} else {
			TextInterval_setText (left, Melder_cat (left -> text, right -> text));
		}
		my intervals. removeItem (intervalNumber);   // remove right interval
	} catch (MelderError) {
		Melder_throw (me, U": left boundary not removed.");
	}
}

void TextGrid_removeBoundaryAtTime (TextGrid me, int tierNumber, double t) {
	try {
		IntervalTier intervalTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		if (! IntervalTier_hasTime (intervalTier, t))
			Melder_throw (U"There is no boundary at ", t, U" seconds.");
		long intervalNumber = IntervalTier_timeToIndex (intervalTier, t);
		if (intervalNumber == 0)
			Melder_throw (U"The time of ", t, U" seconds is outside the time domain of the intervals.");
		if (intervalNumber == 1)
			Melder_throw (U"The time of ", t, U" seconds is at the left edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, intervalNumber);
	} catch (MelderError) {
		Melder_throw (me, U": boundary not removed.");
	}
}

void TextGrid_setIntervalText (TextGrid me, int tierNumber, long intervalNumber, const char32 *text) {
	try {
		IntervalTier intervalTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		if (intervalNumber < 1 || intervalNumber > intervalTier -> intervals.size)
			Melder_throw (U"Interval ", intervalNumber, U" does not exist on tier ", tierNumber, U".");
		TextInterval interval = intervalTier -> intervals.at [intervalNumber];
		TextInterval_setText (interval, text);
	} catch (MelderError) {
		Melder_throw (me, U": interval text not set.");
	}
}

void TextGrid_insertPoint (TextGrid me, int tierNumber, double t, const char32 *mark) {
	try {
		TextTier textTier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		if (AnyTier_hasPoint (textTier->asAnyTier(), t))
			Melder_throw (U"There is already a point at ", t, U" seconds.");
		autoTextPoint newPoint = TextPoint_create (t, mark);
		textTier -> points. addItem_move (newPoint.move());
	} catch (MelderError) {
		Melder_throw (me, U": point not inserted.");
	}
}

void TextTier_removePoint (TextTier me, long ipoint) {
	Melder_assert (ipoint <= my points.size);
	my points. removeItem (ipoint);
}

void TextTier_removePoints (TextTier me, int which_Melder_STRING, const char32 *criterion) {
	for (long i = my points.size; i > 0; i --)
		if (Melder_stringMatchesCriterion (my points.at [i] -> mark, which_Melder_STRING, criterion))
			my points. removeItem (i);
}

void TextGrid_removePoints (TextGrid me, long tierNumber, int which_Melder_STRING, const char32 *criterion) {
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		TextTier_removePoints (tier, which_Melder_STRING, criterion);
	} catch (MelderError) {
		Melder_throw (me, U": points not removed.");
	}
}

void TextGrid_setPointText (TextGrid me, int tierNumber, long pointNumber, const char32 *text) {
	try {
		TextTier textTier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		if (pointNumber < 1 || pointNumber > textTier -> points.size)
			Melder_throw (U"Point ", pointNumber, U" does not exist on tier ", tierNumber, U".");
		TextPoint point = textTier -> points.at [pointNumber];
		TextPoint_setText (point, text);
	} catch (MelderError) {
		Melder_throw (me, U": point text not set.");
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
				{ nullptr, nullptr } };
			char sgmlCode [201];
			int i = 0;
			++ sgml;
			for (i = 0; i < 200; i ++) {
				char sgmlChar = sgml [i];
				if (sgmlChar == ';') {
					if (i == 0) Melder_throw (U"Empty SGML code.");
					sgml += i + 1;
					break;
				}
				sgmlCode [i] = sgmlChar;
			}
			if (i >= 200) Melder_throw (U"Unfinished SGML code.");
			sgmlCode [i] = '\0';
			for (i = 0; translations [i]. sgml; i ++) {
				if (strequ (sgmlCode, translations [i]. sgml)) {
					memcpy (praat, translations [i]. praat, strlen (translations [i]. praat));
					praat += strlen (translations [i]. praat);
					break;
				}
			}
			if (! translations [i]. sgml) Melder_throw (U"Unknown SGML code &", Melder_peek8to32 (sgmlCode), U";.");
		} else {
			* praat ++ = * sgml ++;
		}
	}
	*praat = '\0';
}

autoTextGrid TextGrid_readFromChronologicalTextFile (MelderFile file) {
	try {
		int formatVersion = 0;
		autoMelderReadText text = MelderReadText_createFromFile (file);
		autostring32 tag = texgetw16 (text.peek());
		if (! str32equ (tag.peek(), U"Praat chronological TextGrid text file"))
			Melder_throw (U"This is not a chronological TextGrid text file.");
		autoTextGrid me = Thing_new (TextGrid);
		my structFunction :: v_readText (text.peek(), formatVersion);
		my tiers = FunctionList_create ();
		long numberOfTiers = texgeti32 (text.peek());
		for (long itier = 1; itier <= numberOfTiers; itier ++) {
			autostring32 klas = texgetw16 (text.peek());
			if (str32equ (klas.peek(), U"IntervalTier")) {
				autoIntervalTier tier = Thing_new (IntervalTier);
				tier -> name = texgetw16 (text.peek());
				tier -> structFunction :: v_readText (text.peek(), formatVersion);
				my tiers -> addItem_move (tier.move());
			} else if (str32equ (klas.peek(), U"TextTier")) {
				autoTextTier tier = Thing_new (TextTier);
				tier -> name = texgetw16 (text.peek());
				tier -> structFunction :: v_readText (text.peek(), formatVersion);
				my tiers -> addItem_move (tier.move());
			} else {
				Melder_throw (U"Unknown tier class \"", klas.peek(), U"\".");
			}
		}
		for (;;) {
			long tierNumber;
			try {
				tierNumber = texgeti32 (text.peek());
			} catch (MelderError) {
				if (str32str (Melder_getError (), U"Early end of text")) {
					Melder_clearError ();
					break;
				} else {
					throw;
				}
			}
			Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me.get(), tierNumber);
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				autoTextInterval interval = Thing_new (TextInterval);
				interval -> v_readText (text.peek(), formatVersion);
				tier -> intervals. addItem_move (interval.move());   // not earlier: sorting depends on contents of interval
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				autoTextPoint point = Thing_new (TextPoint);
				point -> v_readText (text.peek(), formatVersion);
				tier -> points. addItem_move (point.move());   // not earlier: sorting depends on contents of point
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from chronological text file ", file, U".");
	}
}

static void writeQuotedString (MelderFile file, const char32 *string) {
	MelderFile_writeCharacter (file, U'\"');
	if (string) {
		char32 kar;
		while ((kar = *string ++) != U'\0') {
			MelderFile_writeCharacter (file, kar);
			if (kar == '\"') MelderFile_writeCharacter (file, kar);
		}
	}   // BUG
	MelderFile_writeCharacter (file, U'\"');
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
		MelderFile_write (file, U"\"Praat chronological TextGrid text file\"\n", my xmin, U" ", my xmax,
			U"   ! Time domain.\n", my tiers->size, U"   ! Number of tiers.");
		for (long itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			MelderFile_write (file, U"\n");
			writeQuotedString (file, Thing_className (anyTier));
			MelderFile_write (file, U" ");
			writeQuotedString (file, anyTier -> name);
			MelderFile_write (file, U" ", anyTier -> xmin, U" ", anyTier -> xmax);
		}
		for (;;) {
			double firstRemainingTime = +1e308;
			long firstRemainingTier = 2000000000, firstRemainingElement = 0;
			for (long itier = 1; itier <= my tiers->size; itier ++) {
				Function anyTier = my tiers->at [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					for (long ielement = 1; ielement <= tier -> intervals.size; ielement ++) {
						TextInterval interval = tier -> intervals.at [ielement];
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
					TextTier tier = static_cast <TextTier> (anyTier);
					for (long ielement = 1; ielement <= tier -> points.size; ielement ++) {
						TextPoint point = tier -> points.at [ielement];
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
				Function anyTier = my tiers->at [firstRemainingTier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					TextInterval interval = tier -> intervals.at [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, U"\n\n! ", tier -> name, U":");
					MelderFile_write (file, U"\n", firstRemainingTier, U" ", interval -> xmin, U" ", interval -> xmax);
					texputw32 (file, interval -> text, U"", 0,0,0,0,0);
				} else {
					TextTier tier = static_cast <TextTier> (anyTier);
					TextPoint point = tier -> points.at [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, U"\n\n! ", tier -> name, U":");
					MelderFile_write (file, U"\n", firstRemainingTier, U" ", point -> number, U" ");
					texputw32 (file, point -> mark, U"", 0,0,0,0,0);
				}
				sortingTime = firstRemainingTime;
				sortingTier = firstRemainingTier;
			}
		}
		texexdent (file);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to chronological text file ", file, U".");
	}
}

autoTextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file) {
	try {
		autoTextGrid me = Thing_new (TextGrid);
		long sentenceNumber = 0;
		double phraseBegin = 0.0, phraseEnd = 0.0;
		IntervalTier sentenceTier = nullptr, phraseTier = nullptr;
		TextInterval lastInterval = nullptr;
		static char phrase [1000];
		my tiers = FunctionList_create ();
		autoMelderFile mfile = MelderFile_open (file);
		char *line = MelderFile_readLine (file);
		if (! strequ (line, "<?xml version=\"1.0\"?>"))
			Melder_throw (U"This is not a CGN syntax file.");
		line = MelderFile_readLine (file);
		if (! strequ (line, "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">"))
			Melder_throw (U"This is not a CGN syntax file.");
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
					Melder_throw (U"Too few strings in tau line.");
				my xmax = atof (arg5 + 4);
			}
		}
		if (my xmax <= 0.0) Melder_throw (U"Duration (", my xmax, U" seconds) should be greater than zero.");
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
					Melder_throw (U"Too few strings in tau line.");
				length = strlen (arg3);
				if (length < 5 || ! strnequ (arg3, "s=\"", 3))
					Melder_throw (U"Missing speaker name.");
				arg3 [length - 1] = '\0';   // truncate at double quote
				speakerName = arg3 + 3;   // truncate leading s="
				/*
				 * Does this speaker name occur in the tiers?
				 */
				for (long itier = 1; itier <= my tiers->size; itier ++) {
					IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
					if (str32equ (tier -> name, Melder_peek8to32 (speakerName))) {
						speakerTier = itier;
						break;
					}
				}
				if (speakerTier == 0) {
					/*
					 * Create two new tiers.
					 */
					autoIntervalTier newSentenceTier = Thing_new (IntervalTier);
					newSentenceTier -> xmin = 0.0;
					newSentenceTier -> xmax = my xmax;
					Thing_setName (newSentenceTier.get(), Melder_peek8to32 (speakerName));
					sentenceTier = (IntervalTier) my tiers -> addItem_move (newSentenceTier.move());
					autoIntervalTier newPhraseTier = Thing_new (IntervalTier);
					newPhraseTier -> xmin = 0.0;
					newPhraseTier -> xmax = my xmax;
					Thing_setName (newPhraseTier.get(), Melder_peek8to32 (speakerName));
					phraseTier = (IntervalTier) my tiers -> addItem_move (newPhraseTier.move());
				} else {
					sentenceTier = (IntervalTier) my tiers->at [speakerTier];
					phraseTier = (IntervalTier) my tiers->at [speakerTier + 1];
				}
				tb = atof (arg4 + 4), te = atof (arg5 + 4);
				if (te <= tb)
					Melder_throw (U"Zero duration for sentence.");
				/*
				 * We are going to add one or two intervals to the sentence tier.
				 */
				if (sentenceTier -> intervals.size > 0) {
					TextInterval latestInterval = sentenceTier -> intervals.at [sentenceTier -> intervals.size];
					if (tb > latestInterval -> xmax) {
						autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, U"");
						sentenceTier -> intervals.addItem_move (interval.move());
					} else if (tb < latestInterval -> xmax) {
						Melder_throw (U"Overlap on tier not allowed.");
					}
				} else {
					if (tb > 0.0) {
						autoTextInterval interval = TextInterval_create (0.0, tb, U"");
						sentenceTier -> intervals.addItem_move (interval.move());
					} else if (tb < 0.0) {
						Melder_throw (U"Negative times not allowed.");
					}
				}
				autoTextInterval interval = TextInterval_create (tb, te, Melder_integer (++ sentenceNumber));
				sentenceTier -> intervals.addItem_move (interval.move());
			} else if (strnequ (line, "    <tw ref=\"", 13)) {
				int length;
				double tb, te;
				if (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) < 7)
					Melder_throw (U"Too few strings in tw line.");
				length = strlen (arg3);
				if (length < 6 || ! strnequ (arg3, "tb=\"", 4))
					Melder_throw (U"Missing tb.");
				tb = atof (arg3 + 4);
				length = strlen (arg4);
				if (length < 6 || ! strnequ (arg4, "te=\"", 4))
					Melder_throw (U"Missing te.");
				te = atof (arg4 + 4);
				if (te <= tb)
					Melder_throw (U"Zero duration for phrase.");
				if (tb == phraseBegin && te == phraseEnd) {
					/* Append a word. */
					strcat (phrase, " ");
					length = strlen (arg7);
					if (length < 6 || ! strnequ (arg7, "w=\"", 3))
						Melder_throw (U"Missing word.");
					arg7 [length - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
				} else {
					/* Begin a phrase. */
					if (lastInterval) {
						sgmlToPraat (phrase);
						TextInterval_setText (lastInterval, Melder_peek8to32 (phrase));
					}
					phrase [0] = '\0';
					length = strlen (arg7);
					if (length < 6 || ! strnequ (arg7, "w=\"", 3))
						Melder_throw (U"Missing word.");
					arg7 [length - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
					if (phraseTier -> intervals.size > 0) {
						TextInterval latestInterval = phraseTier -> intervals.at [phraseTier -> intervals.size];
						if (tb > latestInterval -> xmax) {
							autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, U"");
							phraseTier -> intervals. addItem_move (interval.move());
						} else if (tb < latestInterval -> xmax) {
							Melder_throw (U"Overlap on tier not allowed.");
						}
					} else {
						if (tb > 0.0) {
							autoTextInterval interval = TextInterval_create (0.0, tb, U"");
							phraseTier -> intervals. addItem_move (interval.move());
						} else if (tb < 0.0) {
							Melder_throw (U"Negative times not allowed.");
						}
					}
					if (! phraseTier)
						Melder_throw (U"Phrase outside sentence.");
					autoTextInterval newLastInterval = TextInterval_create (tb, te, U"");
					lastInterval = newLastInterval.get();
					phraseTier -> intervals.addItem_move (newLastInterval.move());
					phraseBegin = tb;
					phraseEnd = te;
				}
			}
		}
		if (lastInterval) {
			sgmlToPraat (phrase);
			TextInterval_setText (lastInterval, Melder_peek8to32 (phrase));
		}
		for (long itier = 1; itier <= my tiers->size; itier ++) {
			IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
			if (tier -> intervals.size > 0) {
				TextInterval latestInterval = tier -> intervals.at [tier -> intervals.size];
				if (my xmax > latestInterval -> xmax) {
					autoTextInterval interval = TextInterval_create (latestInterval -> xmax, my xmax, U"");
					tier -> intervals. addItem_move (interval.move());
				}
			} else {
				autoTextInterval interval = TextInterval_create (my xmin, my xmax, U"");
				tier -> intervals. addItem_move (interval.move());
			}
		}
		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from CGN syntax file ", file, U".");
	}
}

autoTable TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals) {
	long numberOfRows = 0;
	for (long itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			if (includeEmptyIntervals) {
				numberOfRows += tier -> intervals.size;
			} else {
				for (long iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
					TextInterval interval = tier -> intervals.at [iinterval];
					if (interval -> text && interval -> text [0] != U'\0') {
						numberOfRows ++;
					}
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			numberOfRows += tier -> points.size;
		}
	}
	autoTable thee = Table_createWithoutColumnNames (numberOfRows, 3 + includeLineNumbers + includeTierNames);
	long icol = 0;
	if (includeLineNumbers)
		Table_setColumnLabel (thee.get(), ++ icol, U"line");
	Table_setColumnLabel (thee.get(), ++ icol, U"tmin");
	if (includeTierNames)
		Table_setColumnLabel (thee.get(), ++ icol, U"tier");
	Table_setColumnLabel (thee.get(), ++ icol, U"text");
	Table_setColumnLabel (thee.get(), ++ icol, U"tmax");
	long irow = 0;
	for (long itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (long iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
				TextInterval interval = tier -> intervals.at [iinterval];
				if (includeEmptyIntervals || (interval -> text && interval -> text [0] != U'\0')) {
					++ irow;
					icol = 0;
					if (includeLineNumbers)
						Table_setNumericValue (thee.get(), irow, ++ icol, irow);
					Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (interval -> xmin, timeDecimals));
					if (includeTierNames)
						Table_setStringValue (thee.get(), irow, ++ icol, tier -> name);
					Table_setStringValue (thee.get(), irow, ++ icol, interval -> text);
					Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (interval -> xmax, timeDecimals));
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (long ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
				TextPoint point = tier -> points.at [ipoint];
				++ irow;
				icol = 0;
				if (includeLineNumbers)
					Table_setNumericValue (thee.get(), irow, ++ icol, irow);
				Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (point -> number, timeDecimals));
				if (includeTierNames)
					Table_setStringValue (thee.get(), irow, ++ icol, tier -> name);
				Table_setStringValue (thee.get(), irow, ++ icol, point -> mark);
				Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (point -> number, timeDecimals));
			}
		}
	}
	long columns [1+2] = { 0, 1 + includeLineNumbers, 3 + includeLineNumbers + includeTierNames };   // sort by tmin and tmax
	Table_sortRows_Assert (thee.get(), columns, 2);
	return thee;
}

void TextGrid_list (TextGrid me, bool includeLineNumbers, int timeDecimals, bool includeTierNames, bool includeEmptyIntervals) {
	try {
		autoTable table = TextGrid_downto_Table (me, includeLineNumbers, timeDecimals, includeTierNames, includeEmptyIntervals);
		Table_list (table.get(), false);
	} catch (MelderError) {
		Melder_throw (me, U": not listed.");
	}
}

void TextGrid_correctRoundingErrors (TextGrid me) {
	for (long itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			Melder_assert (tier -> intervals.size > 0);
			TextInterval first = tier -> intervals.at [1];
			first -> xmin = my xmin;
			Melder_assert (first -> xmin < first -> xmax);
			for (long iinterval = 1; iinterval < tier -> intervals.size; iinterval ++) {
				TextInterval left = tier -> intervals.at [iinterval];
				TextInterval right = tier -> intervals.at [iinterval + 1];
				right -> xmin = left -> xmax;
				trace (U"tier ", itier, U", interval ", iinterval, U", ", right -> xmin, U" ", right -> xmax);
				Melder_assert (right -> xmin < right -> xmax);
			}
			TextInterval last = tier -> intervals.at [tier -> intervals.size];
			trace (tier -> intervals.size, U" ", last -> xmax, U" ", my xmax);
			last -> xmax = my xmax;
			Melder_assert (last -> xmax > last -> xmin);
		}
		anyTier -> xmin = my xmin;
		anyTier -> xmax = my xmax;
	}
}

autoTextGrid TextGrids_concatenate (OrderedOf<structTextGrid>* me) {
	try {
		autoTextGrid thee = TextGrids_to_TextGrid_appendContinuous (me, false);
		TextGrid_correctRoundingErrors (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrids not concatenated.");
	}
}

/* End of file TextGrid.cpp */
