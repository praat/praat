/* FFNet.cpp
 *
 * Copyright (C) 1997-2019 David Weenink
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

#include "FFNet.h"
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

Thing_implement (FFNet, Daata, 1);

autostring32 FFNet_createNameFromTopology (FFNet me) {
	autoMelderString name;
	MelderString_copy (& name, my numberOfInputs); // MelderString_copy (& name, my numberOfInputs);
	for (integer i = 1; i <= my numberOfLayers; i ++) {
		MelderString_appendCharacter (& name, U'-');
		MelderString_append (& name, my numberOfUnitsInLayer [i]);
	}
	return Melder_dup (name.string);
}

/****** non-linearities ****************************************************/

static double sigmoid (FFNet /*me*/, double x, double *out_deriv) {
	double act = NUMsigmoid (x);
	if (out_deriv) *out_deriv = act * (1.0 - act);
	return act;
}

/* ******************* cost functions ****************************************/

/*
	For the errors calculated in the cost functions:
		if target > activity ==> error > 0
		if target < activity ==> error < 0
*/

static double minimumSquaredError (FFNet me, constVEC& target) {
	Melder_assert (my numberOfOutputs == target.size);
	integer k = my numberOfNodes - my numberOfOutputs + 1;
	double cost = 0.0;
	for (integer i = 1; i <= my numberOfOutputs; i ++, k ++) {
		double e = my error [k] = target [i] - my activity [k];
		cost += e * e;
	}
	return 0.5 * cost;
}

/* E = - sum (i=1; i=numberOfPatterns; sum (k=1;k=numberOfOutputs; t [k]*ln (o [k]) + (1-t [k])ln (1-o [k]))) */
/* dE/do [k] = -(1-t [k])/ (1-o [k]) + t [k]/o [k] */
/* werkt niet bij (grote?) netten */
static double minimumCrossEntropy (FFNet me, constVEC& target) {
	Melder_assert (my numberOfOutputs == target.size);
	integer k = my numberOfNodes - my numberOfOutputs + 1;
	double cost = 0.0;

	for (integer i = 1; i <= my numberOfOutputs; i ++, k ++) {
		double t1 = 1.0 - target [i];
		double o1 = 1.0 - my activity [k];

		cost -= target [i] * log (my activity [k]) + t1 * log (o1);
		my error [k] = -t1 / o1 + target [i] / my activity [k];
	}
	return cost;
}


/* *********************************************************************** */

static void bookkeeping (FFNet me) {
	integer numberOfWeights = 0;
	integer numberOfUnitsInPreviousLayer = my numberOfInputs;
	my numberOfNodes = my numberOfInputs; 
	for (integer i = 1; i <= my numberOfLayers; i ++) {
		my numberOfNodes += my numberOfUnitsInLayer [i] + 1;
		numberOfWeights += my numberOfUnitsInLayer [i] * (numberOfUnitsInPreviousLayer + 1);
		numberOfUnitsInPreviousLayer = my numberOfUnitsInLayer [i];
	}
	if (my numberOfWeights > 0 && my numberOfWeights != numberOfWeights)
		Melder_throw (U"Number of weights is incorrect.");

	my numberOfWeights = numberOfWeights;

	// The following test is essential because when an FFNet is read from file the w array already exists
	if (! my w.at) {
		my w = newVECzero (my numberOfWeights);
	}
	my activity = newVECzero (my numberOfNodes);
	my isbias = newINTVECzero (my numberOfNodes);
	my nodeFirst = newINTVECzero (my numberOfNodes);
	my nodeLast = newINTVECzero (my numberOfNodes);
	my wFirst = newINTVECzero (my numberOfNodes);
	my wLast = newINTVECzero (my numberOfNodes);
	my wSelected = newINTVECzero (my numberOfWeights);
	my error = newVECzero (my numberOfNodes);
	my deriv = newVECzero (my numberOfNodes);
	my dwi = newVECzero (my numberOfWeights);
	my dw = newVECzero (my numberOfWeights);
	my numberOfOutputs = my numberOfUnitsInLayer [my numberOfLayers];
	my isbias [my numberOfInputs + 1] = 1;
	my activity [my numberOfInputs + 1] = 1.0;

	integer n = my numberOfInputs + 2;
	integer firstNodeInPrevious = 1, lastWeightInPrevious = 0;
	numberOfUnitsInPreviousLayer = my numberOfInputs;
	for (integer j = 1; j <= my numberOfLayers; j ++, n ++) {
		for (integer i = 1; i <= my numberOfUnitsInLayer [j]; i ++, n ++) {
			my isbias [n] = 0;
			my nodeFirst [n] = firstNodeInPrevious;
			my nodeLast [n] = my nodeFirst [n] + numberOfUnitsInPreviousLayer;
			my wFirst [n] = lastWeightInPrevious + (i - 1) * (numberOfUnitsInPreviousLayer + 1) + 1;
			my wLast [n] = my wFirst [n] + numberOfUnitsInPreviousLayer;
		}
		if (j != my numberOfLayers) {
			my isbias [n] = 1;
			my activity [n] = 1.0;
		}
		lastWeightInPrevious = my wLast [n - 1];
		firstNodeInPrevious += numberOfUnitsInPreviousLayer + 1;
		numberOfUnitsInPreviousLayer = my numberOfUnitsInLayer [j];
	}
	FFNet_selectAllWeights (me);
}

