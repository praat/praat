/* FFNet.c
 *
 * Copyright (C) 1997-2007 David Weenink
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
 djmw 20020712 GPL header
 djmw 20040420 Modified FFNet_create and FFNet_init parameters.
 djmw 20040422 FFNet_drawActivation: nodes with activity > 0.05 had incorrect size.
 djmw 20040422 FFNet_extractWeights added.
 djmw 20040425 FFNet_drawTopology fill input units; increase distance from arrow for output labels
 djmw 20040513 Info changes. 
 djmw 20040526 Adapted FFNet_drawCostHistory.
 djmw 20050131 Reversed sign of derivative in minimumCrossEntropy.
 djmw 20060811 Changed %d to %ld in sprintf for longs.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20070902 FFNet_createNameFromTopology to wchar_t
 djmw 20071014 Melder_error<n>
*/

#include "FFNet_Matrix.h"
#include "Matrix_extensions.h"
#include "TableOfReal_extensions.h"
#include "Pattern.h"
#include "Collection.h"
#include "Categories.h"

static int bookkeeping (FFNet me);

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

static int FFNet_checkLayerNumber (FFNet me, long layer)
{
	if (layer < 1 || layer > my nLayers)
	{
		if (layer == 0)
			(void) Melder_error1 (L"A Layer number of 0 is not allowed.");
		else if (layer < 0)
			(void) Melder_error1 (L"A negative layer number is not allowed.");
		else if (layer > my nLayers)
			(void) Melder_error3 (L"A layer number of ", Melder_integer (layer), L" is too big.");
			
		(void) Melder_error4 (L"This FFNet has ", Melder_integer (layer), L" layer", (my nLayers > 1 ? L"s\n" : L"\n"));
		if (my nLayers == 1)
			return Melder_error1 (L"Please set the layer number equal to 1.");
		else if (my nLayers == 2)
			return Melder_error1 (L"Please choose a layer number equal to 1 or 2.");
		else if (my nLayers == 3)
			return Melder_error1 (L"Please choose a layer number equal to 1, 2 or 3.");
		else
			return Melder_error2 (L"Please choose a layer number in the range 1 to ", Melder_integer (my nLayers));
	}
	return 1;
}

void FFNet_createNameFromTopology (FFNet me, MelderString *name)
{
	MelderString_append1 (name, Melder_integer (my nUnitsInLayer[0]));
	for (long i = 1; i <= my nLayers; i++)
	{
		MelderString_append2 (name, L"-", Melder_integer (my nUnitsInLayer[i]));
	}
}

/****** non-linearities ****************************************************/

static double sigmoid (I, double x, double *deriv)
{
    double act = NUMsigmoid (x);
	(void) void_me;
    *deriv = act * ( 1.0 - act);
    return act;
}


/* ******************* cost functions ****************************************/

/*
	For the errors calculated in the cost functions:
		if target > activity ==> error > 0
		if target < activity ==> error < 0
*/

static double minimumSquaredError (I, const double target[])
{
    iam (FFNet); 
	long i, k = my nNodes - my nOutputs + 1;
    double cost = 0.0;
    
    for (i = 1; i <= my nOutputs; i++, k++)
    {
		double e = my error[k] = target[i] - my activity[k];
		cost += e * e;
    }
    return 0.5 * cost;
}

/* E = - sum (i=1; i=nPatterns; sum (k=1;k=nOutputs; t[k]*ln (o[k]) + (1-t[k])ln (1-o[k]))) */
/* dE/do[k] = -(1-t[k])/ (1-o[k]) + t[k]/o[k] */
/* werkt niet bij (grote?) netten */
static double minimumCrossEntropy (I, const double target[])
{
    iam (FFNet); 
	long i, k = my nNodes - my nOutputs + 1;
    double cost = 0.0;
    
    for (i = 1; i <= my nOutputs; i++, k++)
    {
    	double t1 = 1.0 - target[i];
		double o1 = 1.0 - my activity[k];
		
   		cost -= target[i] * log (my activity[k]) + t1 * log (o1);
		my error[k] = -t1 / o1 + target[i] / my activity[k];
    }
    return cost;
}


/* *********************************************************************** */

