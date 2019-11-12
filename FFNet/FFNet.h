#ifndef _FFNet_h_
#define _FFNet_h_
/* FFNet.h
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
 djmw 19970515
 djmw 20020712 GPL header.
 djmw 20031025 Added FFNet_getMinimum.
 djmw 20040420 FFNet_create and Pattern&Categories: To FFNet changes.
 djmw 20040422 FFNet_extractWeights added.
 djmw 20040505 FFNet_getNodeNumberFromUnitNumber added.
 djmw 20071024 Latest modification.
 djmw 20080121 float -> double
*/

#include "Data.h"
#include "Categories.h"
#include "Minimizers.h"
#include "PatternList.h"
#include "TableOfReal.h"
#include "melder.h"

#include "FFNet_def.h"

/* Parameters:
 *  integer numberOfLayers	    : the #layers in the net (exclusive the inputs)
 *    numberOfUnitsInLayer	    : array[1..numberOfLayers] the #units in each layer
 *						  numberOfUnitsInLayer[numberOfLayers] :#outputs
 *    numberOfWeights		    : the total #weights in the net (inclusive bias)
 *  double *w		    : array[1..numberOfWeights] with the connection strengths
 *    *activity		    : array[1..numberOfNodes] with activities
 *  outputLabels	    : labels belonging to the outputs
 *  BOOKKEEPING:
 *  integer numberOfNodes: total #nodes: bias modelled as unit with constant activity)
 *    *isbias		    : array[1..numberOfNodes] set 1 if node is bias else 0
 *    *nodeFirst	    : array[1..numberOfNodes] first node connected to this unit
 *    *nodeLast:	    : array[1..numberOfNodes] last node connected to this unit
 *    *wFirst		    : array[1..numberOfNodes] first index in *w for this unit
 *    *wLast		    : array[1..numberOfNodes] last (inclusive the bias)
 *  LEARNING:
 *  int *wSelected		: array[1..numberOfWeights] weights selected for minimization
 *  double *deriv	    : array[1..numberOfNodes] derivative of nonlinearity at node
 *    *error		    : array[1..numberOfNodes] the error at node
 *    *dw		    	: array[1..numberOfWeights] total derivative for weights
 *    *dwi		   		: array[1..numberOfWeights] derivative per pattern
 *  integer dimension		: dimension of minimizer space (<= my numberOfWeights)
 *  integer numberOfPatterns	    : the #patterns to be learned
 *  MAT inputPattern: matrix[1..numberOfPatterns][1..numberOfInputs]
 *  MAT targetActivation: matrix[1..numberOfPatterns][1..numberOfOutputs]
 *  double accumulatedCost : accumulated costs of testing/training with patterns
 *
 * A network consists of numberOfLayers layers. Layer numbering is from 1...numberOfLayers.
 * The highest numbered layer is the output layer
 *   (numberOfLayers <= 4)
 * Each layer consists of a number of units. The biases of all the units in a layer
 * are modelled with connections to an extra unit in the lower layer (with constant
 * activity 1.0). Nodes refers to 'units' + 'bias units'.
 * The variable 'numberOfNodes' is the total number of nodes (inclusive bias nodes).
 * E.g. the topology (2,3,4), i.e., 2 inputs, 3 units in the first layer
 * and 4 units in the second layer (outputs) is modelled
 * with (2+1)+ (3+1)+ (4) = 11 nodes.
 * The numbering of the weights is as follows (indices 1..numberOfWeights):
 * E.g., topology (I,H,O) (I inputs, H hidden units and O output units)
 * There are a total of H* (I+1) + O* (H+1) weights in this net.
 * w[1] - w[I]                        : I (1)->H (1), I (2)->H (1) ... I (I)->H (1)
 *   w[I+1]                           :   bias->H (1)
 * w[ (I+1)+1] - w[2 (I+1)-1]           : I (1)->H (2), I (2)->H (2) ... I (I)->H (2)
 *   w[2 (I+1)]                        :   bias->H (2)
 *   ...
 * w[ (H-1) (I+1)+1] - w[H (I+1)-1]      : I (1)->H (H), I (2)->H (H) ... I (I)->H (H)
 *   w[H (I+1)]                        :   bias->H (H)
 *      M = H (I+1)
 * w[M+1] - w[M+H]                    : H (1)->O (1), H (2)->O (1) ... H (H)->O (1)
 *   w[M+H+1]                         :   bias->O (1)
 * w[M+ (H+1)+1] - w (M+2 (H+1)-1)       : H (1)->O (2), H (2)->O (2) ... H (H)->O (2)
 *   w[M+2 (H+1)]                      :   bias->O (1)
 *   ...
 * w[M+ (O-1)* (H+1)+1] - w[M+O (H+1)-1] : H (1)->O (O), H (2)->O (O) ... H (H)->O (O)
 *   w[m+o (h+1)-1]                    :   bias->O (O)
 *
 * Internals:
 *
 * A number of auxiliary arrays for efficient calculations have been setup.
 * For a node k we need to know:
 * 1. isbias[1..numberOfNodes]        : usage: if (isbias[k]) ...
 *      true if node k is a bias node. There are numberOfLayers bias nodes
 * 2. nodeFirst[1..numberOfNodes]       : usage is j=nodeFirst[k];
 *      j is the first node that is connected to k .
 * 3. nodeLast[1..numberOfNodes]        : usage is j=nodeLast[k]
 *      j is the  last node that is connected to k (bias included).
 * For the calculation of the errors,  during learning,  in unit k we need to
 * know which weights from the preceeding layer connect to it.
 * 4. wFirst[1..numberOfNodes] : usage j=wFirst[k]
 *      w[j] is first weight to node k.
 * 5. wLast[1..numberOfNodes]  : usage j=wLast[k]
 *      w[j] is last weight to node k.
 */

