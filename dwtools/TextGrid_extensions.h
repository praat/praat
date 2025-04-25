#ifndef _TextGrid_extensions_h_
#define _TextGrid_extensions_h_
/* TextGrid_extensions.h
 *
 * Copyright (C) 1993-2018,2023 David Weenink
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

/*
 djmw 20020516 GPL header
 djmw 20120418 Latest modification
*/

#include "TextGrid.h"

void TextGrid_extendTime (TextGrid me, double delta_time, int position);
/*
	Extend the begin-time (delta_time<0) or end-time (delta_time>0).
	For Point-tiers only the domain will be extended.
	Interval tiers will have a new (empty) interval at the start or the end.
*/

void TextGrid_setTierName (TextGrid me, integer itier, conststring32 newName);

void TextTier_changeLabels (TextTier me, integer from, integer to, conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches);

void IntervalTier_changeLabels (IntervalTier me, integer from, integer to, conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches);

void TextGrid_addInterval_force (TextGrid me, double tmin, double tmax, integer tierNumber, conststring32 newLabel);

void IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (IntervalTier me, conststring32 label);

void IntervalTier_combineIntervalsOnLabelMatch (IntervalTier me, conststring32 label);

void IntervalTier_cutIntervals_minimumDuration (IntervalTier me, conststring32 label, double minimumDuration);

void TextGrid_changeLabels (TextGrid me, integer tier, integer from, integer to, conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches);

/* Set the start/end time to a smaller/larger value.
 * If mark is null, only times are changed
 * If mark != null, mark the previous start/end time
 *    For a TextTier this involves adding a point with the marker
 *    For an IntervalTier this involves adding a new interval
 */
void IntervalTier_setLaterEndTime (IntervalTier me, double xmax, conststring32 mark);

void IntervalTier_setEarlierStartTime (IntervalTier me, double xmin, conststring32 mark);

void TextTier_setLaterEndTime (TextTier me, double xmax, conststring32 mark);

void TextTier_setEarlierStartTime (TextTier me, double xmin, conststring32 mark);

void TextGrid_setEarlierStartTime (TextGrid me, double xmin, conststring32 intervalMark, conststring32 pointMark);

void TextGrid_setLaterEndTime (TextGrid me, double xmax, conststring32 intervalMark, conststring32 pointMark);


// Precondition: if (preserveTimes) { my xmax <= thy xmin }
// Postcondition: my xmin preserved
void IntervalTiers_append_inplace (IntervalTier me, IntervalTier thee, bool preserveTimes);

void TextTiers_append_inplace (TextTier me, TextTier thee, bool preserveTimes);

void TextGrids_append_inplace (TextGrid me, TextGrid thee, bool preserveTimes);

autoTextGrid TextGrids_to_TextGrid_appendContinuous (OrderedOf<structTextGrid>* me, bool preserveTimes);

double TextGrid_getTotalDurationOfIntervalsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);

#endif /* _TextGrid_extensions_h_ */
