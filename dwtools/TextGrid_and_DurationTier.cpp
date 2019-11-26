/* TextGrid_and_DurationTier.cpp
 *
 * Copyright (C) 2017-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TextGrid_and_DurationTier.h"
#include "Thing.h"

void IntervalTier_DurationTier_scaleTimes (IntervalTier me, DurationTier thee) {
	Melder_require (my xmin == thy xmin && my xmax == thy xmax,
		U"The domains of the IntervalTier and the DurationTier should be equal.");
	const double xmax_new = my xmin + RealTier_getArea (thee, my xmin, my xmax);
	for (integer i = 1; i <= my intervals.size; i ++) {
		const TextInterval segment = my intervals.at [i];
		const double xmin = RealTier_getArea (thee, my xmin, segment -> xmin);
		const double xmax = RealTier_getArea (thee, my xmin, segment -> xmax);
		segment -> xmin = my xmin + xmin;
		segment -> xmax = my xmin + xmax;
	}
	my xmax = xmax_new;
}

void TextTier_DurationTier_scaleTimes (TextTier me, DurationTier thee) {
	Melder_require (my xmin == thy xmin && my xmax == thy xmax,
		U"The domains of the TextTier and the DurationTier should be equal.");
	const double xmax_new = my xmin + RealTier_getArea (thee, my xmin, my xmax);
	for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
		const TextPoint point = my points.at [ipoint];
		const double time = RealTier_getArea (thee, my xmin, point -> number);
		point -> number = time;
	}
	my xmax = xmax_new;
}

autoTextGrid TextGrid_DurationTier_scaleTimes (TextGrid me, DurationTier thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"The domains of the TextGrid and the DurationTier should be equal.");
		
		const double xmax_new = my xmin + RealTier_getArea (thee, my xmin, my xmax);
		autoTextGrid him = Data_copy (me);
		const integer numberOfTiers = my tiers -> size;
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			Function anyTier = his tiers->at [itier];
			if (anyTier -> classInfo == classIntervalTier) {
				const IntervalTier tier = static_cast <IntervalTier> (anyTier);
				IntervalTier_DurationTier_scaleTimes (tier, thee);
			} else {
				const TextTier textTier = static_cast <TextTier> (anyTier);
				TextTier_DurationTier_scaleTimes (textTier, thee);
			}
		}
		his xmax = xmax_new;
		return him;
		
	} catch (MelderError) {
		Melder_throw (me, U": no time-scaled TextGrid created.");
	}
}

autoDurationTier TextGrid_to_DurationTier (TextGrid me, integer tierNumber,
	double timeScalefactor, double leftTransitionDuration, double rightTransitionDuration, kMelder_string which, conststring32 criterion)
{
	try {
		autoDurationTier him = DurationTier_create (my xmin, my xmax);
		const IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		for (integer i = 1; i <= tier ->intervals.size; i ++) {
			const TextInterval segment = tier -> intervals.at [i];
			if (Melder_stringMatchesCriterion (segment -> text.get(), which, criterion, true)) {
				const double xmin = segment -> xmin, xmax = segment -> xmax;
				RealTier_addPoint (him.get(), xmin, 1.0);
				RealTier_addPoint (him.get(), xmin + leftTransitionDuration, timeScalefactor);
				RealTier_addPoint (him.get(), xmax - rightTransitionDuration, timeScalefactor);
				RealTier_addPoint (him.get(), xmax, 1.0);	
			}
		}
		const integer index = tier ->intervals.size;
		if (index == 0)
			RealTier_addPoint (him.get(), my xmin, 1.0);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U":cannot create DurationTier.");
	}
}

/* End of file TextGrid_and_DurationTier.cpp */
