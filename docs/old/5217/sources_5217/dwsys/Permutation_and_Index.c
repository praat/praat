/* Permutation_and_Index.c
 *
 * Copyright (C) 2005 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * djmw 20050725
 */
 

#include "Permutation_and_Index.h"

Permutation Index_to_Permutation_permuteRandomly (I, int permuteWithinClasses)
{
	iam (Index);
	Permutation classes = NULL, classesinv = NULL;	/* ordering of the classes */
	Permutation thee = NULL;
	long class, i, newpos, newindex, numberOfClasses = my classes -> size;
	long **indices = NULL;
	
	thee = Permutation_create (my numberOfElements);
	if (thee == NULL) return NULL;

	if ((classes = Permutation_create (numberOfClasses)) == NULL ||
		! Permutation_permuteRandomly_inline (classes, 0, 0) ||
		(classesinv = Permutation_invert (classes)) == NULL) goto end;
	indices = NUMlmatrix (0, numberOfClasses, 1, 4);
	if (indices == NULL) goto end;

	for (i = 1; i <= my numberOfElements; i++)
	{
		indices[my classIndex[i]][2]++; /* col 2: number of elements in class */
	}
	/* Get some other indices ready */
	for (i = 1; i <= numberOfClasses; i++)
	{
		class = classes -> p[i];
		indices[i][1] = class;
		indices[i][3] = indices[i-1][3] + indices[i-1][2]; /* col 3: index at start of class */
	}

	for (i = 1; i <= my numberOfElements; i++)
	{
		class = my classIndex[i];
		newindex = classesinv -> p[class];
		indices[newindex][4]++; /* col 4: number of elements processed for class */
		newpos = indices[newindex][3] + indices[newindex][4];
		thy p[newpos] = i;
	}
	if (permuteWithinClasses)
	{
		for (i = 1; i <= numberOfClasses; i++)
		{
			long from = indices[i][3] + 1;
			long to = from + indices[i][2] - 1;
			if (to > from && ! Permutation_permuteRandomly_inline (thee, from, to)) goto end;
		}
	}
end:
	NUMlmatrix_free (indices, 0, 1);
	forget (classes);
	forget (classesinv);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

/*  End of file Permutation_and_Index.h */