static int bookkeeping (FFNet me)
{
    long j, i, n = my nUnitsInLayer[0] + 2, nWeights = 0;
    long firstNodeInPrevious = 1, lastWeightInPrevious = 0;
    
    my nNodes = my nUnitsInLayer[0];
    for (i = 1; i <= my nLayers; i++)
    {
		my nNodes += my nUnitsInLayer[i] + 1;
        nWeights += my nUnitsInLayer[i] * (my nUnitsInLayer[i - 1] + 1);
    }
    if (my nWeights > 0 && my nWeights != nWeights) return 0;
    my nWeights = nWeights;
    /* FFNet_readText already fills my w */ 
	if ( (! my w && ! (my w = NUMdvector (1, my nWeights))) ||
        ! (my activity     = NUMdvector (1, my nNodes)) ||
        ! (my isbias       = NUMlvector (1, my nNodes)) ||
        ! (my nodeFirst    = NUMlvector (1, my nNodes)) ||
        ! (my nodeLast     = NUMlvector (1, my nNodes)) ||
        ! (my wFirst       = NUMlvector (1, my nNodes)) ||
        ! (my wLast        = NUMlvector (1, my nNodes)) ||
        ! (my wSelected    = NUMlvector (1, my nWeights)) ||
        ! (my error        = NUMdvector (1, my nNodes)) ||
        ! (my deriv  	   = NUMdvector (1, my nNodes)) ||
        ! (my dwi          = NUMdvector (1, my nWeights)) ||
        ! (my dw           = NUMdvector (1, my nWeights))) return 0;
	my nInputs = my nUnitsInLayer[0];
	my nOutputs = my nUnitsInLayer[my nLayers];
    my isbias[my nInputs + 1] = my activity[my nInputs + 1] = 1;
    for (j=1; j <= my nLayers; j++,  n++)
    {
        for (i=1; i <= my nUnitsInLayer[j]; i++, n++)
        {
            my isbias[n] = 0;
            my nodeFirst[n] = firstNodeInPrevious;
            my nodeLast[n] = my nodeFirst[n] + my nUnitsInLayer[j-1];
            my wFirst[n] = lastWeightInPrevious + (i-1)* (my nUnitsInLayer[j-1] + 1) + 1;
            my wLast[n] = my wFirst[n] + my nUnitsInLayer[j-1];
        }
        if (j != my nLayers) my isbias[n] = my activity[n] = 1;
        lastWeightInPrevious = my wLast[n-1];
        firstNodeInPrevious += my nUnitsInLayer[j-1] + 1;
     }
     FFNet_selectAllWeights (me);
     return 1;
}

static void info (I)
{
	iam (FFNet);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of layers: ", Melder_integer (my nLayers));
	MelderInfo_writeLine2 (L"Total number of units: ", Melder_integer (FFNet_getNumberOfUnits (me)));
	MelderInfo_writeLine4 (L"   Number of units in layer ", Melder_integer (my nLayers), L" (output): ",
		Melder_integer (my nUnitsInLayer[my nLayers]));
	for (int i = my nLayers-1; i >= 1; i--)
	{
		MelderInfo_writeLine4 (L"   Number of units in layer ", Melder_integer (i), L" (hidden): ",
			Melder_integer (my nUnitsInLayer[i]));
	}
	MelderInfo_writeLine2 (L"   Number of units in layer 0 (input): ", Melder_integer (my nUnitsInLayer[0]));
	MelderInfo_writeLine2 (L"Outputs are linear: ", Melder_boolean (my outputsAreLinear));
	MelderInfo_writeLine5 (L"Number of weights: ", Melder_integer (my nWeights), L" (", 
		Melder_integer (FFNet_dimensionOfSearchSpace (me)), L" selected)");
	MelderInfo_writeLine2 (L"Number of nodes: ", Melder_integer (my nNodes));
} 


class_methods (FFNet, Data)
	class_method_local (FFNet, destroy)
	class_method_local (FFNet, copy)
	class_method_local (FFNet, equal)
	class_method_local (FFNet, canWriteAsEncoding)
	class_method_local (FFNet, writeText)
	class_method_local (FFNet, writeBinary)
	class_method_local (FFNet, readText)
	class_method_local (FFNet, readBinary)
	class_method_local (FFNet, description)
    class_method (info)
class_methods_end


