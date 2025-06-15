/* FFNet_Pattern_Activation.c
 *
 * Copyright (C) 1994-2008 David Weenink
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
 djmw 19960826
 djmw 20020712 GPL header
 djmw 20030701 Removed non-GPL minimizations
 djmw 20040416 More precise error messages.
 djmw 20041118 Added FFNet_Pattern_Categories_getCosts.
*/

#include "Graphics.h"
#include "FFNet_Pattern_Activation.h"

static double func (I, const double p[])
{
    iam (FFNet); 
	Minimizer thee = my minimizer; 
	long i, j, k; 
	double fp = 0;

    for (j = 1, k = 1; k <= my nWeights; k++)
    {
        my dw[k] = 0.0;
        if (my wSelected[k]) my w[k] = p[j++];
    }
    for (i = 1; i <= my nPatterns; i++)
    {
    	FFNet_propagate (me, my inputPattern[i], NULL);
        fp += FFNet_computeError (me, my targetActivation[i]);
        FFNet_computeDerivative (me);
        /* derivative (cumulative) */
        for (k = 1; k <= my nWeights; k++)
		{
			my dw[k] += my dwi[k];
		}
    }
	thy funcCalls++;
    return fp;
}

static void dfunc_optimized (I, const double p[], double dp[])
{
    iam (FFNet);
	long j, k;
	(void) p; 

    for (j = 1, k = 1; k <= my nWeights; k++)
	{
    	if (my wSelected[k]) dp[j++] = my dw[k];
	}
}

static int _FFNet_Pattern_Activation_checkDimensions (FFNet me, Pattern p, Activation a)
{
    if (my nInputs != p -> nx) return Melder_error1 (L"The Pattern and the FFNet do not match.\n"
    	"The number of colums in the Pattern must equal the number of inputs in the FFNet.");
    if (my nOutputs != a -> nx) return Melder_error1 (L"The Activation and the FFNet do not match.\n"
    	"The number of colums in the Activation must equal the number of outputs in the FFNet."); 
	if (p -> ny != a -> ny) return Melder_error1 (L"The Pattern and the Activation do not match.\n"
		"The number of rows in the Pattern must equal the number of rows in the Activation.");
	if (! _Pattern_checkElements (p)) return Melder_error1 (L"The elements in the Pattern are not all "
		"in the interval [0, 1].\nThe input of the neural net can only process values that are between 0 "
			"and 1.\nYou could use a \"Formula...\" to scale the Pattern values first.");
	if (! _Activation_checkElements (a)) return Melder_error1 (L"The elements in the Activation are not "
		"all in the interval [0, 1].\nThe output of the neural net can only process values that are "
		"between 0 and 1.\nYou could use \"Formula...\" to scale the Activation values first.");
	return 1;
}

static int _FFNet_Pattern_Activation_learn (FFNet me, Pattern pattern, 
	Activation activation, long maxNumOfEpochs, double tolerance, 
	Any parameters, int costFunctionType, int reset)
{
	int status;
	
	if (! _FFNet_Pattern_Activation_checkDimensions (me, pattern, activation)) return 0;

    Minimizer_setParameters (my minimizer, parameters);
	
    /*
		Link the things to be learned
	*/
	
    my nPatterns = pattern -> ny;
    my inputPattern = pattern -> z;
    my targetActivation = activation -> z;
    FFNet_setCostFunction (me, costFunctionType);
	
    if (reset) 
    {
    	long i, k = 1;
    	double *wbuf = NUMdvector (1, my dimension);
		
    	if (wbuf == NULL) return 0;
		
    	for (i = 1; i <= my nWeights; i++)
		{
			if (my wSelected[i]) wbuf[k++] = my w[i];
		} 
    	Minimizer_reset (my minimizer, wbuf);
    	NUMdvector_free (wbuf, 1);
    }
	
    status = Minimizer_minimize (my minimizer, maxNumOfEpochs, tolerance, 1);

    /*
		Unlink
	*/
	
    my nPatterns = 0; 
	my inputPattern = NULL; 
	my targetActivation = NULL;
	
    return status;
}
  

