/* FFNet.cpp
 *
 * Copyright (C) 1997-2017 David Weenink
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
 djmw 20020712 GPL header
 djmw 20040420 Modified FFNet_create and FFNet_init parameters.
 djmw 20040422 FFNet_drawActivation: nodes with activity > 0.05 had incorrect size.
 djmw 20040422 FFNet_extractWeights added.
 djmw 20040425 FFNet_drawTopology fill input units; increase distance from arrow for output labels
 djmw 20040513 Info changes.
 djmw 20040526 Adapted FFNet_drawCostHistory.
 djmw 20050131 Reversed sign of derivative in minimumCrossEntropy.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20070902 FFNet_createNameFromTopology to wchar
 djmw 20071014 Melder_error<n>
 djmw 20080121 float -> double
 djmw 20110304 Thing_new
*/

#include "FFNet_Matrix.h"
#include "Matrix_extensions.h"
#include "TableOfReal_extensions.h"
#include "PatternList.h"
#include "Collection.h"
#include "Categories.h"

static void bookkeeping (FFNet me);

#include "oo_DESTROY.h"
#include "FFNet_def.h"
#include "oo_COPY.h"
#include "FFNet_def.h"
#include "oo_EQUAL.h"
#include "FFNet_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FFNet_def.h"
#include "oo_WRITE_TEXT.h"
#include "FFNet_def.h"
#include "oo_WRITE_BINARY.h"
#include "FFNet_def.h"
#include "oo_READ_TEXT.h"
#include "FFNet_def.h"
#include "oo_READ_BINARY.h"
#include "FFNet_def.h"
#include "oo_DESCRIPTION.h"
#include "FFNet_def.h"

Thing_implement (FFNet, Daata, 0);

char32 * FFNet_createNameFromTopology (FFNet me) {
	autoMelderString name;
	MelderString_copy (& name, my nUnitsInLayer [0]);
	for (integer i = 1; i <= my nLayers; i ++) {
		MelderString_appendCharacter (& name, U'-');
		MelderString_append (& name, my nUnitsInLayer [i]);
	}
	autostring32 naam = Melder_dup (name.string);
	return naam.transfer();
}

/****** non-linearities ****************************************************/

static double sigmoid (FFNet /*me*/, double x, double *deriv) {
	double act = NUMsigmoid (x);
	*deriv = act * (1.0 - act);
	return act;
}

/* ******************* cost functions ****************************************/

/*
	For the errors calculated in the cost functions:
		if target > activity ==> error > 0
		if target < activity ==> error < 0
*/

static double minimumSquaredError (FFNet me, const double target []) {
	integer k = my nNodes - my nOutputs + 1;
	double cost = 0.0;
	for (integer i = 1; i <= my nOutputs; i ++, k ++) {
		double e = my error [k] = target [i] - my activity [k];
		cost += e * e;
	}
	return 0.5 * cost;
}

/* E = - sum (i=1; i=nPatterns; sum (k=1;k=nOutputs; t [k]*ln (o [k]) + (1-t [k])ln (1-o [k]))) */
/* dE/do [k] = -(1-t [k])/ (1-o [k]) + t [k]/o [k] */
/* werkt niet bij (grote?) netten */
static double minimumCrossEntropy (FFNet me, const double target []) {
	integer k = my nNodes - my nOutputs + 1;
	double cost = 0.0;

	for (integer i = 1; i <= my nOutputs; i ++, k ++) {
		double t1 = 1.0 - target [i];
		double o1 = 1.0 - my activity [k];

		cost -= target [i] * log (my activity [k]) + t1 * log (o1);
		my error [k] = -t1 / o1 + target [i] / my activity [k];
	}
	return cost;
}


/* *********************************************************************** */