int FFNet_init (FFNet me, long numberOfInputs, long nodesInLayer1, long nodesInLayer2, 
	long numberOfOutputs, int outputsAreLinear)
{
	long numberOfLayers = 3;
	
	if (numberOfInputs < 1 || numberOfOutputs < 1) return 0;
	if (nodesInLayer1 < 1) numberOfLayers--;
	if (nodesInLayer2 < 1) numberOfLayers--;
	my nLayers = numberOfLayers;
	my nUnitsInLayer = NUMlvector (0, numberOfLayers);
	if (my nUnitsInLayer == NULL) return 0;
	
	my nUnitsInLayer[numberOfLayers--] = numberOfOutputs;
	if (nodesInLayer2 > 0) my nUnitsInLayer[numberOfLayers--] = nodesInLayer2;
	if (nodesInLayer1 > 0) my nUnitsInLayer[numberOfLayers--] = nodesInLayer1;
	my nUnitsInLayer[numberOfLayers] = numberOfInputs;
	Melder_assert (numberOfLayers == 0);
	my outputsAreLinear = outputsAreLinear;
	
	if (! bookkeeping (me)) return 0;
	
	FFNet_setCostFunction (me, FFNet_COST_MSE);
	FFNet_setNonLinearity (me, FFNet_NONLIN_SIGMOID);
    FFNet_reset (me, 0.1);
	
	return 1;
}

int FFNet_setOutputCategories (FFNet me, Categories thee)
{
	Categories uniq = Categories_selectUniqueItems (thee, 1);
	
	if (uniq -> size == thy size)
	{
		my outputCategories = uniq;
		return 1;
	}
	forget (uniq);
	return 0;
}

FFNet FFNet_create (long numberOfInputs, long numberInLayer1, long numberInLayer2, 
	long numberOfOutputs, int outputsAreLinear)
{
    FFNet me = new (FFNet);
	if (me == NULL || ! FFNet_init (me, numberOfInputs, numberInLayer1, numberInLayer2, 
		numberOfOutputs, outputsAreLinear)) forget (me);
	return me;
}

void FFNet_setNonLinearity (FFNet me, int nonLinearityType)
{
	my nonLinearityType = nonLinearityType;
	my nonLinearity = sigmoid;
	my nlClosure = NULL;
}

void FFNet_setCostFunction (FFNet me, int costType)
{
	my costFunctionType = costType;
	if (costType == 2) my costFunction = minimumCrossEntropy;
	else my costFunction = minimumSquaredError;
	my cfClosure = NULL;
}
       
void FFNet_reset (FFNet me, double wrange)
{
    long i;
    for (i = 1; i <= my nWeights; i++)
        if (my wSelected[i]) my w[i] = NUMrandomUniform (-wrange, wrange);
    for (i = 1; i <= my nNodes; i++)
        my activity[i] = (my isbias[i] ? 1.0 : 0.0);
    my accumulatedCost = 0.0;
    forget (my minimizer);  
}


/***** OPERATION: ***********************************************************/
/* step 1 */
void FFNet_propagate (FFNet me, const double input[], double output[])
{
    double act; long i, j, k = 1;
    long nNodes = my outputsAreLinear ? my nNodes - my nOutputs : my nNodes;

	/* clamp input pattern on the network */
	for (i=1; i <= my nUnitsInLayer[0]; i++) my activity[i] = input[i];
    	
    /* on hidden units use activation function */
    for (i=my nUnitsInLayer[0]+2; i <= nNodes; i++)
    {
        if (my isbias[i]) continue;
        for (act=0.0, j=my nodeFirst[i]; j <= my nodeLast[i]; j++, k++)
            act += my w[k] * my activity[j];
        my activity[i] = my nonLinearity (me, act, & my deriv[i]);
    }
    /* on output units use another activation function */
    if (my outputsAreLinear)
    {
    	for (i=nNodes+1; i <= my nNodes; i++)  
    	{
        	if (my isbias[i]) continue;
        	for (act=0.0, j=my nodeFirst[i]; j <= my nodeLast[i]; j++, k++)
            	act += my w[k] * my activity[j];
			my activity[i] = act;
			my deriv[i] = 1.0;
    	}
    }
    if (output) for (k=my nNodes-my nOutputs+1, i=1; i <= my nOutputs; i++, k++)
    	output[i] = my activity[k];  
}


