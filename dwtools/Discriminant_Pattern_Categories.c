/* Discriminant_Pattern_Categories.c
 *
 * Copyright (C) 2004 David Weenink
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
 djmw 20040422 Initial version
*/

#include "Discriminant_Pattern_Categories.h"
#include "TableOfReal.h"
#include "Matrix_Categories.h"

Discriminant Pattern_and_Categories_to_Discriminant (Pattern me, Categories thee)
{
	Discriminant him = NULL;
	TableOfReal t = Matrix_and_Categories_to_TableOfReal (me, thee);

	if (t == NULL) return NULL;
	him = TableOfReal_to_Discriminant (t);
	forget (t);
	return him;
}

Categories Discriminant_and_Pattern_to_Categories (Discriminant me, Pattern thee, 
	int poolCovarianceMatrices,int useAprioriProbabilities)
{
	ClassificationTable ct = NULL;
	Categories him = NULL;
	TableOfReal t = Matrix_to_TableOfReal (thee);

	if (t == NULL) return NULL;	
	ct = Discriminant_and_TableOfReal_to_ClassificationTable (me, t, poolCovarianceMatrices,
		useAprioriProbabilities);
	if (ct != NULL) him =  ClassificationTable_to_Categories_maximumProbability (ct);
	
	forget (ct); forget (t);
	
	return him;
}

/* End of file Discriminant_Pattern_Categories.c */