static void bookkeeping (FFNet me) {
	integer nWeights = 0;
	my nNodes = my nUnitsInLayer [0];
	for (integer i = 1; i <= my nLayers; i ++) {
		my nNodes += my nUnitsInLayer [i] + 1;
		nWeights += my nUnitsInLayer [i] * (my nUnitsInLayer [i - 1] + 1);
	}
	if (my nWeights > 0 && my nWeights != nWeights) {
		Melder_throw (U"Number of weights is incorret.");
	}
	my nWeights = nWeights;

	// The following test is essential because when an FFNet is read from file the w array already exists
	if (! my w) {
		my w = NUMvector<double> (1, my nWeights);
	}
	my activity = NUMvector<double> (1, my nNodes);
	my isbias = NUMvector<integer> (1, my nNodes);
	my nodeFirst = NUMvector<integer> (1, my nNodes);
	my nodeLast = NUMvector<integer> (1, my nNodes);
	my wFirst = NUMvector<integer> (1, my nNodes);
	my wLast = NUMvector<integer> (1, my nNodes);
	my wSelected = NUMvector<integer> (1, my nWeights);
	my error = NUMvector<double> (1, my nNodes);
	my deriv = NUMvector<double> (1, my nNodes);
	my dwi = NUMvector<double> (1, my nWeights);
	my dw = NUMvector<double> (1, my nWeights);
	my nInputs = my nUnitsInLayer [0];
	my nOutputs = my nUnitsInLayer [my nLayers];
	my isbias [my nInputs + 1] = 1;
	my activity [my nInputs + 1] = 1.0;

	integer n = my nUnitsInLayer [0] + 2;
	integer firstNodeInPrevious = 1, lastWeightInPrevious = 0;
	for (integer j = 1; j <= my nLayers; j ++, n ++) {
		for (integer i = 1; i <= my nUnitsInLayer [j]; i ++, n ++) {
			my isbias [n] = 0;
			my nodeFirst [n] = firstNodeInPrevious;
			my nodeLast [n] = my nodeFirst [n] + my nUnitsInLayer [j - 1];
			my wFirst [n] = lastWeightInPrevious + (i - 1) * (my nUnitsInLayer [j - 1] + 1) + 1;
			my wLast [n] = my wFirst [n] + my nUnitsInLayer [j - 1];
		}
		if (j != my nLayers) {
			my isbias [n] = 1;
			my activity [n] = 1.0;
		}
		lastWeightInPrevious = my wLast [n - 1];
		firstNodeInPrevious += my nUnitsInLayer [j - 1] + 1;
	}
	FFNet_selectAllWeights (me);
}

void structFFNet :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of layers: ", our nLayers);
	MelderInfo_writeLine (U"Total number of units: ", FFNet_getNumberOfUnits (this));
	MelderInfo_writeLine (U"   Number of units in layer ", our nLayers, U" (output): ", our nUnitsInLayer [nLayers]);
	for (integer i = our nLayers - 1; i >= 1; i --) {
		MelderInfo_writeLine (U"   Number of units in layer ", i, U" (hidden): ", our nUnitsInLayer [i]);
	}
	MelderInfo_writeLine (U"   Number of units in layer 0 (input): ", our nUnitsInLayer [0]);
	MelderInfo_writeLine (U"Outputs are linear: ", Melder_boolean (our outputsAreLinear));
	MelderInfo_writeLine (U"Number of weights: ", our nWeights, U" (",
	                       FFNet_dimensionOfSearchSpace (this), U" selected)");
	MelderInfo_writeLine (U"Number of nodes: ", our nNodes);
}