double FFNet_computeError (FFNet me, const double target[])
{
	long i, j, k; double cost;
	
	/* compute error at output layer */
	cost = my costFunction (me, target);
    for (i=1; i <= my nNodes - my nOutputs; i++) my error[i] = 0.0;
    /* backpropagation of errors from output to first hidden layer*/
    for (i=my nNodes; i > my nInputs+1; i--)
    {
        if (my isbias[i]) continue;
        my error[i] *= my deriv[i];
        if (my nodeFirst[i] > my nInputs + 1)
        {
            for (k=my wFirst[i], j=my nodeFirst[i]; j <= my nodeLast[i]-1; j++, k++)
	        	my error[j] += my error[i] * my w[k];
        }
    }
    return cost;
}

void FFNet_computeDerivative (FFNet me)
{
    long i, j, k = 1;
    for (i = my nInputs+2; i <= my nNodes; i++)
    {
        if (! my isbias[i])
		{
			for (j = my nodeFirst[i]; j <= my nodeLast[i]; j++, k++)
			{
            	my dwi[k] = - my error[i] * my activity[j];
			}
		}
    }
}

/******* end operation ******************************************************/

long FFNet_getWinningUnit (FFNet me, int labeling)
{
    long i, pos = 1, k = my nNodes - my nOutputs;
	if (labeling == 2) /* stochastic */
	{
		double sum = 0, random;
		for (i=1; i <= my nOutputs; i++) sum += my activity[k+i];
		random = NUMrandomUniform (0, sum);
		for (pos=my nOutputs; pos >= 2; pos--) if (random > (sum -= my activity[k+pos])) break;
	}
	else /* winner-takes-all */
	{
		double max = my activity[k+1];
		for (i=2; i <= my nOutputs; i++) if (my activity[k+i] > max)
		{ max = my activity[k+i]; pos = i; }
	}
	return pos;
}

void FFNet_propagateToLayer (FFNet me, const double input[], double activity[], long layer)
{
	long i, k = 0;
	Melder_assert (activity);
	FFNet_propagate (me, input, NULL);
	for (i=0; i < layer; i++) k += my nUnitsInLayer[i] + 1;
	for (i=1; i <= my nUnitsInLayer[layer]; i++) activity[i] = my activity[k+i];
}

void FFNet_selectAllWeights (FFNet me)
{
	long i;
	for (i=1; i <= my nWeights; i++) my wSelected[i] = 1;
	my dimension = my nWeights;
}

long FFNet_dimensionOfSearchSpace (FFNet me)
{
	long i, n = 0;
	for (i = 1; i <= my nWeights; i++)
	{
		if (my wSelected[i]) n++;
	}
	return n;
}

void FFNet_selectBiasesInLayer (FFNet me, long layer)
{
	long i, node = my nUnitsInLayer[0]+1;
	if (layer < 1 || layer > my nLayers) return;
	for (i=1; i <= my nWeights; i++) my wSelected[i] = 0;
	for (i=1; i < layer; i++) node += my nUnitsInLayer[i] + 1;
	for (i=node+1; i <= node+my nUnitsInLayer[layer]; i++)
		my wSelected[my wLast[i]] = 1;
	my dimension = my nUnitsInLayer[layer];
}

void FFNet_weightConnectsUnits (FFNet me, long index, long *fromUnit, long *toUnit, 
    long *layer)
{
    long i = 1, np = 0, nw = my nUnitsInLayer[1] * (my nInputs + 1);

    Melder_assert (index > 0 && index <= my nWeights);
    while (index > nw)
    {
		 i++;
		 nw += ( np = my nUnitsInLayer[i] * (my nUnitsInLayer[i-1] + 1));
	}
    if (i > 1) index -= nw - np;
    *fromUnit = index % (my nUnitsInLayer[i-1] + 1);
    *toUnit = (index - 1) / (my nUnitsInLayer[i-1] + 1) + 1;
    *layer = i;
}

long FFNet_getNodeNumberFromUnitNumber (FFNet me, long unit, long layer)
{
	long i, node = unit;
	
	if (layer < 0 || layer > my nLayers || unit > my nUnitsInLayer[layer]) return -1;
	
	for (i = 0; i < layer; i++)
	{
		node += my nUnitsInLayer[i] + 1;
	}
	return node;
}

void FFNet_nodeToUnitInLayer (FFNet me, long node, long *unit, long *layer)
{
    long i = 0, nn = my nUnitsInLayer[0] + 1;
    Melder_assert (node > 0 && node <= my nNodes);
    while (node > nn) { nn += my nUnitsInLayer[++i] + 1; }
    if (i > 0) node -= nn - (my nUnitsInLayer[i] + 1);
    *unit = node % (my nUnitsInLayer[i] + 1);
    *layer = i;
}