void structFFNet :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of layers: ", our numberOfLayers);
	MelderInfo_writeLine (U"Total number of units: ", FFNet_getNumberOfUnits (this));
	MelderInfo_writeLine (U"   Number of units in layer ", our numberOfLayers, U" (output): ", our numberOfUnitsInLayer [numberOfLayers]);
	for (integer i = our numberOfLayers - 1; i >= 1; i --)
		MelderInfo_writeLine (U"   Number of units in layer ", i, U" (hidden): ", our numberOfUnitsInLayer [i]);
	MelderInfo_writeLine (U"   Number of units in input: ", our numberOfInputs);
	MelderInfo_writeLine (U"Outputs are linear: ", Melder_boolean (our outputsAreLinear));
	MelderInfo_writeLine (U"Number of weights: ", our numberOfWeights, U" (",
	                       FFNet_dimensionOfSearchSpace (this), U" selected)");
	MelderInfo_writeLine (U"Number of nodes: ", our numberOfNodes);
}

void FFNet_init (FFNet me, integer numberOfInputs, integer nodesInLayer1, integer nodesInLayer2, integer numberOfOutputs, bool outputsAreLinear) {
	integer numberOfLayers = 3;
	
	Melder_require (numberOfInputs > 0,
		U"Number of inputs should be greater than zero.");
	Melder_require (numberOfOutputs > 0,
		U"Number of outputs should be greater than zero.");
	
	if (nodesInLayer1 < 1)
		numberOfLayers --;
	if (nodesInLayer2 < 1)
		numberOfLayers --;
	my numberOfLayers = numberOfLayers;
	my numberOfUnitsInLayer = newINTVECzero (numberOfLayers);

	my numberOfUnitsInLayer [numberOfLayers --] = numberOfOutputs;
	if (nodesInLayer2 > 0)
		my numberOfUnitsInLayer [numberOfLayers --] = nodesInLayer2;
	if (nodesInLayer1 > 0)
		my numberOfUnitsInLayer [numberOfLayers --] = nodesInLayer1;
	my numberOfInputs = numberOfInputs;
	my numberOfOutputs = numberOfOutputs;
	Melder_assert (numberOfLayers == 0);
	my outputsAreLinear = outputsAreLinear;

	bookkeeping (me);

	FFNet_setCostFunction (me, FFNet_COST_MSE);
	FFNet_setNonLinearity (me, FFNet_NONLIN_SIGMOID);
	FFNet_reset (me, 0.1);
}

void FFNet_setOutputCategories (FFNet me, Categories thee) {
	autoCategories uniq = Categories_selectUniqueItems (thee);
	if (uniq->size == thy size)
		my outputCategories = uniq.move();
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
		Melder_clearError ();
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
	for (integer i = 1; i <= my numberOfWeights; i ++)
		if (my wSelected [i])
			my w [i] = NUMrandomUniform (- weightRange, weightRange);

	for (integer i = 1; i <= my numberOfNodes; i ++)
		my activity [i] = ( my isbias [i] ? 1.0 : 0.0 );

	my accumulatedCost = 0.0;
	my minimizer.reset();
}

