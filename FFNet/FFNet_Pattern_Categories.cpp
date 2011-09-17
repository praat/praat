/* FFNet_Pattern_Categories.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020712 GPL header.
 djmw 20020910 changes.
 djmw 20030701 Removed non-GPL minimizations.
 djmw 20041118 Added FFNet_Pattern_Categories_getCosts.
*/

#include "FFNet_Activation_Categories.h"
#include "FFNet_Pattern_Categories.h"
#include "FFNet_Pattern_Activation.h"

static void _FFNet_Pattern_Categories_checkDimensions (FFNet me, Pattern p, Categories c) {

	if (my nInputs != p -> nx) Melder_throw (L"The Pattern and the FFNet do not match.\n"
		        "The number of colums in the Pattern must equal the number of inputs in the FFNet.");
	if (p -> ny != c -> size) Melder_throw (L"The Pattern and the categories do not match.\n"
		                                        "The number of rows in the Pattern must equal the number of categories.");
	if (! _Pattern_checkElements (p)) Melder_throw (L"The elements in the Pattern are not all "
		        "in the interval [0, 1].\nThe input of the neural net can only process values that are between 0 "
		        "and 1.\nYou could use \"Formula...\" to scale the Pattern values first.");
}

static void _FFNet_Pattern_Categories_learn (FFNet me, Pattern p, Categories c,
        long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType,
        void (*learn) (FFNet, Pattern, Activation, long, double, Any, int)) {
	_FFNet_Pattern_Categories_checkDimensions (me, p, c);
	autoActivation activation = FFNet_Categories_to_Activation (me, c);
	double min, max;
	Matrix_getWindowExtrema (p, 0, 0, 0, 0, &min, &max);
	learn (me, p, activation.peek(), maxNumOfEpochs, tolerance, parameters, costFunctionType);
}

double FFNet_Pattern_Categories_getCosts_total (FFNet me, Pattern p, Categories c, int costFunctionType) {
	try {
		_FFNet_Pattern_Categories_checkDimensions (me, p, c);
		autoActivation activation = FFNet_Categories_to_Activation (me, c);
		return FFNet_Pattern_Activation_getCosts_total (me, p, activation.peek(), costFunctionType);
	} catch (MelderError) {
		return NUMundefined;
	}
}

double FFNet_Pattern_Categories_getCosts_average (FFNet me, Pattern p, Categories c, int costFunctionType) {
	double costs = FFNet_Pattern_Categories_getCosts_total (me, p, c, costFunctionType);
	return costs == NUMundefined ? NUMundefined : costs / p -> ny;
}

void FFNet_Pattern_Categories_learnSM (FFNet me, Pattern p, Categories c,
                                       long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType) {
	_FFNet_Pattern_Categories_learn (me, p, c, maxNumOfEpochs, tolerance, parameters, costFunctionType, FFNet_Pattern_Activation_learnSM);
}

void FFNet_Pattern_Categories_learnSD (FFNet me, Pattern p, Categories c,
                                       long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType) {
	_FFNet_Pattern_Categories_learn (me, p, c, maxNumOfEpochs, tolerance, parameters, costFunctionType, FFNet_Pattern_Activation_learnSD);
}

Categories FFNet_Pattern_to_Categories (FFNet me, Pattern thee, int labeling) {
	try {
		if (my outputCategories == 0) {
			Melder_throw ("The FFNet has no output categories.");
		}
		if (my nInputs != thy nx) Melder_throw ("The number of colums in the Pattern (", thy nx,
			                                        ") must equal the number of inputs in the FFNet (", my nInputs, L").");
		if (! _Pattern_checkElements (thee)) Melder_throw
			("The elements in the Pattern are not all in the interval [0, 1].\n"
			 "The input of the neural net can only process values that are between 0 and 1.\n"
			 "You could use \"Formula...\" to scale the Pattern values first.");


		autoCategories him = Categories_create ();

		for (long k = 1; k <= thy ny; k++) {
			FFNet_propagate (me, thy z[k], 0);
			long index = FFNet_getWinningUnit (me, labeling);
			autoData item = Data_copy ( (Data) my outputCategories -> item[index]);
			Collection_addItem (him.peek(), item.transfer());
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Categories created.");
	}
}

/* End of file FFNet_Pattern_Categories.cpp */