long FFNet_getNumberOfWeights (FFNet me) { return my nWeights; }

long FFNet_getNumberOfLayers (FFNet me) {  return my nLayers; }

long FFNet_getNumberOfUnits (FFNet me) { return my nNodes - my nLayers; }

long FFNet_getNumberOfHiddenLayers (FFNet me) { return my nLayers - 1; }

long FFNet_getNumberOfUnitsInLayer (FFNet me, int layer)
{
    if (layer > my nLayers || layer < 0) return 0;
    return my nUnitsInLayer[layer];  
}

double FFNet_getMinimum (FFNet me)
{
	return my minimizer ? Minimizer_getMinimum (my minimizer) : NUMundefined;
}

void FFNet_drawTopology (FFNet me, Graphics g)
{
    long i, maxNumOfUnits = my nUnitsInLayer[0];
    int dxIsFixed = 1;
    double radius, dx, dy = 1.0 / (my nLayers + 1);

    for (i = 1; i <= my nLayers; i++)
	{
		if (my nUnitsInLayer[i] > maxNumOfUnits) maxNumOfUnits = my nUnitsInLayer[i];
	}
    dx = 1.0 / maxNumOfUnits;
    radius = dx / 10;
    Graphics_setInner (g);
    Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0); 
    for (i = 0; i <= my nLayers; i++)
    {
		long j, k;
		double dx2 = dx, x2WC, y2WC = dy / 2 + i * dy;
		double x2 = (maxNumOfUnits - my nUnitsInLayer[i] + 1) * dx2 / 2;
		/* draw the units */
		if (! dxIsFixed)
		{
			dx2 = 1.0 / my nUnitsInLayer[i];
			x2 = dx2 / 2;
		}
		if (i == 0)
		{
	    	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
	    	for (x2WC = x2, j = 1; j <= my nInputs; j++)
	    	{
				Graphics_arrow (g, x2WC, y2WC - radius - dy / 4, x2WC, y2WC - radius);
				x2WC += dx2;
	    	}
		}  
		Graphics_setColour (g, Graphics_RED);
		for (x2WC = x2, j = 1; j <= my nUnitsInLayer[i]; j++)
		{
	   	 	Graphics_circle (g, x2WC, y2WC, radius);
			if (i > 0) Graphics_fillCircle (g, x2WC, y2WC, radius);
			x2WC += dx2;
		}
		Graphics_setColour (g, Graphics_BLACK);
		if (i > 0)
		{
	    	double dx1 = dx;
			double x1 = (maxNumOfUnits - my nUnitsInLayer[i - 1] + 1) * dx1 / 2;
	   		double y1WC = y2WC - dy;
	    	if (! dxIsFixed)
			{
				dx1 = 1.0 / my nUnitsInLayer[i - 1];
				x1 = dx1 / 2;
			}
	    	x2WC = x2;	
	    	for (j = 1; j <= my nUnitsInLayer[i]; j++)
	    	{
				double x1WC = x1;
				for (k = 1; k <= my nUnitsInLayer[i - 1]; k++)
				{
		    		double xd = x2WC - x1WC;
		    		double cosa = xd / sqrt (xd * xd + dy * dy);
		    		double sina = dy / sqrt (xd * xd + dy * dy);
		    		Graphics_line (g, x1WC + radius * cosa, y1WC + radius * sina, x2WC - radius * cosa, y2WC - radius * sina);
		    		x1WC += dx1;
				}
				x2WC += dx2;
	    	}
		}
		if (i == my nLayers)
		{
	    	x2WC = x2;
	    	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	    	for (j = 1; j <= my nOutputs; j++)
	    	{
				Graphics_arrow (g, x2WC, y2WC + radius, x2WC, y2WC + radius + dy / 4);
				if (my outputCategories) Categories_drawItem (my outputCategories, g, j, x2WC, y2WC + radius + dy / 4); 
				x2WC += dx2;
	    	}
		}
    }
    Graphics_unsetInner (g);
}

