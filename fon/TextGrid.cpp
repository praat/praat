/* TextGrid.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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
#include "../kar/longchar.h"

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

autoTextPoint TextPoint_create (double time, conststring32 mark) {
	try {
		autoTextPoint me = Thing_new (TextPoint);
		my number = time;
		my mark = Melder_dup (mark);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Text point not created.");
	}
}

void TextPoint_setText (TextPoint me, conststring32 text) {
	try {
		my mark = Melder_dup (text);
	} catch (MelderError) {
		Melder_throw (me, U": text not set.");
	}
}

Thing_implement (TextInterval, Function, 0);

autoTextInterval TextInterval_create (double tmin, double tmax, conststring32 text) {
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

void TextInterval_setText (TextInterval me, conststring32 text) {
	try {
		my text = Melder_dup (text);
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

void TextTier_addPoint (TextTier me, double time, conststring32 mark) {
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
	for (integer i = 1; i <= our intervals.size; i ++) {
		TextInterval interval = our intervals.at [i];
		interval -> v_shiftX (xfrom, xto);
	}
}

void structIntervalTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	IntervalTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (integer i = 1; i <= our intervals.size; i ++) {
		TextInterval interval = our intervals.at [i];
		interval -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

autoIntervalTier IntervalTier_create_raw (double tmin, double tmax) {
	try {
		autoIntervalTier me = Thing_new (IntervalTier);
		my xmin = tmin;
		my xmax = tmax;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interval tier without intervals not created.");
	}
}

TextInterval /* reference */ IntervalTier_addInterval_raw (IntervalTier me, double tmin, double tmax, conststring32 text) {
	try {
		autoTextInterval interval = TextInterval_create (tmin, tmax, text);
		Melder_clipRight (& my xmin, tmin);
		Melder_clipLeft (tmax, & my xmax);
		return my intervals. addItem_move (interval.move());
	} catch (MelderError) {
		Melder_throw (U"Interval could not be added to tier.");
	}
}

void IntervalTier_haveAtLeastOneInterval (IntervalTier me) {
	try {
		if (my intervals.size == 0)
			IntervalTier_addInterval_raw (me, my xmin, my xmax, U"");
	} catch (MelderError) {
		Melder_throw (U"Text interval not added to tier.");
	}
}

autoIntervalTier IntervalTier_create (double tmin, double tmax) {
	try {
		autoIntervalTier me = IntervalTier_create_raw (tmin, tmax);
		IntervalTier_haveAtLeastOneInterval (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interval tier not created.");
	}
}

integer IntervalTier_timeToLowIndex (IntervalTier me, double t) {
	integer ileft = 1, iright = my intervals.size;
	if (iright < 1)
		return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin)
		return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t >= rightInterval -> xmax)
		return 0;   // very large t
	while (ileft < iright) {
		integer imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax)
			ileft = imid + 1;
		else
			iright = imid;
	}
	return ileft;
}

integer IntervalTier_timeToIndex (IntervalTier me, double t) {
	integer ileft = 1, iright = my intervals.size;
	if (iright < 1)
		return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin)
		return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t > rightInterval -> xmax)
		return 0;   // very large t
	while (ileft < iright) {
		integer imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax)
			ileft = imid + 1;
		else
			iright = imid;
	}
	return ileft;
}

integer IntervalTier_timeToHighIndex (IntervalTier me, double t) {
	integer ileft = 1, iright = my intervals.size;
	if (iright < 1)
		return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t <= leftInterval -> xmin)
		return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t > rightInterval -> xmax)
		return 0;   // very large t
	while (ileft < iright) {
		integer imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t > midInterval -> xmax)
			ileft = imid + 1;
		else
			iright = imid;
	}
	return ileft;
}

integer IntervalTier_hasTime (IntervalTier me, double t) {
	integer ileft = 1, iright = my intervals.size;
	if (iright < 1)
		return 0;   // empty tier
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin)
		return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t > rightInterval -> xmax)
		return 0;   // very large t
	while (ileft < iright) {
		integer imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax)
			ileft = imid + 1;
		else
			iright = imid;
	}
	/*
		We now know that t is within interval ileft.
	*/
	leftInterval = my intervals.at [ileft];
	if (t == leftInterval -> xmin || t == leftInterval -> xmax) return ileft;
	return 0;   // not found
}

integer IntervalTier_hasBoundary (IntervalTier me, double t) {
	integer ileft = 2, iright = my intervals.size;
	if (iright < 2)
		return 0;   // tier without inner boundaries
	TextInterval leftInterval = my intervals.at [ileft];
	if (t < leftInterval -> xmin)
		return 0;   // very small t
	TextInterval rightInterval = my intervals.at [iright];
	if (t >= rightInterval -> xmax)
		return 0;   // very large t
	while (ileft < iright) {
		integer imid = (ileft + iright) / 2;
		TextInterval midInterval = my intervals.at [imid];
		if (t >= midInterval -> xmax)
			ileft = imid + 1;
		else
			iright = imid;
	}
	leftInterval = my intervals.at [ileft];
	if (t == leftInterval -> xmin)
		return ileft;
	return 0;   // not found
}

