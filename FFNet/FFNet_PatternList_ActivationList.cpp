/* FFNet_PatternList_ActivationList.cpp
 *
 * Copyright (C) 1994-2017 David Weenink, 2015,2017 Paul Boersma
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
 djmw 19960826
 djmw 20020712 GPL header
 djmw 20030701 Removed non-GPL minimizations
 djmw 20040416 More precise error messages.
 djmw 20041118 Added FFNet_PatternList_Categories_getCosts.
*/

#include "Graphics.h"
#include "FFNet_PatternList_ActivationList.h"

static double func (Daata object, const double p []) {
	FFNet me = (FFNet) object;
	Minimizer thee = my minimizer.get();
	double fp = 0.0;

	for (integer j = 1, k = 1; k <= my nWeights; k ++) {
		my dw [k] = 0.0;
		if (my wSelected [k]) {
			my w [k] = p [j ++];
		}
	}
	for (integer i = 1; i <= my nPatterns; i ++) {
		FFNet_propagate (me, my inputPattern [i], nullptr);
		fp += FFNet_computeError (me, my targetActivation [i]);
		FFNet_computeDerivative (me);
		/* derivative (cumulative) */
		for (integer k = 1; k <= my nWeights; k ++) {
			my dw [k] += my dwi [k];
		}
	}
	thy funcCalls ++;
	return fp;
}

static void dfunc_optimized (Daata object, const double p [], double dp []) {
	FFNet me = (FFNet) object;
	(void) p;

	integer j = 1;
	for (integer k = 1; k <= my nWeights; k ++) {
		if (my wSelected [k]) {
			dp [j ++] = my dw [k];
		}
	}
}

static void _FFNet_PatternList_ActivationList_checkDimensions (FFNet me, PatternList p, ActivationList a) {
	if (my nInputs != p -> nx) {
		Melder_throw (U"The PatternList and the FFNet do not match.\nThe number of columns in the PatternList must equal the number of inputs in the FFNet.");
	}
	if (my nOutputs != a -> nx) {
		Melder_throw (U"The Activation and the FFNet do not match.\nThe number of columns in the Activation must equal the number of outputs in the FFNet.");
	}
	if (p -> ny != a -> ny) {
		Melder_throw (U"The PatternList and the ActivationList do not match.\nThe number of rows in the PatternList must equal the number of rows in the Activation.");
	}
	if (! _PatternList_checkElements (p)) {
		Melder_throw (U"All PatternList elements should be in the interval [0, 1].\nYou could use \"Formula...\" to scale the PatternList values first.");	}
	if (! _ActivationList_checkElements (a)) {
		Melder_throw (U"All Activation elements should be in the interval [0, 1].\nYou could use \"Formula...\" to scale the Activation values first.");
	}
}

static void _FFNet_PatternList_ActivationList_learn (FFNet me, PatternList pattern, ActivationList activation, integer maxNumOfEpochs, double tolerance, int costFunctionType, bool reset) {
	try {
		_FFNet_PatternList_ActivationList_checkDimensions (me, pattern, activation);

		// Link the things to be learned

		my nPatterns = pattern -> ny;
		my inputPattern = pattern -> z;
		my targetActivation = activation -> z;
		FFNet_setCostFunction (me, costFunctionType);

		if (reset) {
			autoNUMvector<double> wbuf (1, my dimension);
			integer k = 1;
			for (integer i = 1; i <= my nWeights; i ++) {
				if (my wSelected [i]) {
					wbuf [k ++] = my w [i];
				}
			}
			Minimizer_reset (my minimizer.get(), wbuf.peek());
		}

		Minimizer_minimize (my minimizer.get(), maxNumOfEpochs, tolerance, 1);

		// Unlink

		my nPatterns = 0;
		my inputPattern = nullptr;
		my targetActivation = nullptr;
	} catch (MelderError) {
		my nPatterns = 0;
		my inputPattern = nullptr;
		my targetActivation = nullptr;
	}
}


void FFNet_PatternList_ActivationList_learnSD (FFNet me, PatternList p, ActivationList a, integer maxNumOfEpochs, double tolerance, double learningRate, double momentum, int costFunctionType) {
	int resetMinimizer = 0;
	/* Did we choose another minimizer */
	if (my minimizer && ! Thing_isa (my minimizer.get(), classSteepestDescentMinimizer)) {
		my minimizer.reset();
		resetMinimizer = 1;
	}
	/* create the minimizer if it doesn't exist */
	if (! my minimizer) {
		resetMinimizer = 1;
		my minimizer = SteepestDescentMinimizer_create (my dimension, me, func, dfunc_optimized);
	}
	((SteepestDescentMinimizer) my minimizer.get()) -> eta = learningRate;
	((SteepestDescentMinimizer) my minimizer.get()) -> momentum = momentum;
	_FFNet_PatternList_ActivationList_learn (me, p, a, maxNumOfEpochs, tolerance, costFunctionType, resetMinimizer);
}

void FFNet_PatternList_ActivationList_learnSM (FFNet me, PatternList p, ActivationList a, integer maxNumOfEpochs, double tolerance, int costFunctionType) {
	int resetMinimizer = 0;

	// Did we choose another minimizer

	if (my minimizer.get() && ! Thing_isa (my minimizer.get(), classVDSmagtMinimizer)) {
		my minimizer.reset();
		resetMinimizer = 1;
	}
	// create the minimizer if it doesn't exist
	if (! my minimizer.get()) {
		resetMinimizer = 1;
		my minimizer = VDSmagtMinimizer_create (my dimension, me, func, dfunc_optimized);
	}
	_FFNet_PatternList_ActivationList_learn (me, p, a, maxNumOfEpochs, tolerance, costFunctionType, resetMinimizer);
}

double FFNet_PatternList_ActivationList_getCosts_total (FFNet me, PatternList p, ActivationList a, int costFunctionType) {
	try {
		_FFNet_PatternList_ActivationList_checkDimensions (me, p, a);
		FFNet_setCostFunction (me, costFunctionType);

		double cost = 0.0;
		for (integer i = 1; i <= p -> ny; i ++) {
			FFNet_propagate (me, p -> z [i], nullptr);
			cost += FFNet_computeError (me, a -> z [i]);
		}
		return cost;
	} catch (MelderError) {
		return undefined;
	}
}

double FFNet_PatternList_ActivationList_getCosts_average (FFNet me, PatternList p, ActivationList a, int costFunctionType) {
	double costs = FFNet_PatternList_ActivationList_getCosts_total (me, p, a, costFunctionType);
	return ( isundef (costs) ? undefined : costs / p -> ny );
}

autoActivationList FFNet_PatternList_to_ActivationList (FFNet me, PatternList p, integer layer) {
	try {
		if (layer < 1 || layer > my nLayers) {
			layer = my nLayers;
		}
		Melder_require (my nInputs == p -> nx, U"The number of colums in the PatternList (", p -> nx, U") should equal the number of inputs in the FFNet (", my nInputs, U").");
		Melder_require (_PatternList_checkElements (p), U"All PatternList elements should be in the interval [0, 1].\nYou could use \"Formula...\" to scale the PatternList values first.");
		
		integer nPatterns = p -> ny;
		autoActivationList thee = ActivationList_create (nPatterns, my nUnitsInLayer [layer]);

		for (integer i = 1; i <= nPatterns; i ++) {
			FFNet_propagateToLayer (me, p -> z [i], thy z [i], layer);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ActivationList created.");
	}
}

/* End of file FFNet_PatternList_ActivationList.cpp */