void FFNet_init (FFNet me, integer numberOfInputs, integer nodesInLayer1, integer nodesInLayer2, integer numberOfOutputs, bool outputsAreLinear) {
	integer numberOfLayers = 3;
	
	Melder_require (numberOfInputs > 0, U"Number of inputs should be greater than zero.");
	Melder_require (numberOfOutputs > 0, U"Number of outputs should be greater than zero.");
	
	if (nodesInLayer1 < 1) {
		numberOfLayers --;
	}
	if (nodesInLayer2 < 1) {
		numberOfLayers --;
	}
	my nLayers = numberOfLayers;
	my nUnitsInLayer = NUMvector<integer> (0, numberOfLayers);

	my nUnitsInLayer [numberOfLayers --] = numberOfOutputs;
	if (nodesInLayer2 > 0) {
		my nUnitsInLayer [numberOfLayers --] = nodesInLayer2;
	}
	if (nodesInLayer1 > 0) {
		my nUnitsInLayer [numberOfLayers --] = nodesInLayer1;
	}
	my nUnitsInLayer [numberOfLayers] = numberOfInputs;
	Melder_assert (numberOfLayers == 0);
	my outputsAreLinear = outputsAreLinear;

	bookkeeping (me);

	FFNet_setCostFunction (me, FFNet_COST_MSE);
	FFNet_setNonLinearity (me, FFNet_NONLIN_SIGMOID);
	FFNet_reset (me, 0.1);
}

void FFNet_setOutputCategories (FFNet me, Categories thee) {
	autoCategories uniq = Categories_selectUniqueItems (thee);
	if (uniq->size == thy size) {
		my outputCategories = uniq.move();
	}
}

autoFFNet FFNet_create (integer numberOfInputs, integer numberInLayer1, integer numberInLayer2, integer numberOfOutputs, bool outputsAreLinear) {
	try {
		autoFFNet me = Thing_new (FFNet);
		FFNet_init (me.get(), numberOfInputs, numberInLayer1, numberInLayer2, numberOfOutputs, outputsAreLinear);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FFNet not created.");
	}
}

void FFNet_setNonLinearity (FFNet me, int nonLinearityType) {
	my nonLinearityType = nonLinearityType;
	my nonLinearity = sigmoid;
	my nlClosure = nullptr;
}

void FFNet_setCostFunction (FFNet me, int costType) {
	my costFunctionType = costType;
	if (costType == 2) {
		my costFunction = minimumCrossEntropy;
	} else {
		my costFunction = minimumSquaredError;
	}
	my cfClosure = nullptr;
}

double FFNet_getBias (FFNet me, integer layer, integer unit) {
	try {
		integer node = FFNet_getNodeNumberFromUnitNumber (me, unit, layer);
		Melder_require (node > 0, U"Not a valid unit / layer combination.");
		integer bias_unit = my wLast [node];
		return my w [bias_unit];
	} catch (MelderError) {
		return undefined;
	}
}

void FFNet_setBias (FFNet me, integer layer, integer unit, double value) {
	integer node = FFNet_getNodeNumberFromUnitNumber (me, unit, layer);
	Melder_require (node > 0, U"Not a valid unit / layer combination.");
	integer bias_unit = my wLast [node]; // ??? +1
	my w [bias_unit] = value;
}

void FFNet_setWeight (FFNet me, integer layer, integer unit, integer unit_from, double value) {
	integer node = FFNet_getNodeNumberFromUnitNumber (me, unit, layer);
	Melder_require (node > 0, U"Not a valid unit / layer combination.");
	integer nodef = FFNet_getNodeNumberFromUnitNumber (me, unit_from, layer - 1);
	Melder_require (nodef > 0, U"Not a valid unit / layer combination.");
	integer w_unit = my wFirst [node] + unit_from - 1;
	my w [w_unit] = value;
}

double FFNet_getWeight (FFNet me, integer layer, integer unit, integer unit_from) {
	integer node = FFNet_getNodeNumberFromUnitNumber (me, unit, layer);
	Melder_require (node > 0, U"Not a valid unit / layer combination.");
	
	integer nodef = FFNet_getNodeNumberFromUnitNumber (me, unit_from, layer - 1);
	Melder_require (nodef > 0, U"Not a valid unit / layer combination.");
	integer w_unit = my wFirst [node] + unit_from - 1;
	return my w [w_unit];
}

