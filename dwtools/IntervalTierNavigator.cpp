/* IntervalTierNavigator.cpp
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

#include "IntervalTierNavigator.h"
#include "NUM2.h"


#include "enums_getText.h"
#include "IntervalTierNavigator_enums.h"
#include "enums_getValue.h"
#include "IntervalTierNavigator_enums.h"

Thing_implement (IntervalTierNavigator, Function, 0);

void structIntervalTierNavigator :: v_info () {
	MelderInfo_writeLine (U"Navigation:");
	if (topicLabels) {
		MelderInfo_writeLine (U"\tName: ", topicLabels -> name.get());
		MelderInfo_writeLine (U"\tNumber of labels: ", topicLabels -> strings.size);
		if (beforeLabels) {
			MelderInfo_writeLine (U"\tLeft context name: ", beforeLabels -> name.get());
			MelderInfo_writeLine (U"\tLeft criterion: ", kMelder_string_getText (beforeCriterion));
			MelderInfo_writeLine (U"\tNumber of left context labels: ", beforeLabels -> strings.size);
		} else {
			MelderInfo_writeLine (U"\tNo left context navigation labels defined");
		}
		if (afterLabels) {
			MelderInfo_writeLine (U"\tRight context name: ", afterLabels -> name.get());
			MelderInfo_writeLine (U"\tRight criterion: ", kMelder_string_getText (afterCriterion));
			MelderInfo_writeLine (U"\tNumber of right context labels: ", afterLabels -> strings.size);
		} else {
			MelderInfo_writeLine (U"\tNo right context navigation labels defined");
		}
		MelderInfo_writeLine (U"\tMatch context: ", kContextCombination_getText (contextCombination));
		MelderInfo_writeLine (U"\tMatch context only: ", ( matchContextOnly ? U"yes" : U"no" ));
		MelderInfo_writeLine (U"\tNumber of interval matches: ", IntervalTierNavigator_getNumberOfMatches (this), U" out  of ", intervalTier -> intervals . size);
	} else {
		MelderInfo_writeLine (U"\tNo navigation labels defined");
	}
}

autoIntervalTierNavigator IntervalTierNavigator_createFromTextGrid (TextGrid me, integer navigationTier) {
	try {
		TextGrid_checkSpecifiedTierIsIntervalTier (me, navigationTier);
		IntervalTier intervalTier =  static_cast<IntervalTier> (my tiers -> at [navigationTier]);
		autoIntervalTierNavigator thee = IntervalTierNavigator_create (intervalTier);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"IntervalTierNavigator not created from ", me, U".");
	}
}

autoIntervalTierNavigator IntervalTierNavigator_create (IntervalTier me) {
	try {
		autoIntervalTierNavigator thee = Thing_new (IntervalTierNavigator);
		Function_init (thee.get(), my xmin, my xmax);
		thy intervalTier = me;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"IntervalTierNavigator not created.");
	}
}

autoIntervalTierNavigator IntervalTierNavigator_createEmpty (IntervalTier me) {
	autoIntervalTierNavigator thee = Thing_new (IntervalTierNavigator);
	Function_init (thee.get(), my xmin, my xmax);
	thy intervalTier = me;
	return thee;
}


void IntervalTierNavigator_setNavigationLabels (IntervalTierNavigator me, Strings topicLabels, kMelder_string criterion) {
	try {
		my topicLabels = Data_copy (topicLabels);
		Thing_setName (my topicLabels.get(), topicLabels -> name.get());
		my topicCriterion = criterion;
		my contextCombination = kContextCombination::NO_BEFORE_AND_NO_AFTER;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set navigation labels from ", topicLabels, U".");
	}
}

void IntervalTierNavigator_setLeftContextNavigationLabels (IntervalTierNavigator me, Strings beforeLabels, kMelder_string criterion) {
	try {
		my beforeLabels = Data_copy (beforeLabels);
		Thing_setName (my beforeLabels.get(), beforeLabels -> name.get());
		my beforeCriterion = criterion;
		my contextCombination = kContextCombination::BEFORE;
		my lookBackFrom = 1;
		my lookBackTo = 1;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set left context labels from ", beforeLabels, U".");
	}
}

void IntervalTierNavigator_setRightContextNavigationLabels (IntervalTierNavigator me, Strings afterLabels, kMelder_string criterion) {
	try {
		my afterLabels = Data_copy (afterLabels);
		Thing_setName (my afterLabels.get(), afterLabels -> name.get());
		my afterCriterion = criterion;
		my contextCombination = kContextCombination::AFTER;
		my lookForwardFrom = 1;
		my lookForwardTo = 1;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set right context labels from ", afterLabels, U".");
	}
}

void IntervalTierNavigator_setNavigationContext (IntervalTierNavigator me, integer lookBackFrom, integer lookBackTo, integer lookForwardFrom, integer lookForwardTo, kContextCombination contextCombination, bool matchContextOnly) {
	bool hasLeftContext = ( my beforeLabels && my beforeLabels -> strings.size > 0 );
	bool hasRightContext = ( my afterLabels && my afterLabels -> strings.size > 0 );
	if (contextCombination == kContextCombination::BEFORE)
		Melder_require (hasLeftContext,
			U"For this option you should have left context labels installed.");
	if (contextCombination == kContextCombination::AFTER)
		Melder_require (hasRightContext,
			U"For this option you should have right context labels installed.");
	if (contextCombination == kContextCombination::BEFORE_AND_AFTER || contextCombination == kContextCombination::BEFORE_OR_AFTER_NOT_BOTH || 
		contextCombination == kContextCombination::BEFORE_OR_AFTER_OR_BOTH)
		Melder_require (hasLeftContext && hasRightContext,
			U"For this option you should have left and right context labels installed.");
	if (matchContextOnly)
		Melder_require (hasLeftContext || hasRightContext,
			U"It is not possible to match only the context because you have neither left nor right context labels installed.");
	Melder_require (lookBackFrom > 0 &&  lookBackTo > 0,
		U"The left context interval distance should be positive.");
	Melder_require (lookForwardFrom > 0 && lookForwardTo > 0,
		U"The right context interval distance should be positive.");
	my matchContextOnly = matchContextOnly;
	my contextCombination = contextCombination;
	my lookBackFrom = std::min (lookBackFrom, lookBackTo);
	my lookBackTo = std::max (lookBackFrom, lookBackTo);
	my lookForwardFrom = std::min (lookForwardFrom, lookForwardTo);
	my lookForwardTo = std::max (lookForwardFrom, lookForwardTo);
}

/*
	1. To determine if an item is in a set, we can simply start to test whether it is equal to the first element
		if so we are done. If not check the second element etc until we get a match (or not).
	2. To determine whether an item is not in the set, we need to reverse the test until it fails.
*/
bool STRVEChasMatch (constSTRVEC const& labels, kMelder_string criterion, conststring32 label) {
	if (criterion == kMelder_string :: EQUAL_TO                   || criterion == kMelder_string :: CONTAINS ||
		criterion == kMelder_string :: STARTS_WITH                || criterion == kMelder_string :: ENDS_WITH ||
		criterion == kMelder_string :: CONTAINS_WORD              || criterion == kMelder_string :: CONTAINS_WORD_STARTING_WITH || 
		criterion == kMelder_string :: CONTAINS_WORD_ENDING_WITH  || criterion == kMelder_string :: CONTAINS_INK ||
		criterion == kMelder_string :: CONTAINS_INK_STARTING_WITH || criterion == kMelder_string :: CONTAINS_INK_ENDING_WITH) {
		for (integer istring = 1; istring <= labels.size; istring ++)
			if (Melder_stringMatchesCriterion (label, criterion, labels [istring], true))
				return true;
	} else {
		for (integer istring = 1; istring <= labels.size; istring ++)
			if (! Melder_stringMatchesCriterion (label, criterion, labels [istring], true))
				return false;
		return true;
	}
	return false;
}