conststring32 FFNet_getCategoryOfOutputUnit (FFNet me, integer outputUnit) {
	conststring32 result = U"-- undefined --";
	if (my outputCategories && outputUnit <= my outputCategories -> size) {
		SimpleString ss = my outputCategories->at [outputUnit];
		result = ss -> string.get();
	}
	return result;
}

integer FFNet_getOutputUnitOfCategory (FFNet me, const char32* category) {
	integer result = 0;
	if (my outputCategories) {
		for (integer i = 1; i <= my outputCategories -> size; i ++) {
			SimpleString s = my outputCategories->at [i];
			if (Melder_equ (s -> string.get(), category)) {
				result = i;
				break;
			}
		}
	}
	return result;
}

/***** OPERATION: ***********************************************************/
/* step 1 */
void FFNet_propagate (FFNet me, constVEC input, autoVEC *output) {
	Melder_assert (my numberOfInputs == input.size);
	// clamp input pattern on the network
	my activity.part (1, my numberOfInputs) <<= input;

	// on hidden units use activation function
	integer k = 1, numberOfNodes = my outputsAreLinear ? my numberOfNodes - my numberOfOutputs : my numberOfNodes;
	for (integer i = my numberOfInputs + 2; i <= numberOfNodes; i ++) {
		if (my isbias [i])
			continue;
		longdouble act = 0.0;
		for (integer j = my nodeFirst [i]; j <= my nodeLast [i]; j ++, k ++) 
			act += my w [k] * my activity [j];

		my activity [i] = my nonLinearity (me, act, & my deriv [i]);
	}
	// on output units use another activation function
	if (my outputsAreLinear) {
		for (integer i = numberOfNodes + 1; i <= my numberOfNodes; i ++) {
			if (my isbias [i])
				continue;
			longdouble act = 0.0;
			for (integer j = my nodeFirst [i]; j <= my nodeLast [i]; j ++, k ++)
				act += my w [k] * my activity [j];
			my activity [i] = (double) act;
			my deriv [i] = 1.0;
		}
	}
	k = my numberOfNodes - my numberOfOutputs + 1;
	if (output) {
		Melder_assert (my numberOfOutputs == (*output).size);
		for (integer i = 1; i <= my numberOfOutputs; i ++, k ++)
			(*output) [i] = my activity [k];
	}
}


double FFNet_computeError (FFNet me, constVEC target) {
	Melder_assert (my numberOfOutputs == target.size);
	// compute error at output layer
	double cost = my costFunction (me, target);
	for (integer i = 1; i <= my numberOfNodes - my numberOfOutputs; i ++)
		my error [i] = 0.0;

	// backpropagation of errors from output to first hidden layer
	for (integer i = my numberOfNodes; i > my numberOfInputs + 1; i--) {
		if (my isbias [i])
			continue;

		my error [i] *= my deriv [i];
		if (my nodeFirst [i] > my numberOfInputs + 1) {
			integer k = my wFirst [i];
			for (integer j = my nodeFirst [i]; j <= my nodeLast [i] - 1; j ++, k ++)
				my error [j] += my error [i] * my w [k];
		}
	}
	return cost;
}

void FFNet_computeDerivative (FFNet me) {
	integer k = 1;
	for (integer i = my numberOfInputs + 2; i <= my numberOfNodes; i ++) {
		if (! my isbias [i])
			for (integer j = my nodeFirst [i]; j <= my nodeLast [i]; j ++, k ++)
				my dwi [k] = - my error [i] * my activity [j];
	}
}

/******* end operation ******************************************************/

integer FFNet_getWinningUnit (FFNet me, int labeling) {
	integer pos = 1, k = my numberOfNodes - my numberOfOutputs;
	if (labeling == 2) { /* stochastic */
		double sum = 0.0;
		for (integer i = 1; i <= my numberOfOutputs; i ++)
			sum += my activity [k + i];

		double random = NUMrandomUniform (0.0, sum);
		for (pos = my numberOfOutputs; pos >= 2; pos--) {
			if (random > (sum -= my activity [k + pos]))
				break;
		}
	} else { /* winner-takes-all */
		double max = my activity [k + 1];
		for (integer i = 2; i <= my numberOfOutputs; i ++)
			if (my activity [k + i] > max) {
				max = my activity [k + i];
				pos = i;
			}
	}
	return pos;
}