void FFNet_reset (FFNet me, double weightRange) {
	for (integer i = 1; i <= my nWeights; i ++) {
		if (my wSelected [i]) {
			my w [i] = NUMrandomUniform (- weightRange, weightRange);
		}
	}
	for (integer i = 1; i <= my nNodes; i ++) {
		my activity [i] = ( my isbias [i] ? 1.0 : 0.0 );
	}
	my accumulatedCost = 0.0;
	my minimizer.reset();
}

const char32* FFNet_getCategoryOfOutputUnit (FFNet me, integer outputUnit) {
	const char32 *result = U"-- undefined --";
	if (my outputCategories && outputUnit <= my outputCategories -> size) {
		SimpleString ss = my outputCategories->at [outputUnit];
		result = ss -> string;
	}
	return result;
}

integer FFNet_getOutputUnitOfCategory (FFNet me, const char32* category) {
	integer result = 0;
	if (my outputCategories) {
		for (integer i = 1; i <= my outputCategories -> size; i ++) {
			SimpleString s = my outputCategories->at [i];
			if (Melder_equ (s -> string, category)) {
				result = i;
				break;
			}
		}
	}
	return result;
}

/***** OPERATION: ***********************************************************/
/* step 1 */
void FFNet_propagate (FFNet me, const double input [], double output []) {
	// clamp input pattern on the network
	for (integer i = 1; i <= my nUnitsInLayer [0]; i ++) {
		my activity [i] = input [i];
	}

	// on hidden units use activation function
	integer k = 1, nNodes = my outputsAreLinear ? my nNodes - my nOutputs : my nNodes;
	for (integer i = my nUnitsInLayer [0] + 2; i <= nNodes; i ++) {
		if (my isbias [i]) {
			continue;
		}
		double act = 0.0;
		for (integer j = my nodeFirst [i]; j <= my nodeLast [i]; j ++, k ++) {
			act += my w [k] * my activity [j];
		}
		my activity [i] = my nonLinearity (me, act, & my deriv [i]);
	}
	// on output units use another activation function
	if (my outputsAreLinear) {
		for (integer i = nNodes + 1; i <= my nNodes; i ++) {
			if (my isbias [i]) {
				continue;
			}
			double act = 0.0;
			for (integer j = my nodeFirst [i]; j <= my nodeLast [i]; j ++, k ++) {
				act += my w [k] * my activity [j];
			}
			my activity [i] = act;
			my deriv [i] = 1.0;
		}
	}
	k = my nNodes - my nOutputs + 1;
	if (output) {
		for (integer i = 1; i <= my nOutputs; i ++, k ++) {
			output [i] = my activity [k];
		}
	}
}


double FFNet_computeError (FFNet me, const double target []) {
	// compute error at output layer
	double cost = my costFunction (me, target);
	for (integer i = 1; i <= my nNodes - my nOutputs; i ++) {
		my error [i] = 0.0;
	}
	// backpropagation of errors from output to first hidden layer
	for (integer i = my nNodes; i > my nInputs + 1; i--) {
		if (my isbias [i]) {
			continue;
		}
		my error [i] *= my deriv [i];
		if (my nodeFirst [i] > my nInputs + 1) {
			integer k = my wFirst [i];
			for (integer j = my nodeFirst [i]; j <= my nodeLast [i] - 1; j ++, k ++) {
				my error [j] += my error [i] * my w [k];
			}
		}
	}
	return cost;
}

void FFNet_computeDerivative (FFNet me) {
	integer k = 1;
	for (integer i = my nInputs + 2; i <= my nNodes; i ++) {
		if (! my isbias [i]) {
			for (integer j = my nodeFirst [i]; j <= my nodeLast [i]; j ++, k ++) {
				my dwi [k] = - my error [i] * my activity [j];
			}
		}
	}
}

/******* end operation ******************************************************/

