/* FFNet_PatternList_Categories.cpp
 *
 * Copyright (C) 1994-2011, 2015-2016 David Weenink
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
 djmw 20020712 GPL header.
 djmw 20020910 changes.
 djmw 20030701 Removed non-GPL minimizations.
 djmw 20041118 Added FFNet_PatternList_Categories_getCosts.
*/

#include "FFNet_ActivationList_Categories.h"
#include "FFNet_PatternList_Categories.h"
#include "FFNet_PatternList_ActivationList.h"

static void _FFNet_PatternList_Categories_checkDimensions (FFNet me, PatternList p, Categories c) {
	if (my nInputs != p -> nx) {
		Melder_throw (U"The PatternList and the FFNet do not match.\nThe number of colums in the PatternList must equal the number of inputs in the FFNet.");
	}
	if (p -> ny != c->size) {
		Melder_throw (U"The PatternList and the categories do not match.\nThe number of rows in the PatternList must equal the number of categories.");
	}
	if (! _PatternList_checkElements (p)) {
		Melder_throw (U"All PatternList elements must be in the interval [0, 1].\nYou could use \"Formula...\" to scale the PatternList values first.");
	}
}

double FFNet_PatternList_Categories_getCosts_total (FFNet me, PatternList p, Categories c, int costFunctionType) {
	try {
		      _FFNet_PatternList_Categories_checkDimensions (me, p, c);
		autoActivationList activation = FFNet_Categories_to_ActivationList (me, c);
		return FFNet_PatternList_ActivationList_getCosts_total (me, p, activation.get(), costFunctionType);
	} catch (MelderError) {
		return undefined;
	}
}

double FFNet_PatternList_Categories_getCosts_average (FFNet me, PatternList p, Categories c, int costFunctionType) {
	double costs = FFNet_PatternList_Categories_getCosts_total (me, p, c, costFunctionType);
	return ( isundef (costs) ? undefined : costs / p -> ny );
}

void FFNet_PatternList_Categories_learnSD (FFNet me, PatternList p, Categories c, long maxNumOfEpochs, double tolerance, double learningRate, double momentum, int costFunctionType) {
	   _FFNet_PatternList_Categories_checkDimensions (me, p, c);
	autoActivationList activation = FFNet_Categories_to_ActivationList (me, c);
	double min, max;
	Matrix_getWindowExtrema (p, 0, 0, 0, 0, & min, & max);
	FFNet_PatternList_ActivationList_learnSD (me, p, activation.get(), maxNumOfEpochs, tolerance, learningRate, momentum, costFunctionType);
}

void FFNet_PatternList_Categories_learnSM (FFNet me, PatternList p, Categories c, long maxNumOfEpochs, double tolerance, int costFunctionType) {
	   _FFNet_PatternList_Categories_checkDimensions (me, p, c);
	autoActivationList activation = FFNet_Categories_to_ActivationList (me, c);
	double min, max;
	Matrix_getWindowExtrema (p, 0, 0, 0, 0, & min, & max);
	FFNet_PatternList_ActivationList_learnSM (me, p, activation.get(), maxNumOfEpochs, tolerance, costFunctionType);
}

autoCategories FFNet_PatternList_to_Categories (FFNet me, PatternList thee, int labeling) {
	try {
		if (! my outputCategories) {
			Melder_throw (U"The FFNet has no output categories.");
		}
		if (my nInputs != thy nx) {
			Melder_throw (U"The number of colums in the PatternList (", thy nx, U") should equal the number of inputs in the FFNet (", my nInputs, U").");
		}
		if (! _PatternList_checkElements (thee)) {
			Melder_throw (U"All PatternList elements must be in the interval [0, 1].\nYou could use \"Formula...\" to scale the PatternList values first.");
		}

		autoCategories him = Categories_create ();

		for (long k = 1; k <= thy ny; k ++) {
			FFNet_propagate (me, thy z [k], nullptr);
			long index = FFNet_getWinningUnit (me, labeling);
			autoSimpleString item = Data_copy (my outputCategories->at [index]);
			his addItem_move (item.move());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Categories created.");
	}
}

/* End of file FFNet_PatternList_Categories.cpp */
