/* FFNet_PatternList_ActivationList.cpp
 *
 * Copyright (C) 1994-2019 David Weenink, 2015,2017 Paul Boersma
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

static double func (Daata object, VEC const& p) {
	FFNet me = (FFNet) object;
	const Minimizer thee = my minimizer.get();

	for (integer j = 1, k = 1; k <= my numberOfWeights; k ++) {
		my dw [k] = 0.0;
		if (my wSelected [k])
			my w [k] = p [j ++];
	}
	longdouble fp = 0.0;
	for (integer i = 1; i <= my numberOfPatterns; i ++) {
		FFNet_propagate (me, my inputPattern.row (i), nullptr);
		fp += FFNet_computeError (me, my targetActivation.row (i));
		FFNet_computeDerivative (me);
		/*
			Derivative (cumulative)
		*/
		my dw.part (1, my numberOfWeights)  +=  my dwi.part (1, my numberOfWeights);
	}
	thy numberOfFunctionCalls ++;
	return (double) fp;
}

static void dfunc_optimized (Daata object, VEC const& /* p */, VEC const& dp) {
	FFNet me = (FFNet) object;

	integer j = 1;
	for (integer k = 1; k <= my numberOfWeights; k ++) {
		if (my wSelected [k])
			dp [j ++] = my dw [k];
	}
}

static void _FFNet_PatternList_ActivationList_checkDimensions (FFNet me, PatternList p, ActivationList a) {
	Melder_require (my numberOfInputs == p -> nx,
		U"The PatternList and the FFNet do not match.\nThe number of columns in the PatternList must equal the number of inputs in the FFNet.");
	Melder_require (my numberOfOutputs == a -> nx,
		U"The Activation and the FFNet do not match.\nThe number of columns in the Activation must equal the number of outputs in the FFNet.");
	Melder_require (p -> ny == a -> ny,
		U"The PatternList and the ActivationList do not match.\nThe number of rows in the PatternList must equal the number of rows in the Activation.");
	Melder_require (_PatternList_checkElements (p),
		U"All PatternList elements should be in the interval [0, 1].\nYou could use \"Formula...\" to scale the PatternList values first.");
	Melder_require (_ActivationList_checkElements (a),
		U"All Activation elements should be in the interval [0, 1].\nYou could use \"Formula...\" to scale the Activation values first.");
}

static void _FFNet_PatternList_ActivationList_learn (FFNet me, PatternList pattern, ActivationList activation, integer maxNumOfEpochs, double tolerance, int costFunctionType, bool reset) {
	try {
		_FFNet_PatternList_ActivationList_checkDimensions (me, pattern, activation);
		/*
			Link the things to be learned
		*/
		my numberOfPatterns = pattern -> ny;
		my inputPattern = pattern -> z.get();
		my targetActivation = activation -> z.get();
		FFNet_setCostFunction (me, costFunctionType);

		if (reset) {
			autoVEC wbuf = zero_VEC (my dimension);
			integer k = 1;
			for (integer i = 1; i <= my numberOfWeights; i ++)
				if (my wSelected [i])
					wbuf [k ++] = my w [i];
			Minimizer_reset (my minimizer.get(), wbuf.get());
		}

		Minimizer_minimize (my minimizer.get(), maxNumOfEpochs, tolerance, 1);

		// Unlink

		my numberOfPatterns = 0;
		my inputPattern = MAT();
		my targetActivation = MAT();
	} catch (MelderError) {
		my numberOfPatterns = 0;
		my inputPattern = MAT();
		my targetActivation = MAT();
	}
}


void FFNet_PatternList_ActivationList_learnSD (FFNet me, PatternList p, ActivationList a, integer maxNumOfEpochs, double tolerance, double learningRate, double momentum, int costFunctionType) {
	bool resetMinimizer = false;
	/*
		Did we choose another minimizer
	*/
	if (my minimizer && ! Thing_isa (my minimizer.get(), classSteepestDescentMinimizer)) {
		my minimizer.reset();
		resetMinimizer = true;
	}
	/*
		Create the minimizer if it doesn't exist
	*/
	if (! my minimizer) {
		resetMinimizer = true;
		my minimizer = SteepestDescentMinimizer_create (my dimension, me, func, dfunc_optimized);
	}
	((SteepestDescentMinimizer) my minimizer.get()) -> eta = learningRate;
	((SteepestDescentMinimizer) my minimizer.get()) -> momentum = momentum;
	_FFNet_PatternList_ActivationList_learn (me, p, a, maxNumOfEpochs, tolerance, costFunctionType, resetMinimizer);
}

void FFNet_PatternList_ActivationList_learnSM (FFNet me, PatternList p, ActivationList a, integer maxNumOfEpochs, double tolerance, int costFunctionType) {
	bool resetMinimizer = false;
	/*
		Did we choose another minimizer
	*/
	if (my minimizer.get() && ! Thing_isa (my minimizer.get(), classVDSmagtMinimizer)) {
		my minimizer.reset();
		resetMinimizer = true;
	}
	/*
		Create the minimizer if it doesn't exist
	*/
	if (! my minimizer.get()) {
		resetMinimizer = true;
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
			FFNet_propagate (me, p -> z.row (i), nullptr);
			cost += FFNet_computeError (me, a -> z.row (i));
		}
		return cost;
	} catch (MelderError) {
		return undefined;
	}
}

double FFNet_PatternList_ActivationList_getCosts_average (FFNet me, PatternList p, ActivationList a, int costFunctionType) {
	const double costs = FFNet_PatternList_ActivationList_getCosts_total (me, p, a, costFunctionType);
	return ( isundef (costs) ? undefined : costs / p -> ny );
}

autoActivationList FFNet_PatternList_to_ActivationList (FFNet me, PatternList p, integer layer) {
	try {
		if (layer < 1 || layer > my numberOfLayers)
			layer = my numberOfLayers;
		Melder_require (my numberOfInputs == p -> nx,
			U"The number of colums in the PatternList (", p -> nx, U") should equal the number of inputs in the FFNet (", my numberOfInputs, U").");
		Melder_require (_PatternList_checkElements (p),
			U"All PatternList elements should be in the interval [0, 1].\nYou could use \"Formula...\" to scale the PatternList values first.");
		
		const integer numberOfPatterns = p -> ny;
		autoActivationList thee = ActivationList_create (numberOfPatterns, my numberOfUnitsInLayer [layer]);
		for (integer i = 1; i <= numberOfPatterns; i ++)
			FFNet_propagateToLayer (me, p -> z.row (i), thy z.row (i), layer);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ActivationList created.");
	}
}

/* End of file FFNet_PatternList_ActivationList.cpp */
