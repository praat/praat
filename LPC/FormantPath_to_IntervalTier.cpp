/* FormantPath_to_IntervalTier.cpp
 *
 * Copyright (C) 2020 David Weenink
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

#include "FormantPath_to_IntervalTier.h"

static void IntervalTier_addCandidate (IntervalTier me, double splitTime, integer candidate) {
	conststring32 label =  Melder_integer (candidate);
	TextInterval last = my intervals.at [my intervals.size];
	last -> xmax = splitTime;
	last -> text = Melder_dup (label);
	autoTextInterval interval = TextInterval_create (splitTime, my xmax, U"");
	my intervals.addItem_move (interval.move());
}

autoIntervalTier FormantPath_to_IntervalTier (FormantPath me, double tmin, double tmax) {
	autoIntervalTier thee = IntervalTier_create (tmin, tmax);
	integer itmin, itmax;
	Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax);
	/*
		If the interval [tmin,tmax] did not overlap any samples and falls 
		inbetween two successive samples that have index i1 and i1+1, 
		itmin == i1+1 && itmax == i1;
	*/
	integer startIndex = Sampled_xToNearestIndex (me, tmin);
	Melder_clipLeft (1_integer, & startIndex);
	integer candidate = my path [startIndex];
	for (integer itime = itmin; itime <= itmax; itime ++) {
		if (my path [itime] != candidate) {
			const double endTime = Sampled_indexToX (me, itime) - 0.5 * my dx;
			IntervalTier_addCandidate (thee.get(), endTime, candidate); 
			candidate = my path [itime];
		}
	}
	integer endIndex = Sampled_xToNearestIndex (me, tmax);
	Melder_clipRight (& endIndex, my nx);
	integer endCandidate = my path [endIndex];
	if (endCandidate != candidate) {
		const double endTime = Sampled_indexToX (me, itmax) + 0.5 * my dx;
		IntervalTier_addCandidate (thee.get(), endTime, candidate); 
		candidate = endCandidate;
	}
	/*
		Set label on last interval
	*/
	const TextInterval textInterval = thy intervals.at [thy intervals.size];
	TextInterval_setText (textInterval, Melder_integer (candidate));
	return thee;
}

/* End of file FormantPath_to_IntervalTier.cpp */