void FFNet_propagateToLayer (FFNet me, constVEC input, VEC activity, integer layer) {
	Melder_require (layer > 0, U"Layer must be greater than zero.");
	Melder_assert (my numberOfUnitsInLayer [layer] == activity.size);
	FFNet_propagate (me, input, nullptr);
	integer k = my numberOfInputs + 1;
	for (integer i = 1; i < layer; i ++)
		k += my numberOfUnitsInLayer [i] + 1;

	for (integer i = 1; i <= my numberOfUnitsInLayer [layer]; i ++)
		activity [i] = my activity [k + i];
}

void FFNet_selectAllWeights (FFNet me) {
	for (integer i = 1; i <= my numberOfWeights; i ++)
		my wSelected [i] = 1;
	my dimension = my numberOfWeights;
}

integer FFNet_dimensionOfSearchSpace (FFNet me) {
	integer n = 0;
	for (integer i = 1; i <= my numberOfWeights; i ++)
		if (my wSelected [i])
			n ++;
	return n;
}

void FFNet_selectBiasesInLayer (FFNet me, integer layer) {
	if (layer < 1 || layer > my numberOfLayers)
		return;

	for (integer i = 1; i <= my numberOfWeights; i ++)
		my wSelected [i] = 0.0;
	integer node = my numberOfInputs + 1;
	for (integer i = 1; i < layer; i ++)
		node += my numberOfUnitsInLayer [i] + 1;
	for (integer i = node + 1; i <= node + my numberOfUnitsInLayer [layer]; i ++)
		my wSelected [my wLast [i]] = 1;
	my dimension = my numberOfUnitsInLayer [layer];
}

void FFNet_weightConnectsUnits (FFNet me, integer index, integer *out_fromUnit, integer *out_toUnit, integer *out_layer) {
	Melder_assert (index > 0 && index <= my numberOfWeights);

	integer i = 1, np = 0, nw = my numberOfUnitsInLayer [1] * (my numberOfInputs + 1);
	while (index > nw) {
		i ++;
		nw += (np = my numberOfUnitsInLayer [i] * (my numberOfUnitsInLayer [i - 1] + 1));
	}
	if (i > 1)
		index -= nw - np;
	integer numberOfUnitsInPreviousLayer = ( i == 1 ? my numberOfInputs : my numberOfUnitsInLayer [i - 1] );
	if (out_fromUnit) *out_fromUnit = index % (numberOfUnitsInPreviousLayer + 1);
	if (out_toUnit) *out_toUnit = (index - 1) / (numberOfUnitsInPreviousLayer + 1) + 1;
	if (out_layer) *out_layer = i;
}

integer FFNet_getNodeNumberFromUnitNumber (FFNet me, integer unit, integer layer) {
	if (layer < 0 || layer > my numberOfLayers || layer == 0 && unit > my numberOfInputs || layer > 0 && unit > my numberOfUnitsInLayer [layer])
		return -1;
	integer node = unit;
	if (layer > 0) {
		node += my numberOfInputs + 1;
		for (integer i = 1; i < layer; i ++)
			node += my numberOfUnitsInLayer [i] + 1;
	}
	if (node > my numberOfNodes) node = -1;
	return node;
}


integer FFNet_getNumberOfWeights (FFNet me) {
	return my numberOfWeights;
}

integer FFNet_getNumberOfLayers (FFNet me) {
	return my numberOfLayers;
}

integer FFNet_getNumberOfUnits (FFNet me) {
	return my numberOfNodes - my numberOfLayers;
}

integer FFNet_getNumberOfHiddenumberOfLayers (FFNet me) {
	return my numberOfLayers - 1;
}

integer FFNet_getNumberOfUnitsInLayer (FFNet me, int layer) {
	return ( layer < 0 || layer > my numberOfLayers ? 0 : 
		layer == 0 ? my numberOfInputs : my numberOfUnitsInLayer [layer] );
}

double FFNet_getMinimum (FFNet me) {
	return ( my minimizer ? Minimizer_getMinimum (my minimizer.get()) : undefined );
}

