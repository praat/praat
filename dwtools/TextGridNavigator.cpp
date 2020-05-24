/* TextGridNavigator.cpp
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

#include "TextGridNavigator.h"
#include "NUM2.h"


Thing_implement (TextGridNavigator, TextGrid, 0);

void structTextGridNavigator :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:", xmin, U" to ", xmax, U" seconds");
}

autoTextGridNavigator TextGridNavigator_create (TextGrid me, Strings navigationLabels, kMelder_string criterion) {
	try {
		autoTextGridNavigator thee = Thing_new (TextGridNavigator);
		Function_init (thee.get(), my xmin, my xmax);
		thy textGrid = me;
		thy navigationLabels = Data_copy (navigationLabels);
		thy criterion = criterion;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGridNavigator not created.");
	}
}

autoTextGridNavigator TextGridNavigator_createEmpty (TextGrid me) {
	autoTextGridNavigator thee = Thing_new (TextGridNavigator);
	Function_init (thee.get(), my xmin, my xmax);
	thy textGrid = me;
	return thee;
}

void TextGridNavigdNavigator_setNavigatorLabels (TextGridNavigator me, Strings navigationLabels, kMelder_string criterion) {
	try {
		my navigationLabels = Data_copy (navigationLabels);
		my criterion = criterion;
	} catch (MelderError) {
		Melder_throw (me, U": cannot set navigation labels from ", navigationLabels, U".");
	}
}

bool TextGridNavigator_hasLabelMatch (TextGridNavigator me, conststring32 label) {
	if (my navigationLabels) {
		for (integer istring = 1; istring <= my navigationLabels -> numberOfStrings; istring ++)
			if (Melder_stringMatchesCriterion (label, my criterion, label, true))
				return true;
	}
	return false;
}

integer TextGridNavigator_nextNavigationIntervalNumber (TextGridNavigator me, double time, integer navigationTier) {
	TextGrid_checkSpecifiedTierIsIntervalTier (my textGrid, navigationTier);
	IntervalTier intervalTier =  static_cast<IntervalTier> (my textGrid -> tiers -> at [navigationTier]);
	integer index = IntervalTier_timeToIndex (intervalTier, time);
	if (index == 0 && time >= my xmax) // offright
		return 0;
	if (index == intervalTier -> intervals . size)
		return 0;
	if (my navigationLabels) {
		for (integer interval = index + 1; interval <= intervalTier -> intervals . size; interval ++) {
			TextInterval textInterval = intervalTier -> intervals . at [interval];
			if (TextGridNavigator_hasLabelMatch (me, textInterval -> text.get()))
				return interval;
		}
		return 0;
	}
	index ++;
	return ( index > intervalTier -> intervals . size ? 0 : index );
}

integer TextGridNavigator_previousNavigationIntervalNumber (TextGridNavigator me, double time, integer navigationTier) {
	TextGrid_checkSpecifiedTierIsIntervalTier (my textGrid, navigationTier);
	IntervalTier intervalTier =  static_cast<IntervalTier> (my textGrid -> tiers -> at [navigationTier]);
	integer index = IntervalTier_timeToIndex (intervalTier, time);
	if (index == 0) {
		if (time < my xmin) // offleft
			return 0;
		else if (time > my xmax)
			index = intervalTier -> intervals . size + 1; // offright
	}
	if (index == 1)
		return 0;
	if (my navigationLabels) {
		for (integer interval = index - 1; interval > 0; interval --) {
			TextInterval textInterval = intervalTier -> intervals . at [interval];
			if (TextGridNavigator_hasLabelMatch (me, textInterval -> text.get()))
					return interval;
		}
		return 0;
	}
	index --;
	return index;
}

/* End of file TextGridNavigator.cpp */