int FFNet_Pattern_Activation_learnSD (FFNet me, Pattern p, Activation a, 
	long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType)
{
    int resetMinimizer = 0;
	/* Did we choose another minimizer */
    if (my minimizer != NULL && 
		! Thing_member (my minimizer, classSteepestDescentMinimizer))
	{
		forget (my minimizer);
		resetMinimizer = 1;
	}
	/* create the minimizer if it doesn't exist */
    if (my minimizer == NULL)
    {
    	resetMinimizer = 1;
		my minimizer = SteepestDescentMinimizer_create (my dimension, me, func,
			dfunc_optimized);
    	if (my minimizer == NULL) return 0;
    }
    return _FFNet_Pattern_Activation_learn (me, p, a, maxNumOfEpochs,
		tolerance, parameters, costFunctionType, resetMinimizer);
}

int FFNet_Pattern_Activation_learnSM (FFNet me, Pattern p, Activation a, 
	long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType)
{
    int resetMinimizer = 0;
	/*
		Did we choose another minimizer
	*/
    if (my minimizer != NULL && 
		! Thing_member (my minimizer, classVDSmagtMinimizer))
	{
		forget (my minimizer);
		resetMinimizer = 1;
	}
	/* create the minimizer if it doesn't exist */
    if (! my minimizer)
    {
    	resetMinimizer = 1;
		my minimizer = VDSmagtMinimizer_create (my dimension, me, func, 
			dfunc_optimized);
    	if (my minimizer == NULL) return 0;
    }
    return _FFNet_Pattern_Activation_learn (me, p, a, maxNumOfEpochs,
		tolerance, parameters, costFunctionType, resetMinimizer);
}

double FFNet_Pattern_Activation_getCosts_total (FFNet me, Pattern p, Activation a, int costFunctionType)
{
	double cost;
	long i;
	
	if (! _FFNet_Pattern_Activation_checkDimensions (me, p, a)) return NUMundefined;
	
    FFNet_setCostFunction (me, costFunctionType);
	
    for (cost = 0, i = 1; i <= p -> ny; i++)
    {
    	FFNet_propagate (me, p -> z[i], NULL);
        cost += FFNet_computeError (me, a -> z[i]);
	}
		
	return cost;
}

double FFNet_Pattern_Activation_getCosts_average (FFNet me, Pattern p, Activation a, int costFunctionType)
{
	double costs = FFNet_Pattern_Activation_getCosts_total (me, p, a, costFunctionType);
	return costs == NUMundefined ? NUMundefined : costs / p -> ny;
}

Activation FFNet_Pattern_to_Activation (FFNet me, Pattern p, long layer)
{
    Activation thee;
    long i, nPatterns = p -> ny;
	
    if (layer < 1 || layer > my nLayers) layer = my nLayers;
    if (my nInputs != p -> nx) return Melder_errorp5 
		(L"The Pattern and the FFNet do not match.\nThe number of colums in the Pattern (", Melder_integer (p -> nx),
    	L") must equal the number of inputs in the FFNet (", Melder_integer (my nInputs), L").");
    if (! _Pattern_checkElements (p)) return Melder_errorp1 
		(L"The elements in the Activation are not all in the interval [0, 1].\n"
		"The output units of the neural net can only process values that are between 0 and 1.\n"
		"You could use \"Formula...\" to scale the Activation values first.");
		
	thee = Activation_create (nPatterns, my nUnitsInLayer[layer]);	
    if (thee == NULL) return NULL;
	
    for (i = 1; i <= nPatterns; i++)
    {
    	FFNet_propagateToLayer (me, p -> z[i], thy z[i], layer);
    }
    return thee;
}

/* End of file FFNet_Pattern_Activation.c */