static bool IntervalTierNavigator_isNavigationMatch (IntervalTierNavigator me, integer intervalNumber) {
	conststring32 label = my intervalTier -> intervals . at [intervalNumber] -> text.get();
	return ( my topicLabels && STRVEChasMatch (my topicLabels -> strings.get(), my topicCriterion, label) );
}

static bool IntervalTierNavigator_isLeftContextMatch (IntervalTierNavigator me, integer intervalNumber) {
	if (! my beforeLabels)
		return false;
	if (intervalNumber - my lookBackFrom < 1)
		return false;
	integer startInterval = std::max (1_integer, intervalNumber - my lookBackFrom);
	integer endInterval = std::max (1_integer, intervalNumber - my lookBackTo);
	for (integer interval = startInterval; interval >= endInterval; interval --) {
		conststring32 label = my intervalTier -> intervals . at [interval] -> text.get();
		if (STRVEChasMatch (my beforeLabels -> strings.get(), my beforeCriterion, label))
			return true;
	}
	return false;
}

static bool IntervalTierNavigator_isRightContextMatch (IntervalTierNavigator me, integer intervalNumber) {
	if (! my afterLabels)
		return false;
	if (intervalNumber + my lookForwardFrom > my intervalTier -> intervals.size)
		return false;
	const integer startInterval = std::min (my intervalTier -> intervals.size, intervalNumber + my lookForwardFrom);
	const integer endInterval = std::min (my intervalTier -> intervals.size, intervalNumber + my lookForwardTo);
	for (integer interval = startInterval; interval <= endInterval; interval ++) {
		conststring32 label = my intervalTier -> intervals . at [interval] -> text.get();
		if (STRVEChasMatch (my afterLabels -> strings.get(), my afterCriterion, label))
			return true;
	}
	return false;
}