void FFNet_drawTopology (FFNet me, Graphics g) {
	integer maxNumOfUnits = my numberOfInputs;
	int dxIsFixed = 1;
	double dy = 1.0 / (my numberOfLayers + 1);

	for (integer i = 1; i <= my numberOfLayers; i ++)
		if (my numberOfUnitsInLayer [i] > maxNumOfUnits)
			maxNumOfUnits = my numberOfUnitsInLayer [i];

	double dx = 1.0 / maxNumOfUnits;
	double radius = dx / 10.0;
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	for (integer i = 0; i <= my numberOfLayers; i ++) {
		integer numberOfUnitsInLayer = ( i == 0 ? my numberOfInputs : my numberOfUnitsInLayer [i] );
		double dx2 = dx, x2WC, y2WC = dy / 2 + i * dy;
		double x2 = (maxNumOfUnits - numberOfUnitsInLayer + 1) * dx2 / 2;
		/* draw the units */
		if (! dxIsFixed) {
			dx2 = 1.0 / numberOfUnitsInLayer;
			x2 = dx2 / 2.0;
		}
		if (i == 0) {
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
			x2WC = x2;
			for (integer j = 1; j <= my numberOfInputs; j ++) {
				Graphics_arrow (g, x2WC, y2WC - radius - dy / 4.0, x2WC, y2WC - radius);
				x2WC += dx2;
			}
		}
		Graphics_setColour (g, Graphics_RED);
		x2WC = x2;
		for (integer j = 1; j <= numberOfUnitsInLayer; j ++) {
			Graphics_circle (g, x2WC, y2WC, radius);
			if (i > 0)
				Graphics_fillCircle (g, x2WC, y2WC, radius);
			x2WC += dx2;
		}
		Graphics_setColour (g, Graphics_BLACK);
		if (i > 0) {
			integer numberOfUnitsInLayer_m1 = ( i == 1 ? my numberOfInputs : my numberOfUnitsInLayer [i - 1] );
			double dx1 = dx;
			double x1 = (maxNumOfUnits - numberOfUnitsInLayer_m1 + 1) * dx1 / 2.0;
			double y1WC = y2WC - dy;
			if (! dxIsFixed) {
				dx1 = 1.0 / numberOfUnitsInLayer_m1;
				x1 = dx1 / 2.0;
			}
			x2WC = x2;
			for (integer j = 1; j <= numberOfUnitsInLayer; j ++) {
				double x1WC = x1;
				for (integer k = 1; k <= numberOfUnitsInLayer_m1; k ++) {
					double xd = x2WC - x1WC;
					double cosa = xd / sqrt (xd * xd + dy * dy);
					double sina = dy / sqrt (xd * xd + dy * dy);
					Graphics_line (g, x1WC + radius * cosa, y1WC + radius * sina, x2WC - radius * cosa, y2WC - radius * sina);
					x1WC += dx1;
				}
				x2WC += dx2;
			}
		}
		if (i == my numberOfLayers) {
			x2WC = x2;
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
			for (integer j = 1; j <= my numberOfOutputs; j ++) {
				Graphics_arrow (g, x2WC, y2WC + radius, x2WC, y2WC + radius + dy / 4.0);
				if (my outputCategories)
					Categories_drawItem (my outputCategories.get(), g, j, x2WC, y2WC + radius + dy / 4.0);
				x2WC += dx2;
			}
		}
	}
	Graphics_unsetInner (g);
}