integer FFNet_getWinningUnit (FFNet me, int labeling) {
	integer pos = 1, k = my nNodes - my nOutputs;
	if (labeling == 2) { /* stochastic */
		double sum = 0.0;
		for (integer i = 1; i <= my nOutputs; i ++) {
			sum += my activity [k + i];
		}
		double random = NUMrandomUniform (0.0, sum);
		for (pos = my nOutputs; pos >= 2; pos--) {
			if (random > (sum -= my activity [k + pos])) {
				break;
			}
		}
	} else { /* winner-takes-all */
		double max = my activity [k + 1];
		for (integer i = 2; i <= my nOutputs; i ++) if (my activity [k + i] > max) {
				max = my activity [k + i];
				pos = i;
			}
	}
	return pos;
}

void FFNet_propagateToLayer (FFNet me, const double input [], double activity [], integer layer) {
	Melder_assert (activity);
	integer k = 0;
	FFNet_propagate (me, input, nullptr);
	for (integer i = 0; i < layer; i ++) {
		k += my nUnitsInLayer [i] + 1;
	}
	for (integer i = 1; i <= my nUnitsInLayer [layer]; i ++) {
		activity [i] = my activity [k + i];
	}
}

void FFNet_selectAllWeights (FFNet me) {
	for (integer i = 1; i <= my nWeights; i ++) {
		my wSelected [i] = 1;
	}
	my dimension = my nWeights;
}

integer FFNet_dimensionOfSearchSpace (FFNet me) {
	integer n = 0;
	for (integer i = 1; i <= my nWeights; i ++) {
		if (my wSelected [i]) {
			n ++;
		}
	}
	return n;
}

void FFNet_selectBiasesInLayer (FFNet me, integer layer) {
	integer node = my nUnitsInLayer [0] + 1;
	if (layer < 1 || layer > my nLayers) {
		return;
	}
	for (integer i = 1; i <= my nWeights; i ++) {
		my wSelected [i] = 0.0;
	}
	for (integer i = 1; i < layer; i ++) {
		node += my nUnitsInLayer [i] + 1;
	}
	for (integer i = node + 1; i <= node + my nUnitsInLayer [layer]; i ++) {
		my wSelected [my wLast [i]] = 1;
	}
	my dimension = my nUnitsInLayer [layer];
}

void FFNet_weightConnectsUnits (FFNet me, integer index, integer *fromUnit, integer *toUnit, integer *layer) {
	Melder_assert (index > 0 && index <= my nWeights);

	integer i = 1, np = 0, nw = my nUnitsInLayer [1] * (my nInputs + 1);
	while (index > nw) {
		i ++;
		nw += (np = my nUnitsInLayer [i] * (my nUnitsInLayer [i - 1] + 1));
	}
	if (i > 1) {
		index -= nw - np;
	}
	if (fromUnit) {
		*fromUnit = index % (my nUnitsInLayer [i - 1] + 1);
	}
	if (toUnit) {
		*toUnit = (index - 1) / (my nUnitsInLayer [i - 1] + 1) + 1;
	}
	if (layer) {
		*layer = i;
	}
}

integer FFNet_getNodeNumberFromUnitNumber (FFNet me, integer unit, integer layer) {
	if (layer < 0 || layer > my nLayers || unit > my nUnitsInLayer [layer]) {
		return -1;
	}

	integer node = unit;
	for (integer i = 0; i < layer; i ++) {
		node += my nUnitsInLayer [i] + 1;
	}
	return node;
}

void FFNet_nodeToUnitInLayer (FFNet me, integer node, integer *unit, integer *layer) {
	Melder_assert (node > 0 && node <= my nNodes);

	integer i = 0, nn = my nUnitsInLayer [0] + 1;
	while (node > nn) {
		nn += my nUnitsInLayer [ ++i] + 1;
	}
	if (i > 0) {
		node -= nn - (my nUnitsInLayer [i] + 1);
	}
	*unit = node % (my nUnitsInLayer [i] + 1);
	*layer = i;
}