void FFNet_drawActivation (FFNet me, Graphics g)
{
    long i, j, node = 1, maxNumOfUnits = my nUnitsInLayer[0];
    int dxIsFixed = 1, colour = Graphics_inqColour (g);
    double r1, dx, dy = 1.0 / (my nLayers + 1);

    Graphics_setInner (g);
    Graphics_setWindow (g, 0.0, 1.0, 0.0, 1.0); 
    for (i = 1; i <= my nLayers; i++)
	{
		if (my nUnitsInLayer[i] > maxNumOfUnits) maxNumOfUnits = my nUnitsInLayer[i];
	}
    dx = 1.0 / maxNumOfUnits;
    r1 = dx / 2; /* May touch when neighbouring activities are both 1 (very rare). */
	Graphics_setColour (g, colour);
    for (i = 0; i <= my nLayers; i++, node++)
    {
		double dx2 = dx, x2WC, y2WC = dy / 2 + i * dy;
		double x2 = (maxNumOfUnits - my nUnitsInLayer[i] + 1) * dx2 / 2;
		if (! dxIsFixed)
		{
			dx2 = 1.0 / my nUnitsInLayer[i];
			x2 = dx2 / 2;
		}
		x2WC = x2;
		for (j = 1; j <= my nUnitsInLayer[i]; j++, node++)
		{
	    	double activity = my activity[node];
	    	double radius = r1 * (fabs (activity) < 0.05 ? 0.05 : fabs (activity));
	    	/*Graphics_setColour (g, activity < 0 ? Graphics_BLACK : Graphics_RED);*/
	    	Graphics_circle (g, x2WC, y2WC, radius);
	    	if (activity < 0) Graphics_fillCircle (g, x2WC, y2WC, radius);
	    	x2WC += dx2;
		}
    }
    Graphics_setColour (g, Graphics_BLACK);
    Graphics_unsetInner (g);
}

/* This routine is deprecated since praat-4.2.4 20040422 and will be removed in the future. */
void FFNet_drawWeightsToLayer (FFNet me, Graphics g, int layer, int scaling, int garnish)
{
    Matrix weights;
    
    if (layer < 1 || layer > my nLayers || 
    	! (weights = FFNet_weightsToMatrix (me, layer, 0))) return;
    Matrix_scale (weights, scaling);
    Matrix_drawAsSquares (weights, g, 0, 0, 0, 0, 0);
    if (garnish)
    {
    	double x1WC, x2WC, y1WC, y2WC; wchar_t text[30];
		Graphics_inqWindow (g, & x1WC, & x2WC, & y1WC, & y2WC);
		swprintf (text, 30, L"Units in layer %ld ->", layer);
		Graphics_textBottom (g, 0, text);
		if (layer == 1) wcscpy (text, L"Input units ->");
		else swprintf (text, 30, L"Units in layer %ld ->", layer-1);
		Graphics_textLeft (g, 0, text);
		/* how do I find out the current settings ??? */
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		Graphics_setInner (g);
		Graphics_text (g, 0.5, weights->ny, L"bias");
		Graphics_unsetInner (g);
    }
    forget (weights);
}

void FFNet_drawWeights (FFNet me, Graphics g, long layer, int garnish)
{
    TableOfReal thee = FFNet_extractWeights (me, layer);
    
	if (thee == NULL) return;
	TableOfReal_drawAsSquares (thee, g, 1, thy numberOfRows, 1, thy numberOfColumns, garnish);

    forget (thee);
}

void FFNet_drawCostHistory (FFNet me, Graphics g, long iFrom, long iTo, 
    double costMin, double costMax, int garnish)
{
    if (my minimizer) Minimizer_drawHistory (my minimizer, g, iFrom, iTo,
		costMin, costMax, 0);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, 1, my costFunctionType == FFNet_COST_MSE ?
			L"Minimum squared error" : L"Minimum cross entropy");
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textBottom (g, 1, L"Number of epochs");
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}
}

Collection FFNet_createIrisExample (long numberOfHidden1, long numberOfHidden2)
{
	TableOfReal iris = NULL;
	Collection c = NULL;
	FFNet me = NULL;
	Pattern thee = NULL;
	Categories him = NULL, uniq = NULL;
	long i, j;
	MelderString ffnetname = { 0 };
	
	if (! (c = Collection_create (classData, 3)) ||
		! (uniq = Categories_sequentialNumbers (3)) ||
		! (me = FFNet_create (4, numberOfHidden1, numberOfHidden2, 3, 0)) ||
		! FFNet_setOutputCategories (me, uniq) ||
		! Collection_addItem (c, me) ||
		! (iris = TableOfReal_createIrisDataset ()) ||
		! TableOfReal_to_Pattern_and_Categories (iris, 0, 0, 0, 0, &thee, &him) ||
		! Collection_addItem (c, thee) ||
		! Collection_addItem (c, him)) goto end;
		
	/*
		Scale data to interval [0-1]
	*/
	for (i = 1; i <= 150; i++)
	{
		for (j = 1; j <= 4; j++) thy z[i][j] /= 10.0;
	}
	FFNet_createNameFromTopology (me, &ffnetname);
	Thing_setName (me, ffnetname.string);
	Thing_setName (thee, L"iris");
	Thing_setName (him, L"iris");
	MelderString_free (&ffnetname);
end:
	forget (uniq); forget (iris);
	if (! Melder_hasError()) return c;
	forget (c);
	return NULL;
}

