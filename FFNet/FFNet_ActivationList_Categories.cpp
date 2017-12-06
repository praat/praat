/* FFNet_ActivationList_Categories.cpp
 *
 * Copyright (C) 1997-2011, 2015-2017 David Weenink
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
 djmw 19960322
 djmw 20020712 GPL header
 djmw 20040416 Better error messages
 djmw 20071014 Melder_error<n>
*/

#include "FFNet_ActivationList_Categories.h"

static integer winnerTakesAll (FFNet me, const double activation[]) {
	integer pos = 1;
	double max = activation[1];
	for (integer i = 2; i <= my nOutputs; i ++) {
		if (activation [i] > max) {
			max = activation [i]; pos = i;
		}
	}
	return pos;
}

static integer stochastic (FFNet me, const double activation []) {
	integer i;
	double range = 0.0, lower = 0.0;
	for (i = 1; i <= my nOutputs; i ++) {
		range += activation [i];
	}
	double number = NUMrandomUniform (0.0, range);
	for (i = 1; i <= my nOutputs; i ++) {
		lower += activation [i];
		if (number < lower) {
			break;
		}
	}
	return i;
}

autoCategories FFNet_ActivationList_to_Categories (FFNet me, ActivationList activation, int labeling) {
	try {
		integer (*labelingFunction) (FFNet me, const double act []);
		Melder_require (my outputCategories, U"No Categories (has the FFNet been trained yet?).");
		Melder_require (my nOutputs == activation -> nx, U"Number of columns and number of outputs should be equal.");

		autoCategories thee = Categories_create ();
		labelingFunction = labeling == 2 ? stochastic : winnerTakesAll;
		for (integer i = 1; i <= activation->ny; i ++) {
			integer index = labelingFunction (me, activation -> z [i]);
			autoSimpleString item = Data_copy (my outputCategories->at  [index]);
			thy addItem_move (item.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Categories created.");
	}
}

autoActivationList FFNet_Categories_to_ActivationList (FFNet me, Categories thee) {
	try {
		autoCategories uniq = Categories_selectUniqueItems (thee);
		Melder_require (my outputCategories, U"The FFNet does not have categories.");
		
		integer nl = OrderedOfString_isSubsetOf (uniq.get(), my outputCategories.get(), 0);
		Melder_require (nl > 0, U"The Categories should match the categories of the FFNet.");

		autoActivationList him = ActivationList_create (thy size, my nOutputs);
		for (integer i = 1; i <= thy size; i ++) {
			const char32 *citem = OrderedOfString_itemAtIndex_c (thee, i);
			integer pos = OrderedOfString_indexOfItem_c (my outputCategories.get(), citem);
			if (pos < 1) {
				Melder_throw (U"The FFNet doesn't know the category ", citem, U".");
			}
			his z [i] [pos] = 1.0;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no ActivationList created.");
	}
}

/* End of file FFNet_ActivationList_Categories.cpp */