integer FFNet_getNumberOfWeights (FFNet me) {
	return my nWeights;
}

integer FFNet_getNumberOfLayers (FFNet me) {
	return my nLayers;
}

integer FFNet_getNumberOfUnits (FFNet me) {
	return my nNodes - my nLayers;
}

integer FFNet_getNumberOfHiddenLayers (FFNet me) {
	return my nLayers - 1;
}

integer FFNet_getNumberOfUnitsInLayer (FFNet me, int layer) {
	if (layer > my nLayers || layer < 0) {
		return 0;
	}
	return my nUnitsInLayer [layer];
}

double FFNet_getMinimum (FFNet me) {
	return ( my minimizer ? Minimizer_getMinimum (my minimizer.get()) : undefined );
}

void FFNet_drawTopology (FFNet me, Graphics g) {
	integer maxNumOfUnits = my nUnitsInLayer [0];
	int dxIsFixed = 1;
	double dy = 1.0 / (my nLayers + 1);

	for (integer i = 1; i <= my nLayers; i ++) {
		if (my nUnitsInLayer [i] > maxNumOfUnits) {
			maxNumOfUnits = my nUnitsInLayer [i];
		}
	}
	double dx = 1.0 / maxNumOfUnits;
	double radius = dx / 10.0;
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	for (integer i = 0; i <= my nLayers; i ++) {
		double dx2 = dx, x2WC, y2WC = dy / 2 + i * dy;
		double x2 = (maxNumOfUnits - my nUnitsInLayer [i] + 1) * dx2 / 2;
		/* draw the units */
		if (! dxIsFixed) {
			dx2 = 1.0 / my nUnitsInLayer [i];
			x2 = dx2 / 2.0;
		}
		if (i == 0) {
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
			x2WC = x2;
			for (integer j = 1; j <= my nInputs; j ++) {
				Graphics_arrow (g, x2WC, y2WC - radius - dy / 4.0, x2WC, y2WC - radius);
				x2WC += dx2;
			}
		}
		Graphics_setColour (g, Graphics_RED);
		x2WC = x2;
		for (integer j = 1; j <= my nUnitsInLayer [i]; j ++) {
			Graphics_circle (g, x2WC, y2WC, radius);
			if (i > 0) {
				Graphics_fillCircle (g, x2WC, y2WC, radius);
			}
			x2WC += dx2;
		}
		Graphics_setColour (g, Graphics_BLACK);
		if (i > 0) {
			double dx1 = dx;
			double x1 = (maxNumOfUnits - my nUnitsInLayer [i - 1] + 1) * dx1 / 2.0;
			double y1WC = y2WC - dy;
			if (! dxIsFixed) {
				dx1 = 1.0 / my nUnitsInLayer [i - 1];
				x1 = dx1 / 2.0;
			}
			x2WC = x2;
			for (integer j = 1; j <= my nUnitsInLayer [i]; j ++) {
				double x1WC = x1;
				for (integer k = 1; k <= my nUnitsInLayer [i - 1]; k ++) {
					double xd = x2WC - x1WC;
					double cosa = xd / sqrt (xd * xd + dy * dy);
					double sina = dy / sqrt (xd * xd + dy * dy);
					Graphics_line (g, x1WC + radius * cosa, y1WC + radius * sina, x2WC - radius * cosa, y2WC - radius * sina);
					x1WC += dx1;
				}
				x2WC += dx2;
			}
		}
		if (i == my nLayers) {
			x2WC = x2;
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			for (integer j = 1; j <= my nOutputs; j ++) {
				Graphics_arrow (g, x2WC, y2WC + radius, x2WC, y2WC + radius + dy / 4.0);
				if (my outputCategories) {
					Categories_drawItem (my outputCategories.get(), g, j, x2WC, y2WC + radius + dy / 4.0);
				}
				x2WC += dx2;
			}
		}
	}
	Graphics_unsetInner (g);
}