bool IntervalTierNavigator_isLabelMatch (IntervalTierNavigator me, integer intervalNumber) {
	Melder_require (intervalNumber > 0 && intervalNumber <= my intervalTier -> intervals . size,
		U"The interval number should be in the range from 1 to ",  my intervalTier -> intervals . size, U".");
	const bool isNavigationMatch = ( my matchContextOnly ? true : IntervalTierNavigator_isNavigationMatch (me, intervalNumber) );
	if (! isNavigationMatch || my contextCombination == kContextCombination::NO_BEFORE_AND_NO_AFTER)
		return isNavigationMatch;
	
	if (my contextCombination == kContextCombination::BEFORE_AND_AFTER)
		return ( IntervalTierNavigator_isLeftContextMatch (me, intervalNumber) &&
			IntervalTierNavigator_isRightContextMatch (me, intervalNumber) );
	else if (my contextCombination == kContextCombination::AFTER)
		return IntervalTierNavigator_isRightContextMatch (me, intervalNumber);
	else if (my contextCombination == kContextCombination::BEFORE)
		return IntervalTierNavigator_isLeftContextMatch (me, intervalNumber);
	else if (my contextCombination == kContextCombination::BEFORE_OR_AFTER_OR_BOTH)
		return ( IntervalTierNavigator_isLeftContextMatch (me, intervalNumber) ||
			IntervalTierNavigator_isRightContextMatch (me, intervalNumber) );
	else if (my contextCombination == kContextCombination::BEFORE_OR_AFTER_NOT_BOTH)
		return ( IntervalTierNavigator_isLeftContextMatch (me, intervalNumber)  == ! IntervalTierNavigator_isRightContextMatch (me, intervalNumber) );
	return false;
}

integer IntervalTierNavigator_getNumberOfMatches (IntervalTierNavigator me) {
	if (! my topicLabels)
		return 0;
	integer numberOfMatches = 0;
	for (integer interval = 1; interval <= my intervalTier -> intervals . size; interval ++)
		if (IntervalTierNavigator_isLabelMatch (me, interval))
			numberOfMatches ++;
	return numberOfMatches;
}

/*
	return 0 if time < my xmin
	return intervals .size + 1 if time > my xmax
*/
integer IntervalTierNavigator_getNavigationStartInterval (IntervalTierNavigator me, double time) {
	integer intervalNumber = IntervalTier_timeToIndex (my intervalTier, time);
	if (intervalNumber == 0) {
		if (time < my xmin) {
			intervalNumber = 0; // start
		} else if (time > my xmax)
			intervalNumber = my intervalTier -> intervals .size + 1; // end
	}
	return intervalNumber;
}

integer IntervalTierNavigator_getNextMatchingIntervalNumberFromNumber (IntervalTierNavigator me, integer intervalNumber) {
	if (! my topicLabels)
		return 0;
	const integer startInterval = std::min (std::max (0_integer, intervalNumber), my intervalTier -> intervals .size + 1);
	for (integer interval = startInterval + 1; interval <= my intervalTier -> intervals . size; interval ++)
		if (IntervalTierNavigator_isLabelMatch (me, interval))
			return interval;
	return 0;
}

integer IntervalTierNavigator_getNextMatchingIntervalNumberFromTime (IntervalTierNavigator me, double time) {
	if (! my topicLabels)
		return 0;
	integer startInterval = IntervalTierNavigator_getNavigationStartInterval (me, time);
	return IntervalTierNavigator_getNextMatchingIntervalNumberFromNumber (me, startInterval);
}


TextInterval IntervalTierNavigator_getNextMatchingInterval (IntervalTierNavigator me, double time) {
	const integer interval = IntervalTierNavigator_getNextMatchingIntervalNumberFromTime (me, time);
	if (interval == 0)
		return nullptr;
	return my intervalTier -> intervals . at [interval];
}

integer IntervalTierNavigator_getPreviousMatchingIntervalNumberFromNumber (IntervalTierNavigator me, integer intervalNumber) {
	if (! my topicLabels)
		return 0;
	const integer startInterval = std::min (std::max (0_integer, intervalNumber), my intervalTier -> intervals .size + 1);
	for (integer interval = startInterval - 1; interval > 0; interval --)
		if (IntervalTierNavigator_isLabelMatch (me, interval))
			return interval;
	return 0;
}

integer IntervalTierNavigator_getPreviousMatchingIntervalNumberFromTime (IntervalTierNavigator me, double time) {
	if (! my topicLabels)
		return 0;
	integer startInterval = IntervalTierNavigator_getNavigationStartInterval (me, time);
	return IntervalTierNavigator_getPreviousMatchingIntervalNumberFromNumber (me, startInterval);
}

TextInterval 	IntervalTierNavigator_getPreviousMatchingInterval (IntervalTierNavigator me, double time) {
	const integer interval = IntervalTierNavigator_getPreviousMatchingIntervalNumberFromTime (me, time);
	if (interval == 0)
		return nullptr;
	return my intervalTier -> intervals . at [interval];
}

bool IntervalTierNavigator_atMatchingEnd (IntervalTierNavigator me, double time) {
	return ( IntervalTierNavigator_getNextMatchingIntervalNumberFromTime (me, time) == 0 );
}

bool IntervalTierNavigator_atMatchingStart (IntervalTierNavigator me, double time) {
	return ( IntervalTierNavigator_getPreviousMatchingIntervalNumberFromTime (me, time) == 0 );
}

/* End of file IntervalTierNavigator.cpp */