void FFNet_drawActivation (FFNet me, Graphics g) {
	integer node = 1, maxNumOfUnits = my numberOfInputs;
	int dxIsFixed = 1;
	MelderColour colour = Graphics_inqColour (g);
	double dy = 1.0 / (my numberOfLayers + 1);

	Graphics_setInner (g);
	Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0);
	for (integer i = 1; i <= my numberOfLayers; i ++)
		if (my numberOfUnitsInLayer [i] > maxNumOfUnits)
			maxNumOfUnits = my numberOfUnitsInLayer [i];

	double dx = 1.0 / maxNumOfUnits;
	double r1 = dx / 2.0; // May touch when neighbouring activities are both 1 (very rare).
	for (integer i = 0; i <= my numberOfLayers; i ++, node ++) {
		integer numberOfUnitsInLayer = ( i == 0 ? my numberOfInputs : my numberOfUnitsInLayer [i] );
		double dx2 = dx, x2WC, y2WC = dy / 2.0 + i * dy;
		double x2 = (maxNumOfUnits - numberOfUnitsInLayer + 1) * dx2 / 2.0;
		if (! dxIsFixed) {
			dx2 = 1.0 / numberOfUnitsInLayer;
			x2 = dx2 / 2.0;
		}
		x2WC = x2;
		for (integer j = 1; j <= numberOfUnitsInLayer; j ++, node ++) {
			double activity = my activity [node];
			double radius = r1 * (fabs (activity) < 0.05 ? 0.05 : fabs (activity));
			/*Graphics_setColour (g, activity < 0 ? Graphics_BLACK : Graphics_RED);*/
			Graphics_circle (g, x2WC, y2WC, radius);
			if (activity < 0)
				Graphics_fillCircle (g, x2WC, y2WC, radius);
			x2WC += dx2;
		}
	}
	Graphics_setColour (g, colour);
	Graphics_unsetInner (g);
}

/* This routine is deprecated since praat-4.2.4 20040422 and will be removed in the future. */
void FFNet_drawWeightsToLayer (FFNet me, Graphics g, int layer, int scaling, bool garnish) {
	Melder_require (layer > 0 && layer <= my numberOfLayers, U"Layer number should be between 1 and ", my numberOfLayers, U".");
	
	autoMatrix weights = FFNet_weightsToMatrix (me, layer, false);
	Matrix_scale (weights.get(), scaling);
	Matrix_drawAsSquares (weights.get(), g, 0.0, 0.0, 0.0, 0.0, 0);
	if (garnish) {
		double x1WC, x2WC, y1WC, y2WC;
		Graphics_inqWindow (g, & x1WC, & x2WC, & y1WC, & y2WC);
		Graphics_textBottom (g, false, Melder_cat (U"Units in layer ", layer, U" ->"));
		if (layer == 1)
			Graphics_textLeft (g, false, U"Input units ->");
		else
			Graphics_textLeft (g, false, Melder_cat (U"Units in layer ", layer - 1, U" ->"));

		/* how do I find out the current settings ??? */
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_setInner (g);
		Graphics_text (g, 0.5, weights->ny, U"bias");
		Graphics_unsetInner (g);
	}
}

void FFNet_drawWeights (FFNet me, Graphics g, integer layer, bool garnish) {
	autoTableOfReal thee = FFNet_extractWeights (me, layer);
	TableOfReal_drawAsSquares (thee.get(), g, 1, thy numberOfRows, 1, thy numberOfColumns, garnish);
}

void FFNet_drawCostHistory (FFNet me, Graphics g, integer iFrom, integer iTo, double costMin, double costMax, bool garnish) {
	if (my minimizer)
		Minimizer_drawHistory (my minimizer.get(), g, iFrom, iTo, costMin, costMax, 0);

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
		Thing_setName (me.get(), name.get());
		collection -> addItem_move (me.move());
		autoTableOfReal iris = TableOfReal_createIrisDataset ();

		iris -> data.all()  *=  0.1; // Scale data to interval [0-1]

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
		Melder_require (layer > 0 && layer <= my numberOfLayers,
			U"Layer number should be between 1 and ", my numberOfLayers, U".");

		integer numberOfUnitsFrom = ( layer == 1 ? my numberOfInputs + 1 : my numberOfUnitsInLayer [layer - 1] + 1 );
		integer numberOfUnitsTo = my numberOfUnitsInLayer [layer];
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

		integer node = my numberOfInputs + 1 + 1;
		for (integer i = 1; i < layer; i ++)
			node += my numberOfUnitsInLayer [i] + 1;

		for (integer i = 1; i <= numberOfUnitsTo; i ++, node ++) {
			integer k = 1;
			for (integer j = my wFirst [node]; j <= my wLast [node]; j ++)
				thy data [k ++] [i] = my w [j];
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
		Thing_setName (result.get(), ffnetName.get());
		return result;
	} catch (MelderError) {
		Melder_throw (me, you, U": no FFNet created.");
	}
}
/* End of file FFNet.cpp */