void FFNet_drawActivation (FFNet me, Graphics g) {
	integer node = 1, maxNumOfUnits = my nUnitsInLayer [0];
	int dxIsFixed = 1;
	Graphics_Colour colour = Graphics_inqColour (g);
	double dy = 1.0 / (my nLayers + 1);

	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	for (integer i = 1; i <= my nLayers; i ++) {
		if (my nUnitsInLayer [i] > maxNumOfUnits) {
			maxNumOfUnits = my nUnitsInLayer [i];
		}
	}
	double dx = 1.0 / maxNumOfUnits;
	double r1 = dx / 2.0; /* May touch when neighbouring activities are both 1 (very rare). */
	for (integer i = 0; i <= my nLayers; i ++, node ++) {
		double dx2 = dx, x2WC, y2WC = dy / 2.0 + i * dy;
		double x2 = (maxNumOfUnits - my nUnitsInLayer [i] + 1) * dx2 / 2.0;
		if (! dxIsFixed) {
			dx2 = 1.0 / my nUnitsInLayer [i];
			x2 = dx2 / 2.0;
		}
		x2WC = x2;
		for (integer j = 1; j <= my nUnitsInLayer [i]; j ++, node ++) {
			double activity = my activity [node];
			double radius = r1 * (fabs (activity) < 0.05 ? 0.05 : fabs (activity));
			/*Graphics_setColour (g, activity < 0 ? Graphics_BLACK : Graphics_RED);*/
			Graphics_circle (g, x2WC, y2WC, radius);
			if (activity < 0) {
				Graphics_fillCircle (g, x2WC, y2WC, radius);
			}
			x2WC += dx2;
		}
	}
	Graphics_setColour (g, colour);
	Graphics_unsetInner (g);
}

/* This routine is deprecated since praat-4.2.4 20040422 and will be removed in the future. */
void FFNet_drawWeightsToLayer (FFNet me, Graphics g, int layer, int scaling, int garnish) {
	Melder_require (layer > 0 && layer <= my nLayers, U"Layer number should be between 1 and ", my nLayers, U".");
	
	autoMatrix weights = FFNet_weightsToMatrix (me, layer, false);
	Matrix_scale (weights.get(), scaling);
	Matrix_drawAsSquares (weights.get(), g, 0.0, 0.0, 0.0, 0.0, 0);
	if (garnish) {
		double x1WC, x2WC, y1WC, y2WC;
		Graphics_inqWindow (g, & x1WC, & x2WC, & y1WC, & y2WC);
		Graphics_textBottom (g, false, Melder_cat (U"Units in layer ", layer, U" ->"));
		if (layer == 1) {
			Graphics_textLeft (g, false, U"Input units ->");
		} else {
			Graphics_textLeft (g, false, Melder_cat (U"Units in layer ", layer - 1, U" ->"));
		}
		/* how do I find out the current settings ??? */
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_setInner (g);
		Graphics_text (g, 0.5, weights->ny, U"bias");
		Graphics_unsetInner (g);
	}
}

void FFNet_drawWeights (FFNet me, Graphics g, integer layer, int garnish) {
	autoTableOfReal thee = FFNet_extractWeights (me, layer);
	TableOfReal_drawAsSquares (thee.get(), g, 1, thy numberOfRows, 1, thy numberOfColumns, garnish);
}