/* FFNet::copy
 *  copy everything except minimizer, patterns and inputs.
 */

void FFNet_init (FFNet me, integer numberOfInputs, integer nodesInLayer1, integer nodesInLayer2,
	integer numberOfOutputs, bool outputsAreLinear);

autoFFNet FFNet_create (integer numberOfInputs, integer numberInLayer1, integer numberInLayer2,
	integer numberOfOutputs, bool outputsAreLinear);

autostring32 FFNet_createNameFromTopology (FFNet me);
/* Create names as <inputs>-<outputs>, <inputs>-<hidden>-<outputs>,
	<inputs>-<hidden1>-<hidden2>-<outputs> for 1, 2 or 3 layer networks.
*/

#define FFNet_COST_MSE 1

void FFNet_setCostFunction (FFNet me, int type);

#define FFNet_NONLIN_SIGMOID 1

void FFNet_setNonLinearity (FFNet me, int type);

void FFNet_setOutputCategories (FFNet me, Categories thee);

double FFNet_getBias (FFNet me, integer layer, integer unit);

void FFNet_setBias (FFNet me, integer layer, integer node, double value);

void FFNet_setWeight (FFNet me, integer later, integer node, integer node_from, double value);

double FFNet_getWeight (FFNet me, integer later, integer node, integer node_from);

void FFNet_reset (FFNet me, double wrange);
/* reset the neural net:
 *   initialize all (selected) weights and biases with random numbers from the
 *   interval (-wrange, wrange).
 *   forget links with minimizer.
 */

const char32* FFNet_getCategoryOfOutputUnit (FFNet me, integer outputUnit);

integer FFNet_getOutputUnitOfCategory (FFNet me, const char32* category);

void FFNet_propagateToLayer (FFNet me, constVEC input, VEC activity, integer layer);
/* propagate the input through the net to layer and calculate the activities */

void FFNet_propagate (FFNet me, constVEC input, autoVEC *output);
/* step (1) feed forward input from "input layer" to "output layer"
 * if output != nullptr the output activity is copied into output.
 * postcondition: my activities defined
 */

double FFNet_computeError (FFNet me, constVEC target);
/* step (2) calculate error on output nodes w.r.t. desired output */
/* step (3) backpropagate this error to previous nodes */
/* precondition: step (1) */

void FFNet_computeDerivative (FFNet me);
/* step (4) compute derivative in my dwi */
/* Precondition: step (3) */

integer FFNet_getWinningUnit (FFNet me, integer labeling);
/* labeling = 1 : winner-takes-all */
/* labeling = 2 : stochastic */

void FFNet_selectAllWeights (FFNet me);

void FFNet_selectBiasesInLayer (FFNet me, integer layer);

integer FFNet_dimensionOfSearchSpace (FFNet me);
/* count the selected weights */

integer FFNet_getNumberOfWeights (FFNet me);
/* return my numberOfWeights */

void FFNet_weightConnectsUnits (FFNet me, integer index, integer *out_fromUnit, integer *out_toUnit, integer *out_layer);
/*
 * w[index] connects unit fromUnit in "layer-1" with unit toUnit in "layer".
 *  fromUnit returns 0 then w[index] is bias.
 */

integer FFNet_getNodeNumberFromUnitNumber (FFNet me, integer unit, integer layer);

integer FFNet_getNumberOfLayers (FFNet me);

integer FFNet_getNumberOfUnits (FFNet me);

integer FFNet_getNumberOfHiddenumberOfLayers (FFNet me);

integer FFNet_getNumberOfUnitsInLayer (FFNet me, integer layer);

double FFNet_getMinimum (FFNet me);

void FFNet_drawTopology (FFNet me, Graphics g);

void FFNet_drawActivation (FFNet me, Graphics g);

void FFNet_drawWeightsToLayer (FFNet me, Graphics g, integer toLayer, integer scaling, bool garnish);
/* Deprecated: the strengths of the weights that connect to the nodes in later 'layer' */
/* are drawn with boxes. The area of each box corresponds to the strength. */
/* Black boxes have negative strength? */

void FFNet_drawCostHistory (FFNet me, Graphics g, integer from_iteration, integer to_iteration,
	double from_cost, double to_cost, bool garnish);
/* draw cost vs epochs */

autoCollection FFNet_createIrisExample (integer numberOfHidden1, integer numberOfHidden2);

autoTableOfReal FFNet_extractWeights (FFNet me, integer layer);

void FFNet_drawWeights (FFNet me, Graphics g, integer layer, bool garnish);

autoFFNet FFNet_TabelOfReal_to_FFNet (FFNet me, TableOfReal him, integer layer);

autoFFNet PatternList_Categories_to_FFNet (PatternList me, Categories you, integer numberOfUnits1, integer numberOfUnits2);

#endif /* _FFNet_h_ */
