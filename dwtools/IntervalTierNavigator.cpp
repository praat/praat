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
	if (navigationLabels) {
		MelderInfo_writeLine (U"\tName: ", navigationLabels -> name.get());
		MelderInfo_writeLine (U"\tNumber of labels: ", navigationLabels -> strings.size);
		if (leftContextLabels) {
			MelderInfo_writeLine (U"\tLeft context name: ", leftContextLabels -> name.get());
			MelderInfo_writeLine (U"\tLeft criterion: ", kMelder_string_getText (leftContextCriterion));
			MelderInfo_writeLine (U"\tNumber of labels: ", leftContextLabels -> strings.size);
		}
		if (rightContextLabels) {
			MelderInfo_writeLine (U"\tRight context name: ", rightContextLabels -> name.get());
			MelderInfo_writeLine (U"\tRight criterion: ", kMelder_string_getText (rightContextCriterion));
			MelderInfo_writeLine (U"\tNumber of labels: ", rightContextLabels -> strings.size);
		}
		MelderInfo_writeLine (U"\tContext use: ", kContextUse_getText (contextUse));
		MelderInfo_writeLine (U"\tMatches: ", IntervalTierNavigator_getNumberOfMatches (this), U" out  of ", intervalTier -> intervals . size);
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


void IntervalTierNavigator_setNavigationLabels (IntervalTierNavigator me, Strings navigationLabels, kMelder_string criterion) {
	try {
		my navigationLabels = Data_copy (navigationLabels);
		Thing_setName (my navigationLabels.get(), navigationLabels -> name.get());
		my criterion = criterion;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set navigation labels from ", navigationLabels, U".");
	}
}

void IntervalTierNavigator_setLeftContextNavigationLabels (IntervalTierNavigator me, Strings leftContextLabels, kMelder_string criterion) {
	try {
		my leftContextLabels = Data_copy (leftContextLabels);
		Thing_setName (my leftContextLabels.get(), leftContextLabels -> name.get());
		my leftContextCriterion = criterion;
		my contextUse = kContextUse::LEFT;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set left context labels from ", leftContextLabels, U".");
	}
}

void IntervalTierNavigator_setRightContextNavigationLabels (IntervalTierNavigator me, Strings rightContextLabels, kMelder_string criterion) {
	try {
		my rightContextLabels = Data_copy (rightContextLabels);
		Thing_setName (my rightContextLabels.get(), rightContextLabels -> name.get());
		my rightContextCriterion = criterion;
		my contextUse = kContextUse::RIGHT;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set right context labels from ", rightContextLabels, U".");
	}
}

void IntervalTierNavigator_setNavigationContextUse (IntervalTierNavigator me, kContextUse contextUse) {
	bool hasLeftContext = ( my leftContextLabels && my leftContextLabels -> strings.size > 0 );
	bool hasRightContext = ( my rightContextLabels && my rightContextLabels -> strings.size > 0 );
	if (contextUse == kContextUse::LEFT)
		Melder_require (hasLeftContext,
			U"For this option you should have a left context installed.");
	if (contextUse == kContextUse::RIGHT)
		Melder_require (hasRightContext,
			U"For this option you should have a right context installed.");
	if (contextUse == kContextUse::LEFT_AND_RIGHT || contextUse == kContextUse::LEFT_OR_RIGHT_NOT_BOTH || 
		contextUse == kContextUse::LEFT_OR_RIGHT_OR_BOTH)
		Melder_require (hasLeftContext && hasRightContext,
			U"For this option you should have a left and a right context installed.");
	my contextUse = contextUse;
}

bool STRVEChasMatch (constSTRVEC const& labels, kMelder_string criterion, conststring32 label) {
	for (integer istring = 1; istring <= labels.size; istring ++)
			if (Melder_stringMatchesCriterion (label, criterion, labels [istring], true))
				return true;
	return false;
}

static bool IntervalTierNavigator_isNavigationMatch (IntervalTierNavigator me, integer intervalNumber) {
	conststring32 label = my intervalTier -> intervals . at [intervalNumber] -> text.get();
	return ( my navigationLabels && STRVEChasMatch (my navigationLabels -> strings.get(), my criterion, label) );
}

static bool IntervalTierNavigator_isLeftContextMatch (IntervalTierNavigator me, integer intervalNumber) {
	conststring32 label = my intervalTier -> intervals . at [intervalNumber] -> text.get();
	return ( my leftContextLabels && STRVEChasMatch (my leftContextLabels -> strings.get(), my leftContextCriterion, label) );
}

static bool IntervalTierNavigator_isRightContextMatch (IntervalTierNavigator me, integer intervalNumber) {
	conststring32 label = my intervalTier -> intervals . at [intervalNumber] -> text.get();
	return ( my rightContextLabels && STRVEChasMatch (my rightContextLabels -> strings.get(), my rightContextCriterion, label) );
}

bool IntervalTierNavigator_isLabelMatch (IntervalTierNavigator me, integer intervalNumber) {
	Melder_require (intervalNumber > 0 && intervalNumber <= my intervalTier -> intervals . size,
		U"The interval number should be in the range from 1 to ",  my intervalTier -> intervals . size, U".");
	const bool isNavigationMatch = IntervalTierNavigator_isNavigationMatch (me, intervalNumber);
	if (! isNavigationMatch || my contextUse == kContextUse::NO_LEFT_AND_NO_RIGHT)
		return isNavigationMatch;
	
	bool leftContextMatch = false, rightContextMatch = false;
	if (my contextUse == kContextUse::LEFT_AND_RIGHT) {
		if (intervalNumber == 1 || intervalNumber == my intervalTier -> intervals . size)
			return false;
		leftContextMatch = IntervalTierNavigator_isLeftContextMatch (me, intervalNumber - 1);
		rightContextMatch = IntervalTierNavigator_isRightContextMatch (me, intervalNumber + 1);
		return leftContextMatch && rightContextMatch;
	} else if (my contextUse == kContextUse::RIGHT) {
		if (intervalNumber == my intervalTier -> intervals . size)
			return false;
		rightContextMatch = IntervalTierNavigator_isRightContextMatch (me, intervalNumber + 1);
		return rightContextMatch;
	} else if (my contextUse == kContextUse::LEFT) {
		if (intervalNumber == 1)
			return false;
		leftContextMatch = IntervalTierNavigator_isLeftContextMatch (me, intervalNumber - 1);
		return leftContextMatch;
	} else if (my contextUse == kContextUse::LEFT_OR_RIGHT_OR_BOTH) {
		if (intervalNumber > 1)
			leftContextMatch = IntervalTierNavigator_isLeftContextMatch (me, intervalNumber - 1);
		if (leftContextMatch)
			return true;
		if (intervalNumber < my intervalTier -> intervals . size)
			rightContextMatch = IntervalTierNavigator_isRightContextMatch (me, intervalNumber + 1);
		return rightContextMatch;
	} else if (my contextUse == kContextUse::LEFT_OR_RIGHT_NOT_BOTH) {
		if (intervalNumber > 1)
			leftContextMatch = IntervalTierNavigator_isLeftContextMatch (me, intervalNumber - 1);
		if (intervalNumber < my intervalTier -> intervals . size)
			rightContextMatch = IntervalTierNavigator_isRightContextMatch (me, intervalNumber + 1);
		return ( leftContextMatch  == ! rightContextMatch);
	}
}

integer IntervalTierNavigator_getNumberOfMatches (IntervalTierNavigator me) {
	if (! my navigationLabels)
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

integer IntervalTierNavigator_getNextMatchingIntervalNumber (IntervalTierNavigator me, double time) {
	if (! my navigationLabels)
		return 0;
	integer startInterval = IntervalTierNavigator_getNavigationStartInterval (me, time);
	for (integer interval = startInterval + 1; interval <= my intervalTier -> intervals . size; interval ++)
		if (IntervalTierNavigator_isLabelMatch (me, interval))
			return interval;
	return 0;
}

TextInterval IntervalTierNavigator_getNextMatchingInterval (IntervalTierNavigator me, double time) {
	const integer interval = IntervalTierNavigator_getNextMatchingIntervalNumber (me, time);
	if (interval == 0)
		return nullptr;
	return my intervalTier -> intervals . at [interval];
}

integer IntervalTierNavigator_getPreviousMatchingIntervalNumber (IntervalTierNavigator me, double time) {
	if (! my navigationLabels)
		return 0;
	integer startInterval = IntervalTierNavigator_getNavigationStartInterval (me, time);
	for (integer interval = startInterval - 1; interval > 0; interval --)
		if (IntervalTierNavigator_isLabelMatch (me, interval))
			return interval;
	return 0;
}

TextInterval IntervalTierNavigator_getPreviousMatchingInterval (IntervalTierNavigator me, double time) {
	const integer interval = IntervalTierNavigator_getPreviousMatchingIntervalNumber (me, time);
	if (interval == 0)
		return nullptr;
	return my intervalTier -> intervals . at [interval];
}

bool IntervalTierNavigator_atMatchingEnd (IntervalTierNavigator me, double time) {
	return ( IntervalTierNavigator_getNextMatchingIntervalNumber (me, time) == 0 );
}

bool IntervalTierNavigator_atMatchingStart (IntervalTierNavigator me, double time) {
	return ( IntervalTierNavigator_getPreviousMatchingIntervalNumber (me, time) == 0 );
}

/* End of file IntervalTierNavigator.cpp */
