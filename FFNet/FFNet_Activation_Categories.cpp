/* FFNet_Activation_Categories.cpp
 *
 * Copyright (C) 1997-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 19960322
 djmw 20020712 GPL header
 djmw 20040416 Better error messages
 djmw 20071014 Melder_error<n>
*/

#include "FFNet_Activation_Categories.h"

static long winnerTakesAll (I, const double activation[]) {
	iam (FFNet);
	long pos = 1;
	double max = activation[1];
	for (long i = 2; i <= my nOutputs; i++) {
		if (activation[i] > max) {
			max = activation[i]; pos = i;
		}
	}
	return pos;
}

static long stochastic (I, const double activation[]) {
	iam (FFNet);
	long i;
	double range = 0;
	for (i = 1; i <= my nOutputs; i++) {
		range += activation[i];
	}
	double number = NUMrandomUniform (0, range);
	double lower = 0;
	for (i = 1; i <= my nOutputs; i++) {
		lower += activation[i];
		if (number < lower) {
			break;
		}
	}
	return i;
}

Categories FFNet_Activation_to_Categories (FFNet me, Activation activation, int labeling) {
	try {
		Categories categories = (Categories) my outputCategories;
		long (*labelingFunction) (I, const double act[]);

		if (my outputCategories == 0) {
			Melder_throw ("No Categories (has the FFNet been trained yet?).");
		}
		if (my nOutputs != activation->nx) {
			Melder_throw ("Number of columns and number of outputs must be equal.");
		}
		autoCategories thee = Categories_create ();
		labelingFunction = labeling == 2 ? stochastic : winnerTakesAll;
		for (long i = 1; i <= activation->ny; i++) {
			long index = labelingFunction (me, activation -> z[i]);
			autoSimpleString item = Data_copy ( (SimpleString) categories -> item[index]);
			Collection_addItem (thee.peek(), item.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Categories created.");
	}
}

Activation FFNet_Categories_to_Activation (FFNet me, Categories thee) {
	try {
		autoCategories uniq = Categories_selectUniqueItems (thee, 1);

		if (my outputCategories == 0) {
			Melder_throw ("The FFNet does not have categories.");
		}
		long nl =  OrderedOfString_isSubsetOf (uniq.peek(), my outputCategories, 0);
		if (nl == 0) {
			Melder_throw ("The Categories do not match the categories of the FFNet.");
		}

		autoActivation him = Activation_create (thy size, my nOutputs);
		for (long i = 1; i <= thy size; i++) {
			const wchar *citem = OrderedOfString_itemAtIndex_c (thee, i);
			long pos =  OrderedOfString_indexOfItem_c (my outputCategories, citem);
			if (pos < 1) {
				Melder_throw ("The FFNet doesn't know the category ", citem, ".");
			}
			his z[i][pos] = 1.0;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Activation created.");
	}
}

/* End of file FFNet_Activation_Categories.c */