void structTextGrid :: v1_info () {
	structDaata :: v1_info ();

	integer intervalTierCount = 0, pointTierCount = 0, intervalCount = 0, pointCount = 0;
	for (integer itier = 1; itier <= our tiers->size; itier ++) {
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

void structTextGrid :: v_repair () {
	for (integer itier = 1; itier <= our tiers->size; itier ++) {
		Function anyTier = our tiers->at [itier];   // it's a triple indirection: * ((* (* us). tiers). at + itier)
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			if (tier -> intervals.size == 0)
				IntervalTier_addInterval_raw (tier, tier -> xmin, tier -> xmax, U"");
		}
	}
}

void structTextGrid :: v_shiftX (double xfrom, double xto) {
	TextGrid_Parent :: v_shiftX (xfrom, xto);
	for (integer i = 1; i <= our tiers->size; i ++) {
		Function tier = our tiers->at [i];
		tier -> v_shiftX (xfrom, xto);
	}
}

void structTextGrid :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	TextGrid_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (integer i = 1; i <= our tiers->size; i ++) {
		Function tier = our tiers->at [i];
		tier -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

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

autoTextGrid TextGrid_create (double tmin, double tmax, conststring32 tierNames_string, conststring32 pointTiers_string) {
	try {
		autoSTRVEC tierNames = splitByWhitespace_STRVEC (tierNames_string);
		autoSTRVEC pointTiers = splitByWhitespace_STRVEC (pointTiers_string);
		autoTextGrid me = TextGrid_createWithoutTiers (tmin, tmax);

		/*
			Create a number of IntervalTier objects.
		*/
		for (integer itoken = 1; itoken <= tierNames.size; itoken ++) {
			autoIntervalTier tier = IntervalTier_create (tmin, tmax);
			Thing_setName (tier.get(), tierNames [itoken].get());
			my tiers -> addItem_move (tier.move());
		}

		/*
			Replace some IntervalTier objects with TextTier objects.
		*/
		for (integer itoken = 1; itoken <= pointTiers.size; itoken ++) {
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				if (str32equ (pointTiers [itoken].get(), Thing_getName (my tiers->at [itier]))) {
					autoTextTier tier = TextTier_create (tmin, tmax);
					Thing_setName (tier.get(), pointTiers [itoken].get());
					my tiers -> replaceItem_move (tier.move(), itier);
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

Function TextGrid_checkSpecifiedTierNumberWithinRange (TextGrid me, integer tierNumber) {
	if (tierNumber < 1)
		Melder_throw (me, U": the specified tier number is ", tierNumber, U", but should be at least 1.");
	if (tierNumber > my tiers->size)
		Melder_throw (me, U": the specified tier number (", tierNumber, U") exceeds my number of tiers (", my tiers->size, U").");
	return my tiers->at [tierNumber];
}

IntervalTier TextGrid_checkSpecifiedTierIsIntervalTier (TextGrid me, integer tierNumber) {
	Function tier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Tier ", tierNumber, U" is not an interval tier.");
	return static_cast <IntervalTier> (tier);
}

TextTier TextGrid_checkSpecifiedTierIsPointTier (TextGrid me, integer tierNumber) {
	Function tier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
	if (tier -> classInfo != classTextTier)
		Melder_throw (U"Tier ", tierNumber, U" is not a point tier.");
	return static_cast <TextTier> (tier);
}

void AnyTextGridTier_identifyClass (Function anyTextGridTier, IntervalTier *intervalTier, TextTier *textTier) {
	if (anyTextGridTier -> classInfo == classIntervalTier) {
		*intervalTier = static_cast <IntervalTier> (anyTextGridTier);
		*textTier = nullptr;
	} else {
		*intervalTier = nullptr;
		*textTier = static_cast <TextTier> (anyTextGridTier);
	}
}

integer TextGrid_countLabels (TextGrid me, integer tierNumber, conststring32 text) {
	try {
		Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		integer count = 0;
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (integer i = 1; i <= tier -> intervals.size; i ++) {
				TextInterval segment = tier -> intervals.at [i];
				if (segment -> text && str32equ (segment -> text.get(), text))
					count ++;
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (integer i = 1; i <= tier -> points.size; i ++) {
				TextPoint point = tier -> points.at [i];
				if (point -> mark && str32equ (point -> mark.get(), text))
					count ++;
			}
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, U": labels not counted.");
	}
}

integer TextGrid_countIntervalsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		integer count = 0;
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, true))
				count ++;
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, U": intervals not counted.");
	}
}

integer TextGrid_countPointsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		integer count = 0;
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark.get(), which, criterion, true))
				count ++;
		}
		return count;
	} catch (MelderError) {
		Melder_throw (me, U": points not counted.");
	}
}

void TextGrid_addTier_copy (TextGrid me, Function anyTier) {
	try {
		autoFunction tier = Data_copy (anyTier);
		Melder_clipRight (& my xmin, tier -> xmin);
		Melder_clipLeft (tier -> xmax, & my xmax);
		my tiers -> addItem_move (tier.move());
	} catch (MelderError) {
		Melder_throw (me, U": tier not added.");
	}
}

void TextGrid_addTier_move (TextGrid me, autoFunction tier) {
	try {
		Melder_clipRight (& my xmin, tier -> xmin);
		Melder_clipLeft (tier -> xmax, & my xmax);
		my tiers -> addItem_move (tier.move());
	} catch (MelderError) {
		Melder_throw (me, U": tier not added.");
	}
}

autoTextGrid TextGrids_merge (
	OrderedOf<structTextGrid>* me,
	const bool equalizeDomains
) {
	try {
		if (my size < 1)
			Melder_throw (U"Cannot merge zero TextGrid objects.");
		autoTextGrid thee = Data_copy (my at [1]);
		for (integer igrid = 2; igrid <= my size; igrid ++) {
			const constTextGrid grid = my at [igrid];
			for (integer itier = 1; itier <= grid -> tiers->size; itier ++)
				TextGrid_addTier_copy (thee.get(), grid -> tiers->at [itier]);
		}
		if (equalizeDomains)
			for (integer itier = 1; itier <= thy tiers->size; itier ++) {
				const mutableFunction anyTier = thy tiers->at [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					if (tier -> xmin > thy xmin)
						IntervalTier_addInterval_raw (tier, thy xmin, tier -> xmin, U"");
					if (tier -> xmax < thy xmax)
						IntervalTier_addInterval_raw (tier, tier -> xmax, thy xmax, U"");
				}
				anyTier -> xmin = thy xmin;
				anyTier -> xmax = thy xmax;
			}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrids not merged.");
	}
}

autoTextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, bool preserveTimes) {
	try {
		autoTextGrid thee = Data_copy (me);
		if (tmax <= tmin)
			return thee;
		if (tmin >= my xmax || tmax <= my xmin)
			Melder_throw (U"Extraction range (from ", tmin, U" to ", tmax, U" seconds) "
				"does not overlap with the time domain of the TextGrid (from ", my xmin, U" to ", my xmax, U" seconds).");

		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = thy tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				for (integer iinterval = tier -> intervals.size; iinterval >= 1; iinterval --) {
					TextInterval interval = tier -> intervals.at [iinterval];
					if (interval -> xmin >= tmax || interval -> xmax <= tmin) {
						tier -> intervals. removeItem (iinterval);
					} else {
						Melder_clipLeft (tmin, & interval -> xmin);
						Melder_clipRight (& interval -> xmax, tmax);
					}
				}
			} else {
				TextTier textTier = static_cast <TextTier> (anyTier);
				for (integer ipoint = textTier -> points.size; ipoint >= 1; ipoint --) {
					TextPoint point = textTier -> points.at [ipoint];
					if (point -> number < tmin || point -> number > tmax)
						textTier -> points. removeItem (ipoint);
				}
			}
			anyTier -> xmin = tmin;
			anyTier -> xmax = tmax;
		}
		thy xmin = tmin;
		thy xmax = tmax;
		if (! preserveTimes)
			Function_shiftXTo (thee.get(), thy xmin, 0.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

static autoTextGrid _Label_to_TextGrid (Label me, double tmin, double tmax) {
	autoTextGrid thee = TextGrid_createWithoutTiers (tmin, tmax);
	for (integer itier = 1; itier <= my size; itier ++) {
		Tier tier = my at [itier];
		autoIntervalTier intervalTier = IntervalTier_create (tmin, tmax);
		thy tiers -> addItem_move (intervalTier.move());
		intervalTier -> intervals. removeItem (1);
		for (integer iinterval = 1; iinterval <= tier->size; iinterval ++) {
			Autosegment autosegment = tier->at [iinterval];
			autoTextInterval textInterval = TextInterval_create (
				iinterval == 1 ? tmin : autosegment -> xmin,
				iinterval == tier->size ? tmax : autosegment -> xmax,
				autosegment -> name.get()
			);
			intervalTier -> intervals. addItem_move (textInterval.move());
		}
	}
	return thee;
}

autoTextGrid Label_to_TextGrid (Label me, double duration) {
	try {
		double tmin = 0.0, tmax = duration;
		if (duration == 0.0)
			Label_suggestDomain (me, & tmin, & tmax);
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

autoTextTier PointProcess_upto_TextTier (PointProcess me, conststring32 text) {
	try {
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		for (integer i = 1; i <= my nt; i ++)
			TextTier_addPoint (thee.get(), my t [i], text);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TextTier.");
	}
}

autoPointProcess TextTier_getPoints (TextTier me, conststring32 text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer i = 1; i <= my points.size; i ++) {
			TextPoint point = my points.at [i];
			if (text && text [0]) {
				if (point -> mark && str32equ (text, point -> mark.get()))
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

autoPointProcess IntervalTier_getStartingPoints (IntervalTier me, conststring32 text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (text && text [0]) {
				if (interval -> text && str32equ (text, interval -> text.get()))
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

autoPointProcess IntervalTier_getEndPoints (IntervalTier me, conststring32 text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (text && text [0]) {
				if (interval -> text && str32equ (text, interval -> text.get()))
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

autoPointProcess IntervalTier_getCentrePoints (IntervalTier me, conststring32 text) {
	try {
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (text && text [0]) {
				if (interval -> text && str32equ (text, interval -> text.get()))
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

autoPointProcess TextGrid_getStartingPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, true))
				PointProcess_addPoint (thee.get(), interval -> xmin);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": starting points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getEndPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, true))
				PointProcess_addPoint (thee.get(), interval -> xmax);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": end points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getCentrePoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, true))
				PointProcess_addPoint (thee.get(), 0.5 * (interval -> xmin + interval -> xmax));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": centre points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark.get(), which, criterion, true))
				PointProcess_addPoint (thee.get(), point -> number);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getPoints_preceded (TextGrid me, integer tierNumber,
	kMelder_string which, conststring32 criterion,
	kMelder_string precededBy, conststring32 criterion_precededBy)
{
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer ipoint = 2; ipoint <= tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark.get(), which, criterion, true)) {
				TextPoint preceding = tier -> points.at [ipoint - 1];
				if (Melder_stringMatchesCriterion (preceding -> mark.get(), precededBy, criterion_precededBy, true))
					PointProcess_addPoint (thee.get(), point -> number);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": points not converted to PointProcess.");
	}
}

autoPointProcess TextGrid_getPoints_followed (TextGrid me, integer tierNumber,
	kMelder_string which, conststring32 criterion,
	kMelder_string followedBy, conststring32 criterion_followedBy)
{
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, 10);
		for (integer ipoint = 1; ipoint < tier -> points.size; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (Melder_stringMatchesCriterion (point -> mark.get(), which, criterion, true)) {
				TextPoint following = tier -> points.at [ipoint + 1];
				if (Melder_stringMatchesCriterion (following -> mark.get(), followedBy, criterion_followedBy, true))
					PointProcess_addPoint (thee.get(), point -> number);
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
		for (integer i = 1; i <= point -> nt; i ++) {
			const double t = point -> t [i];
			const integer index = IntervalTier_timeToLowIndex (tier, t);
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
		for (integer i = 1; i <= point -> nt; i ++) {
			const double t = point -> t [i];
			const integer index = IntervalTier_timeToHighIndex (tier, t);
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

autoTableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, conststring32 label) {
	try {
		integer n = 0;
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (! label || (label [0] == U'\0' && ! interval -> text) || (interval -> text && str32equ (interval -> text.get(), label)))
				n ++;
		}
		autoTableOfReal thee = TableOfReal_create (n, 3);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Start");
		TableOfReal_setColumnLabel (thee.get(), 2, U"End");
		TableOfReal_setColumnLabel (thee.get(), 3, U"Duration");
		n = 0;
		for (integer i = 1; i <= my intervals.size; i ++) {
			TextInterval interval = my intervals.at [i];
			if (! label || (label [0] == U'\0' && ! interval -> text) || (interval -> text && str32equ (interval -> text.get(), label))) {
				n ++;
				TableOfReal_setRowLabel (thee.get(), n, interval -> text ? interval -> text.get() : U"");
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

autoTableOfReal TextTier_downto_TableOfReal (TextTier me, conststring32 label) {
	try {
		integer n = 0;
		for (integer i = 1; i <= my points.size; i ++) {
			TextPoint point = my points.at [i];
			if (! label || (label [0] == U'\0' && ! point -> mark) || (point -> mark && str32equ (point -> mark.get(), label)))
				n ++;
		}
		autoTableOfReal thee = TableOfReal_create (n, 1);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Time");
		n = 0;
		for (integer i = 1; i <= my points.size; i ++) {
			TextPoint point = my points.at [i];
			if (! label || (label [0] == U'\0' && ! point -> mark) || (point -> mark && str32equ (point -> mark.get(), label))) {
				n ++;
				TableOfReal_setRowLabel (thee.get(), n, point -> mark ? point -> mark.get() : U"");
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

autoTextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT) {
	try {
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, U"vuv", nullptr);
		IntervalTier tier = static_cast <IntervalTier> (thy tiers->at [1]);
		tier -> intervals. removeItem (1);
		integer ipointright;
		double beginVoiceless = my xmin, endVoiceless, halfMeanT = 0.5 * meanT;
		for (integer ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
			endVoiceless = my t [ipointleft] - halfMeanT;
			if (endVoiceless <= beginVoiceless) {
				endVoiceless = beginVoiceless;   // we will use for voiced interval
			} else {
				IntervalTier_addInterval_raw (tier, beginVoiceless, endVoiceless, U"U");
			}
			for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
				if (my t [ipointright] - my t [ipointright - 1] > maxT)
					break;
			ipointright --;
			beginVoiceless = my t [ipointright] + halfMeanT;
			if (beginVoiceless > my xmax)
				beginVoiceless = my xmax;
			IntervalTier_addInterval_raw (tier, endVoiceless, beginVoiceless, U"V");
		}
		endVoiceless = my xmax;
		if (endVoiceless > beginVoiceless)
			IntervalTier_addInterval_raw (tier, beginVoiceless, endVoiceless, U"U");
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TextGrid (vuv).");
	}
}

integer TextInterval_labelLength (TextInterval me) {
	return Melder_length (my text.get());
}

integer TextPoint_labelLength (TextPoint me) {
	return Melder_length (my mark.get());
}

integer IntervalTier_maximumLabelLength (IntervalTier me) {
	integer maximum = 0;
	for (integer iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
		integer length = TextInterval_labelLength (my intervals.at [iinterval]);
		if (length > maximum)
			maximum = length;
	}
	return maximum;
}

integer TextTier_maximumLabelLength (TextTier me) {
	integer maximum = 0;
	for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
		integer length = TextPoint_labelLength (my points.at [ipoint]);
		if (length > maximum)
			maximum = length;
	}
	return maximum;
}

integer TextGrid_maximumLabelLength (TextGrid me) {
	integer maximum = 0;
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		integer length = anyTier -> classInfo == classIntervalTier ?
			IntervalTier_maximumLabelLength ((IntervalTier) anyTier) :
			TextTier_maximumLabelLength ((TextTier) anyTier);
		if (length > maximum)
			maximum = length;
	}
	return maximum;
}

static void genericize (autostring32& stringRef, mutablestring32 buffer) {
	if (stringRef) {
		const char32 *p = & stringRef [0];
		while (*p) {
			if (*p > 126) {   // OPTIMIZE: allocate a new string only if necessary
				Longchar_genericize (stringRef.get(), buffer);
				stringRef = Melder_dup (buffer);
				break;
			}
			p ++;
		}
	}
}

void TextGrid_convertToBackslashTrigraphs (TextGrid me) {
	try {
		autostring32 buffer (TextGrid_maximumLabelLength (me) * 3);   // OPTIMIZE: use only one allocation if more are not necessary
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				for (integer i = 1; i <= tier -> intervals.size; i ++) {
					TextInterval interval = tier -> intervals.at [i];
					genericize (interval -> text, buffer.get());
				}
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				for (integer i = 1; i <= tier -> points.size; i ++) {
					TextPoint point = tier -> points.at [i];
					genericize (point -> mark, buffer.get());
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not converted to backslash trigraphs.");
	}
}

void TextGrid_convertToUnicode (TextGrid me) {
	try {
		autostring32 buffer (TextGrid_maximumLabelLength (me));
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				for (integer i = 1; i <= tier -> intervals.size; i ++) {
					TextInterval interval = tier -> intervals.at [i];
					if (interval -> text) {
						Longchar_nativize (interval -> text.get(), buffer.get(), false);
						str32cpy (interval -> text.get(), buffer.get());
					}
				}
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				for (integer i = 1; i <= tier -> points.size; i ++) {
					TextPoint point = tier -> points.at [i];
					if (point -> mark) {
						Longchar_nativize (point -> mark.get(), buffer.get(), false);
						str32cpy (point -> mark.get(), buffer.get());
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": backslash trigraphs not converted to Unicode.");
	}
}

void TextInterval_removeText (TextInterval me) {
	my text. reset();
}

void TextPoint_removeText (TextPoint me) {
	my mark. reset();
}

void IntervalTier_removeText (IntervalTier me) {
	integer numberOfIntervals = my intervals.size;
	for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++)
		TextInterval_removeText (my intervals.at [iinterval]);
}

void TextTier_removeText (TextTier me) {
	integer numberOfPoints = my points.size;
	for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++)
		TextPoint_removeText (my points.at [ipoint]);
}

void TextGrid_insertBoundary (TextGrid me, integer tierNumber, double t) {
	try {
		Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me, tierNumber);
		if (anyTier -> classInfo != classIntervalTier)
			Melder_throw (U"Cannot add a boundary on tier ", tierNumber, U", because that tier is not an interval tier.");
		IntervalTier intervalTier = static_cast <IntervalTier> (anyTier);
		if (IntervalTier_hasTime (intervalTier, t))
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because there is already a boundary there.");
		integer intervalNumber = IntervalTier_timeToIndex (intervalTier, t);
		if (intervalNumber == 0)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = intervalTier -> intervals.at [intervalNumber];
		/*
			Move the text to the left of the boundary.
		*/
		autoTextInterval newInterval = TextInterval_create (t, interval -> xmax, U"");
		interval -> xmax = t;
		intervalTier -> intervals. addItem_move (newInterval.move());
	} catch (MelderError) {
		Melder_throw (me, U": boundary not inserted.");
	}
}

void IntervalTier_removeLeftBoundary (IntervalTier me, integer intervalNumber) {
	try {
		Melder_assert (intervalNumber > 1);
		Melder_assert (intervalNumber <= my intervals.size);
		TextInterval left = my intervals.at [intervalNumber - 1];
		TextInterval right = my intervals.at [intervalNumber];
		/*
			Move the text to the left of the boundary.
		*/
		left -> xmax = right -> xmax;   // collapse left and right intervals into left interval
		if (! right -> text) {
			;
		} else if (! left -> text) {
			TextInterval_setText (left, right -> text.get());
		} else {
			TextInterval_setText (left, Melder_cat (left -> text.get(), right -> text.get()));
		}
		my intervals. removeItem (intervalNumber);   // remove right interval
	} catch (MelderError) {
		Melder_throw (me, U": left boundary not removed.");
	}
}

void TextGrid_removeBoundaryAtTime (TextGrid me, integer tierNumber, double t) {
	try {
		IntervalTier intervalTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		if (! IntervalTier_hasTime (intervalTier, t))
			Melder_throw (U"There is no boundary at ", t, U" seconds.");
		integer intervalNumber = IntervalTier_timeToIndex (intervalTier, t);
		if (intervalNumber == 0)
			Melder_throw (U"The time of ", t, U" seconds is outside the time domain of the intervals.");
		if (intervalNumber == 1)
			Melder_throw (U"The time of ", t, U" seconds is at the left edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, intervalNumber);
	} catch (MelderError) {
		Melder_throw (me, U": boundary not removed.");
	}
}

void IntervalTier_moveLeftBoundary (const IntervalTier me, const integer intervalNumber, const double newTime) {
	try {
		Melder_require (intervalNumber >= 1 && intervalNumber <= my intervals.size,
			U"The interval number (", intervalNumber, U" is out of the valid range (1 ..", my intervals.size, U").");
		TextInterval currentInterval = my intervals.at [intervalNumber];
		Melder_require (newTime < currentInterval -> xmax,
			U"Cannot move boundary forward from ", currentInterval -> xmin, U" to ", newTime, U" seconds, ",
			U"because that would be past the end of the current interval (", currentInterval -> xmax, U" seconds).");
		Melder_require (intervalNumber > 1,
			U"Trying to change the end of the previous interval (", intervalNumber - 1, U"), but there is no previous interval.");
		Melder_assert (my intervals.size >= 2);   // otherwise we would have thrown
		const TextInterval previousInterval = my intervals.at [intervalNumber - 1];
		Melder_require (newTime > previousInterval -> xmin,
			U"Cannot move boundary back from ", currentInterval -> xmin, U" to ", newTime, U" seconds, ",
			U"because that would be past the start of the previous interval (", previousInterval -> xmin, U" seconds).");
		previousInterval -> xmax = currentInterval -> xmin = newTime;
	} catch (MelderError) {
		Melder_throw (me, U": left boundary not moved.");
	}
}

void IntervalTier_moveRightBoundary (const IntervalTier me, const integer intervalNumber, const double newTime) {
	try {
		Melder_require (intervalNumber >= 1 && intervalNumber <= my intervals.size,
			U"The interval number (", intervalNumber, U" is out of the valid range (1 ..", my intervals.size, U").");
		TextInterval currentInterval = my intervals.at [intervalNumber];
		Melder_require (newTime > currentInterval -> xmin,
			U"Cannot move boundary back from ", currentInterval -> xmax, U" to ", newTime, U" seconds, ",
			U"because that would be past the start of the current interval (", currentInterval -> xmin, U" seconds).");
		Melder_require (intervalNumber < my intervals.size,
			U"Trying to change the start of the next interval (", intervalNumber + 1, U"), but there is no next interval.");
		Melder_assert (my intervals.size >= 2);   // otherwise we would have thrown
		const TextInterval nextInterval = my intervals.at [intervalNumber + 1];
		Melder_require (newTime < nextInterval -> xmax,
			U"Cannot move boundary forward from ", currentInterval -> xmax, U" to ", newTime, U" seconds, ",
			U"because that would be past the end of the next interval (", nextInterval -> xmax, U" seconds)."
		);
		nextInterval -> xmin = currentInterval -> xmax = newTime;
	} catch (MelderError) {
		Melder_throw (me, U": right boundary not moved.");
	}
}

void TextGrid_setIntervalText (TextGrid me, integer tierNumber, integer intervalNumber, conststring32 text) {
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

void TextGrid_insertPoint (TextGrid me, integer tierNumber, double time, conststring32 mark) {
	try {
		TextTier textTier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		Melder_assert (isdefined (time));
		if (AnyTier_hasPoint (textTier->asAnyTier(), time))
			Melder_throw (U"There is already a point at ", time, U" seconds.");
		autoTextPoint newPoint = TextPoint_create (time, mark);
		textTier -> points. addItem_move (newPoint.move());
	} catch (MelderError) {
		Melder_throw (me, U": point not inserted.");
	}
}

void TextTier_removePoint (TextTier me, integer ipoint) {
	Melder_assert (ipoint <= my points.size);
	my points. removeItem (ipoint);
}

void TextTier_removePoints (TextTier me, kMelder_string which, conststring32 criterion) {
	for (integer i = my points.size; i > 0; i --)
		if (Melder_stringMatchesCriterion (my points.at [i] -> mark.get(), which, criterion, true))
			my points. removeItem (i);
}

void TextGrid_removePoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		TextTier tier = TextGrid_checkSpecifiedTierIsPointTier (me, tierNumber);
		TextTier_removePoints (tier, which, criterion);
	} catch (MelderError) {
		Melder_throw (me, U": points not removed.");
	}
}

void TextGrid_setPointText (TextGrid me, integer tierNumber, integer pointNumber, conststring32 text) {
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

autoTable TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, integer timeDecimals, bool includeTierNames, bool includeEmptyIntervals) {
	integer numberOfRows = 0;
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			if (includeEmptyIntervals) {
				numberOfRows += tier -> intervals.size;
			} else {
				for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
					TextInterval interval = tier -> intervals.at [iinterval];
					if (interval -> text && interval -> text [0] != U'\0')
						numberOfRows ++;
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			numberOfRows += tier -> points.size;
		}
	}
	autoTable thee = Table_createWithoutColumnNames (numberOfRows, 3 + includeLineNumbers + includeTierNames);
	integer icol = 0;
	if (includeLineNumbers)
		Table_renameColumn_e (thee.get(), ++ icol, U"line");
	Table_renameColumn_e (thee.get(), ++ icol, U"tmin");
	const integer tmin_columnNumber = icol;
	if (includeTierNames)
		Table_renameColumn_e (thee.get(), ++ icol, U"tier");
	Table_renameColumn_e (thee.get(), ++ icol, U"text");
	Table_renameColumn_e (thee.get(), ++ icol, U"tmax");
	const integer tmax_columnNumber = icol;
	integer irow = 0;
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
				TextInterval interval = tier -> intervals.at [iinterval];
				if (includeEmptyIntervals || (interval -> text && interval -> text [0] != U'\0')) {
					++ irow;
					icol = 0;
					if (includeLineNumbers)
						Table_setNumericValue (thee.get(), irow, ++ icol, irow);
					Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (interval -> xmin, timeDecimals));
					if (includeTierNames)
						Table_setStringValue (thee.get(), irow, ++ icol, tier -> name.get());
					Table_setStringValue (thee.get(), irow, ++ icol, interval -> text.get());
					Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (interval -> xmax, timeDecimals));
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
				TextPoint point = tier -> points.at [ipoint];
				++ irow;
				icol = 0;
				if (includeLineNumbers)
					Table_setNumericValue (thee.get(), irow, ++ icol, irow);
				Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (point -> number, timeDecimals));
				if (includeTierNames)
					Table_setStringValue (thee.get(), irow, ++ icol, tier -> name.get());
				Table_setStringValue (thee.get(), irow, ++ icol, point -> mark.get());
				Table_setStringValue (thee.get(), irow, ++ icol, Melder_fixed (point -> number, timeDecimals));
			}
		}
	}
	Table_sortRows_a (thee.get(), autoINTVEC ({ tmin_columnNumber, tmax_columnNumber }).get());   // sort by tmin and tmax
	return thee;
}

autoTable TextGrid_tabulateOccurrences (TextGrid me, constVEC searchTiers, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	constexpr int timeDecimals = 6;
	integer numberOfRows = 0;
	for (integer itier = 1; itier <= searchTiers.size; itier ++) {
		integer tierNumber = Melder_iround (searchTiers [itier]);
		Melder_require (tierNumber > 0 && tierNumber <= my tiers->size, U"Tier number out of range.");
		Function anyTier = my tiers->at [tierNumber];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
				TextInterval interval = tier -> intervals.at [iinterval];
				if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, caseSensitive))
					numberOfRows ++;
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
				TextPoint point = tier -> points.at [ipoint];
				if (Melder_stringMatchesCriterion (point -> mark.get(), which, criterion, caseSensitive))
					numberOfRows ++;
			}
		}
	}
	autoTable thee = Table_createWithColumnNames (numberOfRows,
			autoSTRVEC ({ U"time", U"tier", U"text" }).get());
	constexpr integer time_columnNumber = 1;
	constexpr integer tier_columnNumber = 2;
	constexpr integer text_columnNumber = 3;
	integer rowNumber = 0;
	for (integer itier = 1; itier <= searchTiers.size; itier ++) {
		integer tierNumber = Melder_iround (searchTiers [itier]);
		Function anyTier = my tiers->at [tierNumber];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
				TextInterval interval = tier -> intervals.at [iinterval];
				if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, caseSensitive)) {
					++ rowNumber;
					Melder_assert (rowNumber <= numberOfRows);
					const double time = 0.5 * (interval -> xmin + interval -> xmax);
					Table_setStringValue (thee.get(), rowNumber, time_columnNumber, Melder_fixed (time, timeDecimals));
					Table_setStringValue (thee.get(), rowNumber, tier_columnNumber, tier -> name.get());
					Table_setStringValue (thee.get(), rowNumber, text_columnNumber, interval -> text.get());
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
				TextPoint point = tier -> points.at [ipoint];
				if (Melder_stringMatchesCriterion (point -> mark.get(), which, criterion, caseSensitive)) {
					++ rowNumber;
					Melder_assert (rowNumber <= numberOfRows);
					const double time = point -> number;
					Table_setStringValue (thee.get(), rowNumber, time_columnNumber, Melder_fixed (time, timeDecimals));
					Table_setStringValue (thee.get(), rowNumber, tier_columnNumber, tier -> name.get());
					Table_setStringValue (thee.get(), rowNumber, text_columnNumber, point -> mark.get());
				}
			}
		}
	}
	Table_sortRows_a (thee.get(), autoINTVEC ({ time_columnNumber }).get());   // sort by time
	return thee;
}

