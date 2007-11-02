/* FFNet_Pattern_Categories.c
 *
 * Copyright (C) 1994-2003 David Weenink
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

static int _FFNet_Pattern_Categories_checkDimensions (FFNet me, Pattern p, Categories c)
{
    if (my nInputs != p -> nx) return Melder_error1 (L"The Pattern and the FFNet do not match.\n"
    	"The number of colums in the Pattern must equal the number of inputs in the FFNet.");
	if (p -> ny != c -> size) return Melder_error1 (L"The Pattern and the categories do not match.\n"
		"The number of rows in the Pattern must equal the number of categories.");
	if (! _Pattern_checkElements (p)) return Melder_error1 (L"The elements in the Pattern are not all "
		"in the interval [0, 1].\nThe input of the neural net can only process values that are between 0 "
			"and 1.\nYou could use \"Formula...\" to scale the Pattern values first.");
	return 1;

}

static int _FFNet_Pattern_Categories_learn (FFNet me, Pattern p, Categories c, 
	long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType,
	int (*learn) (FFNet, Pattern, Activation, long, double, Any, int))
{
    Activation activation; 
	int status; 
	double min, max;
    
	if (! _FFNet_Pattern_Categories_checkDimensions (me, p, c)) return 0;
	
    if (! (activation = FFNet_Categories_to_Activation (me, c))) return 0;
	Matrix_getWindowExtrema (p, 0, 0, 0, 0, &min, &max);
    status = learn (me, p, activation, maxNumOfEpochs, tolerance, parameters,
		costFunctionType);
	forget (activation);
	return status;
}

double FFNet_Pattern_Categories_getCosts_total (FFNet me, Pattern p, Categories c, int costFunctionType)
{
    Activation activation;
	double cost;
	
	if (! _FFNet_Pattern_Categories_checkDimensions (me, p, c)) return NUMundefined;
	
    if (! (activation = FFNet_Categories_to_Activation (me, c))) return NUMundefined;
	cost = FFNet_Pattern_Activation_getCosts_total (me, p, activation, costFunctionType);
	forget (activation);
	return cost;
}

double FFNet_Pattern_Categories_getCosts_average (FFNet me, Pattern p, Categories c, int costFunctionType)
{
	double costs = FFNet_Pattern_Categories_getCosts_total (me, p, c, costFunctionType);
	return costs == NUMundefined ? NUMundefined : costs / p -> ny;
}

int FFNet_Pattern_Categories_learnSM (FFNet me, Pattern p, Categories c, 
	long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType)
{
	return _FFNet_Pattern_Categories_learn (me, p, c, maxNumOfEpochs,
    	tolerance, parameters, costFunctionType,
		FFNet_Pattern_Activation_learnSM);
}

int FFNet_Pattern_Categories_learnSD (FFNet me, Pattern p, Categories c, 
	long maxNumOfEpochs, double tolerance, Any parameters, int costFunctionType)
{
	return _FFNet_Pattern_Categories_learn (me, p, c, maxNumOfEpochs,
    	tolerance, parameters, costFunctionType, 
		FFNet_Pattern_Activation_learnSD);
}

Categories FFNet_Pattern_to_Categories (FFNet me, Pattern thee, int labeling)
{
    Categories him = NULL; 
	long k, index;
	Data item;
    if (! my outputCategories) return Melder_errorp1 (L"The FFNet has no output categories.");
    if (my nInputs != thy nx) return Melder_errorp5 
		(L"The Pattern and the FFNet do not match.\nThe number of colums in the Pattern (", Melder_integer (thy nx),
    	L") must equal the number of inputs in the FFNet (", Melder_integer (my nInputs), L").");
    if (! _Pattern_checkElements (thee)) return Melder_errorp1 
		(L"The elements in the Pattern are not all in the interval [0, 1].\n"
		"The input of the neural net can only process values that are between 0 and 1.\n"
		"You could use \"Formula...\" to scale the Pattern values first.");

		
	him = Categories_create ();	
	if (him == NULL) return NULL;
	
    for (k = 1; k <= thy ny; k++)
    {
     	FFNet_propagate (me, thy z[k], NULL);
    	index = FFNet_getWinningUnit (me, labeling);
		item = Data_copy (my outputCategories->item[index]);
    	if (item == NULL || ! Collection_addItem (him, item))
		{
			forget (him); return NULL;
		}
	}
    return him;
}

/* End of file FFNet_Pattern_Categories.c */