void FFNet_drawCostHistory (FFNet me, Graphics g, integer iFrom, integer iTo, double costMin, double costMax, int garnish) {
	if (my minimizer) {
		Minimizer_drawHistory (my minimizer.get(), g, iFrom, iTo, costMin, costMax, 0);
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, true, my costFunctionType == FFNet_COST_MSE ? U"Minimum squared error" : U"Minimum cross entropy");
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Number of epochs");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

autoCollection FFNet_createIrisExample (integer numberOfHidden1, integer numberOfHidden2) {
	try {
		autoCollection collection = Collection_create ();
		autoCategories uniq = Categories_createWithSequentialNumbers (3);
		autoFFNet me = FFNet_create (4, numberOfHidden1, numberOfHidden2, 3, false);
		FFNet_setOutputCategories (me.get(), uniq.get());
		autostring32 name = FFNet_createNameFromTopology (me.get());
		Thing_setName (me.get(), name.peek());
		collection -> addItem_move (me.move());
		autoTableOfReal iris = TableOfReal_createIrisDataset ();

		/*
			Scale data to interval [0-1]
		 */
		for (integer i = 1; i <= 150; i ++) {
			for (integer j = 1; j <= 4; j ++) {
				iris -> data [i] [j] /= 10.0;
			}
		}

		autoPatternList ap;
		autoCategories ac;
		TableOfReal_to_PatternList_and_Categories (iris.get(), 0, 0, 0, 0, & ap, & ac);
		Thing_setName (ap.get(), U"iris");
		Thing_setName (ac.get(), U"iris");
		collection -> addItem_move (ap.move());
		collection -> addItem_move (ac.move());
		return collection;
	} catch (MelderError) {
		Melder_throw (U"Iris example not created.");
	}
}

autoTableOfReal FFNet_extractWeights (FFNet me, integer layer) {
	try {
		Melder_require (layer > 0 && layer <= my nLayers, U"Layer number should be between 1 and ", my nLayers, U".");

		integer numberOfUnitsFrom = my nUnitsInLayer [layer - 1] + 1;
		integer numberOfUnitsTo = my nUnitsInLayer [layer];
		autoTableOfReal thee = TableOfReal_create (numberOfUnitsFrom, numberOfUnitsTo);

		char32 label [40];
		for (integer i = 1; i <= numberOfUnitsFrom - 1; i ++) {
			Melder_sprint (label,40, U"L", layer - 1, U"-", i);
			TableOfReal_setRowLabel (thee.get(), i, label);
		}
		TableOfReal_setRowLabel (thee.get(), numberOfUnitsFrom, U"Bias");
		for (integer i = 1; i <= numberOfUnitsTo; i ++) {
			Melder_sprint (label,40, U"L", layer, U"-", i);
			TableOfReal_setColumnLabel (thee.get(), i, label);
		}

		integer node = 1;
		for (integer i = 0; i < layer; i ++) {
			node += my nUnitsInLayer [i] + 1;
		}
		for (integer i = 1; i <= numberOfUnitsTo; i ++, node ++) {
			integer k = 1;
			for (integer j = my wFirst [node]; j <= my wLast [node]; j ++) {
				thy data [k ++] [i] = my w [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no TableOfReal created.");
	}
}

autoFFNet PatternList_Categories_to_FFNet (PatternList me, Categories you, integer numberOfUnits1, integer numberOfUnits2) {
	try {
		numberOfUnits1 = numberOfUnits1 > 0 ? numberOfUnits1 : 0;
		numberOfUnits2 = numberOfUnits2 > 0 ? numberOfUnits2 : 0;
		autoCategories uniq = Categories_selectUniqueItems (you);
		integer numberOfOutputs = uniq -> size;
		Melder_require (numberOfOutputs > 0, U"The Categories should not be empty.");
		autoFFNet result = FFNet_create (my nx, numberOfUnits1, numberOfUnits2, numberOfOutputs, false);
		FFNet_setOutputCategories (result.get(), uniq.get());
		autostring32 ffnetName = FFNet_createNameFromTopology (result.get());
		Thing_setName (result.get(), ffnetName.peek());
		return result;
	} catch (MelderError) {
		Melder_throw (me, you, U": no FFNet created.");
	}
}
/* End of file FFNet.cpp */