void TextGrid_list (
	const TextGrid me,
	const bool includeLineNumbers,
	const integer timeDecimals,
	const bool includeTierNames,
	const bool includeEmptyIntervals
) {
	try {
		autoTable table = TextGrid_downto_Table (me, includeLineNumbers, timeDecimals, includeTierNames, includeEmptyIntervals);
		Table_list (table.get(), false);
	} catch (MelderError) {
		Melder_throw (me, U": not listed.");
	}
}

void TextGrid_correctRoundingErrors (TextGrid me) {
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			Melder_assert (tier -> intervals.size > 0);
			TextInterval first = tier -> intervals.at [1];
			first -> xmin = my xmin;
			Melder_require (first -> xmin < first -> xmax,
				U"Interval 1 of tier ", itier,
				U" has an empty time domain, running from ", first -> xmin, U" to ", first -> xmax, U" seconds."
			);
			for (integer iinterval = 1; iinterval < tier -> intervals.size; iinterval ++) {
				TextInterval left = tier -> intervals.at [iinterval];
				TextInterval right = tier -> intervals.at [iinterval + 1];
				right -> xmin = left -> xmax;
				trace (U"tier ", itier, U", interval ", iinterval, U", ", right -> xmin, U" ", right -> xmax);
				Melder_require (right -> xmin < right -> xmax,
					U"Interval ", iinterval + 1, U" of tier ", itier,
					U" has an empty time domain, running from ", first -> xmin, U" to ", first -> xmax, U" seconds."
				);
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

void TextGrid_checkInvariants_e (const constTextGrid me, const bool includeWeakInvariants) {
	/*
		Strong invariant: positive domain of TextGrid.
	*/
	Melder_require (my xmax > my xmin,
		U"The end time of the TextGrid should be greater than its start time (", my xmin,
		U" seconds), but is ", my xmax, U" seconds instead."
	);
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		Function anyTier = my tiers->at [itier];
		/*
			Strong invariant: positive domain of tier.
		*/
		Melder_require (anyTier -> xmax > anyTier -> xmin,
			U"The end time of tier ", itier, U" should be greater than its start time (", anyTier -> xmin,
			U" seconds), but is ", anyTier -> xmax, U" seconds instead."
		);
		/*
			Weak invariant: matching domains of tier and TextGrid.
		*/
		if (includeWeakInvariants) {
			Melder_require (anyTier -> xmin == my xmin,
				U"The start time of tier ", itier, U" should equal the start time of the TextGrid (", my xmin,
				U" seconds), but is ", anyTier -> xmin, U" seconds instead."
			);
			Melder_require (anyTier -> xmax == my xmax,
				U"The end time of tier ", itier, U" should equal the end time of the TextGrid (", my xmax,
				U" seconds), but is ", anyTier -> xmax, U" seconds instead."
			);
		}
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			/*
				Strong invariant: interval tier structure.
			*/
			Melder_require (tier -> intervals.size >= 1,
				U"Tier ", itier, U" should contain at least 1 interval, but it contains none.");
			/*
				Weak invariant: matching domains of interval and tier.
			*/
			if (includeWeakInvariants) {
				const constTextInterval firstInterval = tier -> intervals.at [1];
				Melder_require (firstInterval -> xmin == my xmin,
					U"The start time of the first interval of tier ", itier, U" should equal the start time of the TextGrid (", my xmin,
					U" seconds), but is ", firstInterval -> xmin, U" seconds instead."
				);
				const constTextInterval lastInterval = tier -> intervals.at [tier -> intervals.size];
				Melder_require (lastInterval -> xmax == my xmax,
					U"The end time of the last interval of tier ", itier, U" should equal the end time of the TextGrid (", my xmax,
					U" seconds), but is ", lastInterval -> xmax, U" seconds instead."
				);
			}
			/*
				Strong invariant: positive domain of interval.
			*/
			for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
				const constTextInterval interval = tier -> intervals.at [iinterval];
				Melder_require (interval -> xmax > interval -> xmin,
					U"The end time of interval ", iinterval, U" of tier ", itier,
					U" should be greater than its start time (", interval -> xmin,
					U" seconds), but is ", interval -> xmax, U" seconds instead."
				);
			}
			/*
				Strong invariant: adjacency of intervals.
			*/
			for (integer iinterval = 2; iinterval <= tier -> intervals.size; iinterval ++) {
				const constTextInterval previousInterval = tier -> intervals.at [iinterval - 1];
				const constTextInterval currentInterval = tier -> intervals.at [iinterval];
				Melder_require (currentInterval -> xmin == previousInterval -> xmax,
					U"The start time of interval ", iinterval, U" of tier ", itier,
					U" should equal the end time of interval ", iinterval - 1, U" (", previousInterval -> xmax,
					U" seconds), but is ", currentInterval -> xmin, U" seconds instead."
				);
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			/*
				Weak invariant: matching domains of point and tier.
			*/
			if (includeWeakInvariants) {
				for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
					const constTextPoint point = tier -> points.at [ipoint];
					Melder_require (point -> number >= my xmin,
						U"The time of point ", ipoint, U" of tier ", itier, U" should lie within the time domain of the TextGrid (",
						my xmin, U" .. ", my xmax, U" seconds), but is ", point -> number, U" seconds instead."
					);
				}
			}
			/*
				Strong invariant: strict order of points.
			*/
			for (integer ipoint = 2; ipoint <= tier -> points.size; ipoint ++) {
				const constTextPoint previousPoint = tier -> points.at [ipoint - 1];
				const constTextPoint currentPoint = tier -> points.at [ipoint];
				Melder_require (currentPoint -> number > previousPoint -> number,
					U"The time of point ", ipoint, U" of tier ", itier,
					U" should be greater than the time of point ", ipoint - 1, U" (", previousPoint -> number,
					U" seconds), but is ", currentPoint -> number, U" seconds instead."
				);
			}
		} // endif tier class
	} // next tier
}

