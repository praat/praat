/* TableOfReal_and_Permutation.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20050708
*/

#include "TableOfReal_and_Permutation.h"
#include "TableOfReal_extensions.h"
#include "NUM2.h"

TableOfReal TableOfReal_and_Permutation_permuteRows (I, Permutation thee)
{
	iam (TableOfReal);
	long i;
	TableOfReal him;

	if (my numberOfRows != thy numberOfElements) return Melder_errorp1 (L"TableOfReal_and_Permutation_permuteRows: "
		"The number of rows in the table and the number of elements in the Permutation must be equal.");
	him = TableOfReal_create (my numberOfRows, my numberOfColumns);
	if (him == NULL) return NULL;
	
	for (i = 1; i <= thy numberOfElements; i++)
	{
		if (! TableOfReal_copyOneRowWithLabel (me, him, thy p[i], i)) break;
	}
	if (Melder_hasError ()) forget (him);
	return him;
}

Permutation TableOfReal_to_Permutation_sortRowLabels (I)
{
	iam (TableOfReal);
	Permutation thee;
		
	thee = Permutation_create (my numberOfRows);
	if (thee != NULL)
	{
		NUMindexx_s (my rowLabels, my numberOfRows, thy p);
	}
	return thee;
}

/* End of file TableOfReal_and_Permutation.c */
