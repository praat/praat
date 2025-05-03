/* TextGrid_extensions.cpp
 *
 * Copyright (C) 1993-2019,2023 David Weenink, 2015-2022,2024 Paul Boersma
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
 djmw 20020702 GPL header
 djmw 20020702 +TextGrid_extendTime
 djmw 20051215 Corrected a bug in TextGrid_readFromTIMITLabelFile that caused a crash when the first number in
 	a file was not 0 (in that case an empty interval was added as the first element in the tier).
 djmw 20060517 Added (TextTier|IntervalTier|TextGrid)_changeLabels.
 djmw 20060712 TextGrid_readFromTIMITLabelFile: don't set first boundary to zero for .wrd files.
 djmw 20060921 Added IntervalTier_cutIntervalOnLabelMatch, IntervalTier_cutIntervals_minimumDuration
 djmw 20061113 Reassign item in list after a deletion.
 djmw 20061116 Added IntervalTier_cutInterval to correct a bug in IntervalTier_removeBoundary...
 djmw 20071008 Removed two unused variables.
 djmw 20071202 Melder_warning<n>
*/

#include "TextGrid_extensions.h"
#include "NUM2.h"

void IntervalTier_setLaterEndTime (IntervalTier me, double xmax, conststring32 mark) {
	try {
		if (xmax <= my xmax)
			return; // nothing to be done
		Melder_assert (my intervals.size > 0);
		const TextInterval ti = my intervals.at [my intervals.size];
		/*
			The following assert signals that the IntervalTier is not correct:
			the xmax of the last inteval is not equal to the xmax of the IntervalTier.
		*/
		Melder_assert (xmax > ti -> xmax);
		if (mark) {
			autoTextInterval interval = TextInterval_create (ti -> xmax, xmax, mark);
			my intervals. addItem_move (interval.move());
		} else {
			/*
				Extend last interval
			*/
			ti -> xmax = xmax;
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of IntervalTier not set.");
	}
}

void IntervalTier_setEarlierStartTime (IntervalTier me, double xmin, conststring32 mark) {
	try {
		if (xmin >= my xmin)
			return;
		Melder_assert (my intervals.size > 0);
		const TextInterval ti = my intervals.at [1];
		Melder_assert (xmin < ti -> xmin);
		if (mark) {
			autoTextInterval interval = TextInterval_create (xmin, ti -> xmin, mark);
			my intervals. addItem_move (interval.move());
		} else {
			/*
				Extend first interval
			*/
			ti -> xmin = xmin;
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of IntervalTier not set.");
	}
}

void TextTier_setLaterEndTime (TextTier me, double xmax, conststring32 mark) {
	try {
		if (xmax <= my xmax)
			return;
		if (mark) {
			autoTextPoint textpoint = TextPoint_create (my xmax, mark);
			my points. addItem_move (textpoint.move());
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of TextTier not set.");
	}
}

void TextTier_setEarlierStartTime (TextTier me, double xmin, conststring32 mark) {
	try {
		if (xmin >= my xmin)
			return;
		if (mark) {
			autoTextPoint textpoint = TextPoint_create (my xmin, mark);
			my points. addItem_move (textpoint.move());
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of TextTier not set.");
	}
}

void TextGrid_setEarlierStartTime (TextGrid me, double xmin, conststring32 intervalMark, conststring32 pointMark) {
	try {
		if (xmin >= my xmin)
			return;
		for (integer tierNumber = 1 ; tierNumber <= my tiers->size; tierNumber ++) {
			const Function tier = my tiers->at [tierNumber];
			if (tier -> classInfo == classIntervalTier)
				IntervalTier_setEarlierStartTime ((IntervalTier) tier, xmin, intervalMark);
			else
				TextTier_setEarlierStartTime ((TextTier) tier, xmin, pointMark);
		}
		my xmin = xmin;
	} catch (MelderError) {
		Melder_throw (U"Earlier start time of TextGrid not set.");
	}
}

void TextGrid_setLaterEndTime (TextGrid me, double xmax, conststring32 intervalMark, conststring32 pointMark) {
	try {
		if (xmax <= my xmax)
			return;
		for (integer tierNumber = 1 ; tierNumber <= my tiers->size; tierNumber ++) {
			const Function tier = my tiers->at [tierNumber];
			if (tier -> classInfo == classIntervalTier)
				IntervalTier_setLaterEndTime ((IntervalTier) tier, xmax, intervalMark);
			else
				TextTier_setLaterEndTime ((TextTier) tier, xmax, pointMark);
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"Larger end time of TextGrid not set.");
	}
}

void TextGrid_extendTime (TextGrid me, double extra_time, int position) {
	autoTextGrid thee;
	try {
		double xmax = my xmax, xmin = my xmin;
		bool at_end = ( position == 0 );

		if (extra_time == 0.0)
			return;
		extra_time = fabs (extra_time);   // just in case
		thee = Data_copy (me);

		if (at_end)
			xmax += extra_time;
		else
			xmin -= extra_time;

		for (integer i = 1; i <= my tiers->size; i ++) {
			const Function anyTier = my tiers->at [i];
			double tmin = anyTier -> xmin, tmax = anyTier -> xmax;

			if (at_end) {
				anyTier -> xmax = xmax;
				tmin = tmax;
				tmax = xmax;
			} else {
				anyTier -> xmin = xmin;
				tmax = tmin;
				tmin = xmin;
			}
			if (anyTier -> classInfo == classIntervalTier) {
				const IntervalTier tier = (IntervalTier) anyTier;
				autoTextInterval interval = TextInterval_create (tmin, tmax, U"");
				tier -> intervals. addItem_move (interval.move());
			}
		}
		my xmin = xmin;
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (me, U": time not extended.");
	}
}

void TextGrid_setTierName (TextGrid me, integer itier, conststring32 newName) {
	try {
		Melder_require (itier >= 1 && itier <= my tiers->size,
			U"The tier number (", itier, U") should not be larger than the number of tiers (", my tiers->size, U").");
		Thing_setName (my tiers->at [itier], newName);
	} catch (MelderError) {
		Melder_throw (me, U": tier name not set.");
	}
}

static void IntervalTier_cutInterval (IntervalTier me, integer index, int extend_option) {
	integer size_pre = my intervals.size;
	/*
		There always should be at least one interval
	 */
	if (size_pre == 1 || index > size_pre || index < 1)
		return;

	TextInterval ti = my intervals.at [index];
	const double xmin = ti -> xmin;
	const double xmax = ti -> xmax;
	my intervals. removeItem (index);
	if (index == 1) {
		/*
			Change xmin of the new first interval.
		 */
		ti = my intervals.at [index];
		ti -> xmin = xmin;
	} else if (index == size_pre) { 
		/*
			Change xmax of the new last interval.
		*/
		ti = my intervals.at [my intervals.size];
		ti -> xmax = xmax;
	} else {
		if (extend_option == 0) { 
			/*
				Extend earlier interval to the right.
			*/
			ti = my intervals.at [index - 1];
			ti -> xmax = xmax;
		} else {
			/*
				Extend next interval to the left.
			*/
			ti = my intervals.at [index];
			ti -> xmin = xmin;
		}
	}
}

void IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (IntervalTier me, conststring32 label) {
	try {
		for (integer iinterval = my intervals.size; iinterval > 1; iinterval --) {
			const TextInterval thisInterval = my intervals.at [iinterval];
			if (Melder_equ (thisInterval -> text.get(), label)) {
				const TextInterval previousInterval = my intervals.at [iinterval - 1];
				if (Melder_equ (previousInterval -> text.get(), label)) {
					previousInterval -> text. reset();
					IntervalTier_removeLeftBoundary (me, iinterval);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": boundaries not removed.");
	}
}

void IntervalTier_cutIntervals_minimumDuration (IntervalTier me, conststring32 label, double minimumDuration) {
	integer iinterval = 1;
	while (iinterval <= my intervals.size) {
		const TextInterval interval = my intervals.at [iinterval];
		if ((! label || (interval -> text && str32equ (interval -> text.get(), label))) &&
			interval -> xmax - interval -> xmin < minimumDuration)
			IntervalTier_cutInterval (me, iinterval, 0);
		else
			iinterval ++;
	}
}

void IntervalTier_combineIntervalsOnLabelMatch (IntervalTier me, conststring32 label) {
	integer iinterval = 1;
	while (iinterval < my intervals.size) {
		const TextInterval thisInterval = my intervals.at [iinterval];
		const TextInterval nextInterval = my intervals.at [iinterval + 1];
		if ((! label || (thisInterval -> text && str32equ (thisInterval -> text.get(), label))) &&
			Melder_equ (thisInterval -> text.get(), nextInterval -> text.get()))
			IntervalTier_cutInterval (me, iinterval, 1);
		else
			iinterval ++;
	}
}

void IntervalTier_changeLabels (IntervalTier me, integer from, integer to,
	conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches) {
	try {
		if (from == 0)
			from = 1;
		if (to == 0)
			to = my intervals.size;
		Melder_require (to >= from && from >= 1 && to <= my intervals.size,
			U"Incorrect specification of where to act.");
		Melder_require (! (use_regexp && search [0] == U'\0'),
			U"The regex search string cannot be empty.\nYou may search for an empty string with the expression \"^$\"");
		const integer offset = from - 1, nlabels = to - offset;
		autovector <conststring32> labels = newvectorzero <conststring32> (nlabels);
		for (integer i = from; i <= to; i ++) {
			const TextInterval interval = my intervals.at [i];
			labels [i - offset] = interval -> text.get();   // shallow copy
		}
		autoSTRVEC newLabels = string32vector_searchAndReplace (
			constSTRVEC (& labels [1], nlabels),
			//labels.get(),
			search, replace, 0, nmatches, nstringmatches, use_regexp
		);
		for (integer i = from; i <= to; i ++) {
			const TextInterval interval = my intervals.at [i];
			interval -> text = newLabels [i - offset].move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

void TextTier_changeLabels (TextTier me, integer from, integer to,
	conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches)
{
	try {
		if (from == 0)
			from = 1;
		if (to == 0)
			to = my points.size;
		Melder_require (to >= from && from >= 1 && to <= my points.size,
			U"Incorrect specification of where to act.");
		Melder_require (! (use_regexp && search [0] == U'\0'),
			U"The regex search string cannot be empty.\nTo search for an empty string, use the expression \"^$\" instead.");
		const integer offset = from - 1, nmarks = to - offset;
		autovector <conststring32> marks = newvectorzero <conststring32> (nmarks);   // a non-owning vector of strings
		for (integer i = from; i <= to; i ++) {
			TextPoint point = my points.at [i];
			marks [i - offset] = point -> mark.get();   // reference copy
		}
		autoSTRVEC newMarks = string32vector_searchAndReplace (
				constSTRVEC (& marks [1], nmarks),
				search, replace, 0, nmatches, nstringmatches, use_regexp);
		for (integer i = from; i <= to; i ++) {
			TextPoint point = my points.at [i];
			point -> mark = newMarks [i - offset].move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": no labels changed.");
	}
}

static void IntervalTier_addInterval_force (IntervalTier me, double tmin, double tmax, conststring32 newLabel) {
	Melder_require (tmin >= my xmin && tmax <= my xmax,
		U"The interval should not be outside the domain.");
	Melder_require (tmin < tmax,
		U"The start time of the interval should be smaller than the end time.");
	const integer oldSize = my intervals.size;
	integer ileft = IntervalTier_timeToIndex (me, tmin);
	TextInterval leftInterval = my intervals .at [ileft];
	conststring32 leftText = leftInterval -> text.get();
	const double leftxmin = leftInterval -> xmin, leftxmax = leftInterval -> xmax;
	if (Melder_cmp (leftText, newLabel) != 0) {
		if (tmin > leftxmin) {
			autoTextInterval newInterval = TextInterval_create (tmin, leftxmax, leftText);
			leftInterval -> xmax = tmin;
			my intervals. addItem_move (newInterval.move());
			Melder_assert (my intervals.size == oldSize +1);
			ileft ++;
		} else if (tmax == leftxmax) { // tmin == xmin
			TextInterval_setText (leftInterval, newLabel);
			return;
		}
	}
	const integer iright = IntervalTier_timeToHighIndex (me, tmax);
	TextInterval rightInterval = my intervals .at [iright];
	conststring32 rightText = rightInterval -> text.get();
	const double rightxmin = rightInterval -> xmin, rightxmax = rightInterval -> xmax;
	if (Melder_cmp (rightText , newLabel) != 0) {
		if (tmax < rightxmax) {
			autoTextInterval newInterval = TextInterval_create (rightxmin, tmax, rightText);
			rightInterval -> xmin = tmax;
			my intervals. addItem_move (newInterval.move());
		}
	}
	for (integer ipos = ileft; ipos <= iright; ipos ++)
		TextInterval_setText (my intervals .at [ipos], newLabel);
	IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (me, newLabel);
}

void TextGrid_addInterval_force (TextGrid me, double tmin, double tmax, integer tierNumber, conststring32 newLabel) {
	IntervalTier intervalTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
	IntervalTier_addInterval_force (intervalTier, tmin, tmax, newLabel);
}

void TextGrid_changeLabels (TextGrid me, integer tier, integer from, integer to,
	conststring32 search, conststring32 replace, bool use_regexp, integer *nmatches, integer *nstringmatches)
{
	try {
		const integer ntiers = my tiers->size;
		Melder_require (tier > 0 && tier <= ntiers,
			U"The tier number (", tier, U") should not be larger than the number of tiers (", ntiers, U").");
		Melder_require (! (use_regexp && search [0] == U'\0'), 
			U"The regex search string should not be empty.\nTo search for an empty string, use the expression \"^$\"");

		const Function anyTier = my tiers->at [tier];
		if (anyTier -> classInfo == classIntervalTier)
			IntervalTier_changeLabels ((IntervalTier) anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
		else
			TextTier_changeLabels ((TextTier) anyTier, from, to, search, replace, use_regexp, nmatches, nstringmatches);
	} catch (MelderError) {
		Melder_throw (me, U": labels not changed.");
	}
}

static void IntervalTier_checkStartAndEndTime (IntervalTier me) {
	Melder_assert (my intervals.size > 0);
	const TextInterval first = my intervals.at [1];
	Melder_require (my xmin == first -> xmin,
		U": start time of first interval doesn't match start time of the tier.");
	const TextInterval last = my intervals.at [my intervals.size];
	Melder_require (my xmax == last -> xmax,
		U": end time of last interval doesn't match end time of the tier.");
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
// Postcondition: my xmin preserved
void IntervalTiers_append_inplace (IntervalTier me, IntervalTier thee, bool preserveTimes) {
	try {
		IntervalTier_checkStartAndEndTime (me); // start/end time of first/last interval should match with tier
		IntervalTier_checkStartAndEndTime (thee);
		const double time_shift = my xmax - thy xmin;
		double xmax_previous = my xmax;
		if (preserveTimes && my xmax < thy xmin) {
			autoTextInterval connection = TextInterval_create (my xmax, thy xmin, U"");
			xmax_previous = thy xmin;
			my intervals. addItem_move (connection.move());
		}
		for (integer iint = 1; iint <= thy intervals.size; iint ++) {
			autoTextInterval ti = Data_copy (thy intervals.at [iint]);
			if (preserveTimes) {
				my intervals. addItem_move (ti.move());
			} else {
				/*
					The interval could be so short that if we test ti -> xmin < ti->xmax it might be true
					but after assigning ti->xmin = xmax_previous and ti->xmax += time_shift the test
					ti -> xmin < ti->xmax might be false!
					We want to make sure xmin and xmax are not register variables and therefore force
					double64 by using volatile variables.
				*/
				volatile double xmin = xmax_previous;
				volatile double xmax = ti -> xmax + time_shift;
				if (xmin < xmax) {
					ti -> xmin = xmin;
					ti -> xmax = xmax;
					my intervals. addItem_move (ti.move());
					xmax_previous = xmax;
				}
				/*
					Else don't include interval.
				*/
			}
		}
		my xmax = preserveTimes ? thy xmax : xmax_previous;
	} catch (MelderError) {
		Melder_throw (U"IntervalTiers not appended.");
	}
}

// Precondition: if (preserveTimes) { my xmax <= thy xmin }
void TextTiers_append_inplace (TextTier me, TextTier thee, bool preserveTimes) {
	try {
		for (integer iint = 1; iint <= thy points.size; iint ++) {
			autoTextPoint tp = Data_copy (thy points.at [iint]);
			if (! preserveTimes)
				tp -> number += my xmax - thy xmin;
			my points. addItem_move (tp.move());
		}
		my xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
	} catch (MelderError) {
		Melder_throw (U"TextTiers not appended.");
	}
}

static void TextGrid_checkStartAndEndTimesOfTiers (TextGrid me) {
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		const Function tier = my tiers->at [itier];
		Melder_require (tier -> xmin == my xmin,
			U": the start time of tier ", itier, U" does not match the start time of its TextGrid.");
		Melder_require (tier -> xmax == my xmax,
			U": the end time of tier ", itier, U" does not match the end time of its TextGrid.");
	}
}

void TextGrids_append_inplace (TextGrid me, TextGrid thee, bool preserveTimes)
{
	try {
		Melder_require (my tiers->size == thy tiers->size,
			U"The numbers of tiers should be equal.");
		Melder_require (! (preserveTimes && thy xmin < my xmax),
			U"The start time of the second TextGrid can't be earlier than the end time of the first one if you want to preserve times.");
		TextGrid_checkStartAndEndTimesOfTiers (me); // all tiers must have the same start/end time as textgrid
		TextGrid_checkStartAndEndTimesOfTiers (thee);
		/*
			Last intervals must have the same end time.
		*/
		const double xmax = preserveTimes ? thy xmax : my xmax + (thy xmax - thy xmin);
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			const Function myTier = my tiers->at [itier], thyTier = thy tiers->at [itier];
			if (myTier -> classInfo == classIntervalTier && thyTier -> classInfo == classIntervalTier) {
				const IntervalTier  myIntervalTier = static_cast <IntervalTier>  (myTier);
				const IntervalTier thyIntervalTier = static_cast <IntervalTier> (thyTier);
				IntervalTiers_append_inplace (myIntervalTier, thyIntervalTier, preserveTimes);
				/*
					Because of floating-point rounding errors, we explicitly make sure that
					both the xmax of the tier and the xmax of the last interval equal the xmax of the grid.
				*/
				myIntervalTier -> xmax = xmax;
				const TextInterval lastInterval = myIntervalTier -> intervals.at [myIntervalTier -> intervals.size];
				lastInterval -> xmax = xmax;
				Melder_assert (lastInterval -> xmax > lastInterval -> xmin);
			} else if (myTier -> classInfo == classTextTier && thyTier -> classInfo == classTextTier) {
				const TextTier  myTextTier = static_cast <TextTier>  (myTier);
				const TextTier thyTextTier = static_cast <TextTier> (thyTier);
				TextTiers_append_inplace (myTextTier, thyTextTier, preserveTimes);
				myTextTier -> xmax = xmax;
			} else {
				Melder_throw (U"Tier ", itier, U" in the second TextGrid is of a different type "
					"than tier ", itier, U" in the first TextGrid.");
			}
		}
		my xmax = xmax;
	} catch (MelderError) {
		Melder_throw (U"TextGrids not appended.");
	}
}

autoTextGrid TextGrids_to_TextGrid_appendContinuous (OrderedOf<structTextGrid>* me, bool preserveTimes) {
	try {
		Melder_assert (my size > 0);
		autoTextGrid thee = Data_copy (my at [1]);
		for (integer igrid = 2; igrid <= my size; igrid ++)
			TextGrids_append_inplace (thee.get(), my at [igrid], preserveTimes);
		if (! preserveTimes)
			Function_shiftXBy (thee.get(), -thy xmin);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No aligned TextGrid created from Collection.");
	}
}

double TextGrid_getTotalDurationOfIntervalsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion) {
	try {
		longdouble totalDuration = 0.0;
		const IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		for (integer iinterval = 1; iinterval <= tier -> intervals.size; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			if (Melder_stringMatchesCriterion (interval -> text.get(), which, criterion, true))
				totalDuration += interval -> xmax - interval -> xmin;
		}
		return double (totalDuration);
	} catch (MelderError) {
		Melder_throw (me, U": interval durations not counted.");
	}
}

/* End of file TextGrid_extensions.cpp */
