/* FFNet_Activation_Categories.c
 *
 * Copyright (C) 1997-2002 David Weenink
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
 djmw 19960322
 djmw 20020712 GPL header
 djmw 20040416 Better error messages
*/

#include "FFNet_Activation_Categories.h"

static long winnerTakesAll (I, const float activation[])
{
	iam (FFNet); long i, pos = 1; float max = activation[1];
	for (i=2; i <= my nOutputs; i++)
	{
		if (activation[i] > max)
		{
			max = activation[i]; pos = i;
		}
	}
	return pos;
}

static long stochastic (I, const float activation[])
{
	iam (FFNet); long i; double number;
	float range = 0, lower = 0;
	for (i=1; i <= my nOutputs; i++) range += activation[i];
	number = NUMrandomUniform (0,1)*range;
	for (i=1; i <= my nOutputs; i++) if (number < (lower += activation[i])) break;
	return i;
}

Categories FFNet_Activation_to_Categories (FFNet me, Activation activation, int labeling)
{
	Categories thee = NULL, categories = my outputCategories;
	long i, (*labelingFunction) (I, const float act[]);
	
	if (! my outputCategories)
	{
		(void) Melder_error ("FFNet & Activation: To Categories\n");
		return Melder_errorp ("The neural net has no Categories (has the FFNet been trained yet?).");
	}
	if (my nOutputs != activation->nx)
	{
		(void) Melder_error ("FFNet & Activation: To Categories\n");
		return Melder_errorp ("The number of columns in the Activation must equal the number of outputs of FFNet.");
	}
	thee = Categories_create ();
	if (thee == NULL) return NULL;
	labelingFunction = labeling == 2 ? stochastic : winnerTakesAll;
	for (i = 1; i <= activation->ny; i++)
	{
		long index = labelingFunction (me, activation->z[i]);
		Data item = Data_copy (categories->item[index]);
		if (item == NULL || ! Collection_addItem (thee, item)) 
		{
			forget (thee);
			return Melder_errorp ("FFNet & Activation: To Categories\n\nError creating label %ld.", i);
		}
	}
	return thee;
}

Activation FFNet_Categories_to_Activation (FFNet me, Categories thee)
{
	Activation him = NULL; 
	Categories uniq;
	long i, nl, cSize = thy size, hasCategories = 1;
	
	uniq = Categories_selectUniqueItems (thee, 1);
	if (uniq == NULL)  Melder_error ("There is not enough memory to create a Categories."); 
	
	if (my outputCategories == NULL)
	{
		if (my nUnitsInLayer[my nLayers] == uniq -> size)
		{
			my outputCategories = uniq;
			hasCategories = 0;
		}
		else
		{
			(void) Melder_error ("");
			goto end;
		}
	}
	else if (! ( (nl = OrderedOfString_isSubsetOf (uniq, my outputCategories, NULL)) &&
		nl == uniq -> size && my nOutputs >= uniq -> size))
	{
		 (void) Melder_error ("The Categories do not match the categories of the FFNet.");
		goto end;
	}
	
	him = Activation_create (cSize, my nOutputs);
	if (him == NULL) goto end;
	for (i=1; i <= cSize; i++)
	{
		long pos =  OrderedOfString_indexOfItem_c (my outputCategories, OrderedOfString_itemAtIndex_c (thee, i));
		if (pos < 1)
		{
			 (void) Melder_error ("The FFNet doesn't know the category \"%ls\" from Categories.", 
			 	OrderedOfString_itemAtIndex_c (thee, i));
				goto end;
		}
		his z[i][pos] = 1.0;
	}
	
end:

	if (hasCategories) forget (uniq);
	if (Melder_hasError ()) forget (him);
	
	return him;
}

/* End of file FFNet_Activation_Categories.c */
