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


Thing_implement (IntervalTierNavigator, Function, 0);

void structIntervalTierNavigator :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:", xmin, U" to ", xmax, U" seconds");
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


void IntervalTierNavigator_setBeginPosition (IntervalTierNavigator me, double time) {
	integer intervalNumber = IntervalTier_timeToIndex (my intervalTier, time);
	if (intervalNumber == 0) {
		if (time <= my xmin)
			intervalNumber = 0; // offLeft
		else if (time > my xmax)
			intervalNumber = my intervalTier -> intervals .size + 1; // offRight
	}
	my currentIntervalNumber = intervalNumber;

}

void IntervalTierNavigator_setNavigationLabels (IntervalTierNavigator me, Strings navigationLabels, kMelder_string criterion) {
	try {
		my navigationLabels = Data_copy (navigationLabels);
		my criterion = criterion;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set navigation labels from ", navigationLabels, U".");
	}
}

bool IntervalTierNavigator_isLabelMatch (IntervalTierNavigator me, conststring32 label) {
	if (my navigationLabels) {
		for (integer istring = 1; istring <= my navigationLabels -> numberOfStrings; istring ++)
			if (Melder_stringMatchesCriterion (label, my criterion, my navigationLabels -> strings [istring].get(), true))
				return true;
	}
	return false;
}

integer IntervalTierNavigator_getNumberOfMatches (IntervalTierNavigator me) {
	if (! my navigationLabels)
		return 0;
	integer numberOfMatches = 0;
	for (integer interval = 1; interval <= my intervalTier -> intervals . size; interval ++)
		if (IntervalTierNavigator_isLabelMatch (me, my intervalTier -> intervals.at[interval] -> text.get()))
			numberOfMatches ++;
	return numberOfMatches;
}

integer IntervalTierNavigator_nextIntervalNumber (IntervalTierNavigator me) {
	Melder_require (! my offRight (),
		U"The current position is already past the end of the tier.");
	if (my navigationLabels) {
		for (integer interval =  my currentIntervalNumber + 1; interval <= my intervalTier -> intervals . size; interval ++) {
			TextInterval textInterval = my intervalTier -> intervals . at [interval];
			if (IntervalTierNavigator_isLabelMatch (me, textInterval -> text.get())) {
				my currentIntervalNumber = interval;
				return interval;
			}
		}
		my currentIntervalNumber = my intervalTier -> intervals . size + 1; // offRight
		return 0;
	}
	my currentIntervalNumber ++;
	return ( my currentIntervalNumber > my intervalTier -> intervals . size ? 0 : my currentIntervalNumber );
}

TextInterval IntervalTierNavigator_nextInterval (IntervalTierNavigator me) {
	const integer interval = IntervalTierNavigator_nextIntervalNumber (me);
	if (interval == 0)
		return nullptr;
	return my intervalTier -> intervals . at [interval];
}

integer IntervalTierNavigator_previousIntervalNumber (IntervalTierNavigator me) {
	Melder_require (! my offLeft (),
		U"The current position is already before the start of the tier.");
	if (my navigationLabels) {
		for (integer interval = my currentIntervalNumber - 1; interval > 0; interval --) {
			TextInterval textInterval = my intervalTier -> intervals . at [interval];
			if (IntervalTierNavigator_isLabelMatch (me, textInterval -> text.get())) {
				my currentIntervalNumber = interval;
				return interval;
			}
		}
		my currentIntervalNumber = 0; // offLeft
		return 0;
	}
	my currentIntervalNumber --; // always 0 or <= size
	return my currentIntervalNumber;
}

TextInterval IntervalTierNavigator_previousInterval (IntervalTierNavigator me) {
	const integer interval = IntervalTierNavigator_previousIntervalNumber (me);
	if (interval == 0)
		return nullptr;
	return my intervalTier -> intervals . at [interval];
}

/* End of file IntervalTierNavigator.cpp */