TableOfReal FFNet_extractWeights (FFNet me, long layer)
{
	TableOfReal thee = NULL;
	long i, node = 1, numberOfUnitsFrom, numberOfUnitsTo;
	wchar_t label[20];
	
	if (! FFNet_checkLayerNumber (me, layer)) return NULL;
	
	numberOfUnitsFrom = my nUnitsInLayer[layer-1] + 1;
	numberOfUnitsTo = my nUnitsInLayer[layer];
	thee = TableOfReal_create (numberOfUnitsFrom, numberOfUnitsTo);
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= numberOfUnitsFrom - 1; i++)
	{
		swprintf (label,20,L"L%ld-%ld", layer-1, i);
		TableOfReal_setRowLabel (thee, i, label);	
	}
	TableOfReal_setRowLabel (thee, numberOfUnitsFrom, L"Bias");
	for (i = 1; i <= numberOfUnitsTo; i++)
	{
		swprintf (label,20,L"L%ld-%ld", layer, i);
		TableOfReal_setColumnLabel (thee, i, label);	
	}
	
	for (i = 0; i < layer; i++) node += my nUnitsInLayer[i] + 1;
	for (i = 1; i <= numberOfUnitsTo; i++, node++)
	{
		long j, k = 1;
		for (j = my wFirst[node]; j <= my wLast[node]; j++)
		{
			thy data[k++][i] = my w[j];
		}
	}
	return thee;	
}

FFNet FFNet_and_TabelOfReal_to_FFNet (FFNet me, TableOfReal him, long layer)
{
	FFNet thee = NULL; long i, node = 1;
	
	if (! FFNet_checkLayerNumber (me, layer)) return NULL;
	if ((my nUnitsInLayer[layer] != his numberOfColumns) || 
		(my nUnitsInLayer[layer] == his numberOfColumns && my nUnitsInLayer[layer-1]+1 == his numberOfRows))
	{
		long i, try[3], rows[3], cols[3], ntry = my nLayers > 3 ? 3 : my nLayers, ok = 0;
		if (my nLayers > 3) return Melder_errorp1 (L"Dimensions don't fit.");
		for (i = 1; i <= ntry; i++)
		{
			cols[i] = my nUnitsInLayer[i] == his numberOfColumns;
			rows[i] = my nUnitsInLayer[i - 1] + 1 == his numberOfRows;
			try[i] = rows[i] && cols[i];
			if (try[i]) ok ++;
		}
		if (! rows[layer])
			(void)  Melder_error3 (L"The number of rows in the TableOfReal does not equal \n"
				"the number of units in the layer that connect to layer ", Melder_integer (layer), L".\n");
		else
			(void)  Melder_error3 (L"The number of columns in the TableOfReal does not equal \n"
				"the number of units in layer ", Melder_integer (layer), L".\n");
		if (ok == 0)
			return Melder_errorp1 (L"Please quit, there is no appropriate layer in the FFNet for this TableOfReal.");
		else
		{
			if (ok == 1)
				return Melder_errorp3 (L"Please try again with layer number ", 
					Melder_integer (try[1] ? try[1] : (try[2] ? try[2] : try[3])), L".");
			else
				return Melder_errorp1 (L"Please try again with one of the other two layer numbers.");
		}
	}
	thee = Data_copy (me);
	if (thee == NULL) return NULL;
	for (i = 0; i < layer; i++)
	{
		node += thy nUnitsInLayer[i] + 1;
	}
	for (i = 1; i <= thy nUnitsInLayer[layer]; i++, node++)
	{
		long j, k = 1;
		for (j = thy wFirst[node]; j <= thy wLast[node]; j++, k++)
		{
			 thy w[j] = his data[k][i];
		}
	}
	return thee;
}

/* End of file FFNet.c */