void TextGrid_scaleTimes_e (mutableTextGrid me, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	try {
		Melder_require (xminfrom >= my xmin,
			U"xminfrom should not be less than the start time of the TextGrid.");
		Melder_require (xmaxfrom > xminfrom,
			U"xmaxfrom should be greater than xminfrom.");
		Melder_require (xmaxfrom <= my xmax,
			U"xmaxfrom should not be greater than the end time of the TextGrid.");
		Melder_require (xminto >= my xmin,
			U"xminto should not be less than the start time of the TextGrid.");
		Melder_require (xmaxto > xminto,
			U"xmaxto should be greater than xminto.");
		Melder_require (xmaxto <= my xmax,
			U"xmaxto should not be greater than the end time of the TextGrid.");
		TextGrid_checkInvariants_e (me, true);
		if (xminto == xminfrom && xmaxto == xmaxfrom)
			return;   // nothing to do
		/*
			Check overlap.
		*/
		if (xminto < xminfrom) {
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				const constFunction anyTier = my tiers->at [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					const constIntervalTier tier = static_cast <constIntervalTier> (anyTier);
					for (integer iinterval = 2; iinterval <= tier -> intervals.size; iinterval ++) {
						const constTextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> xmin > xminfrom) {
							/* mutable before-after */ double xmin = interval -> xmin;
							NUMscale (& xmin, xminfrom, xmaxfrom, xminto, xmaxto);
							if (xmin > tier -> intervals.at [iinterval - 1] -> xmax)
								break;   // no need to check later intervals
							Melder_throw (U"Cannot move boundary from ", interval -> xmin, U" seconds to ", xmin, U" seconds, "
									"because a boundary at ", tier -> intervals.at [iinterval - 1] -> xmax, U" is in the way.");
						}
					} // next interval
				} else {
					const constTextTier tier = static_cast <constTextTier> (anyTier);
					for (integer ipoint = 2; ipoint <= tier -> points.size; ipoint ++) {
						const constTextPoint point = tier -> points.at [itier];
						if (point -> number > xminfrom) {
							/* mutable before-after */ double time = point -> number;
							NUMscale (& time, xminfrom, xmaxfrom, xminto, xmaxto);
							if (time > tier -> points.at [ipoint - 1] -> number)
								break;   // no need to check later points
							Melder_throw (U"Cannot move point from ", point -> number, U" seconds to ", time, U" seconds, "
									"because a point at ", tier -> points.at [ipoint - 1] -> number, U" is in the way.");
						}
					} // next point
				}
			} // next tier
		}
		if (xmaxto > xmaxfrom) {
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				const constFunction anyTier = my tiers->at [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					const constIntervalTier tier = static_cast <constIntervalTier> (anyTier);
					for (integer iinterval = tier -> intervals.size - 1; iinterval >= 1; iinterval --) {
						const constTextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> xmax < xmaxfrom) {
							/* mutable before-after */ double xmax = interval -> xmax;
							NUMscale (& xmax, xminfrom, xmaxfrom, xminto, xmaxto);
							if (xmax < tier -> intervals.at [iinterval + 1] -> xmin)
								break;
							Melder_throw (U"Cannot move boundary from ", interval -> xmax, U" seconds to ", xmax, U" seconds, "
									"because a boundary at ", tier -> intervals.at [iinterval + 1] -> xmin, U" is in the way.");
						}
					} // next interval
				} else {
					const constTextTier tier = static_cast <constTextTier> (anyTier);
					for (integer ipoint = tier -> points.size - 1; ipoint >= 1 ; ipoint --) {
						const constTextPoint point = tier -> points.at [itier];
						if (point -> number < xmaxfrom) {
							/* mutable before-after */ double time = point -> number;
							NUMscale (& time, xminfrom, xmaxfrom, xminto, xmaxto);
							if (time < tier -> points.at [ipoint + 1] -> number)
								break;
							Melder_throw (U"Cannot move point from ", point -> number, U" seconds to ", time, U" seconds, "
									"because a point at ", tier -> points.at [ipoint + 1] -> number, U" is in the way.");
						}
					} // next point
				}
			} // next tier
		}
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			const mutableFunction anyTier = my tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				const mutableIntervalTier tier = static_cast <mutableIntervalTier> (anyTier);
				for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
					const mutableTextInterval interval = tier -> intervals.at [iinterval];
					NUMscale (& interval -> xmin, xminfrom, xmaxfrom, xminto, xmaxto);
					NUMscale (& interval -> xmax, xminfrom, xmaxfrom, xminto, xmaxto);
				}
			} else {
				const mutableTextTier tier = static_cast <mutableTextTier> (anyTier);
				for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
					const mutableTextPoint point = tier -> points.at [ipoint];
					NUMscale (& point -> number, xminfrom, xmaxfrom, xminto, xmaxto);
				}
			}
		} // next tier
		TextGrid_checkInvariants_e (me, true);
	} catch (MelderError) {
		Melder_throw (me, U": times not scaled.");
	}
}

/* End of file TextGrid.cpp */
