/* ClassificationTable.c
 * 
 * Copyright (C) 1993-2007 David Weenink
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
 djmw 1998
 djmw 20020315 GPL header
 djmw 20040422 Added ClassificationTable_to_Categories_maximumProbability
 djmw 20040623 Added ClassificationTable_to_Strings_maximumProbability
 djmw 20040824 Added Strings_extensions.h header
*/

#include "ClassificationTable.h"
#include "Strings_extensions.h"
#include "NUM2.h"

class_methods (ClassificationTable, TableOfReal)
class_methods_end

ClassificationTable ClassificationTable_create (long numberOfRows, long numberOfGroups)
{
	ClassificationTable me = new (ClassificationTable);
	
	if (! me || ! TableOfReal_init (me, numberOfRows, numberOfGroups)) forget (me);
	return me;
}

Confusion ClassificationTable_to_Confusion (ClassificationTable me)
{
	Confusion thee = NULL; 
	Categories c1 = NULL, c2 = NULL;
	
	c1 = TableOfReal_to_CategoriesRow (me);
	if (c1 == NULL) return NULL;
	
	c2 = ClassificationTable_to_Categories_maximumProbability (me);
	if (c2 != NULL) thee = Categories_to_Confusion (c1, c2);
	
	forget (c1); forget (c2);
	
	return thee;
}

Strings ClassificationTable_to_Strings_maximumProbability (ClassificationTable me)
{
	long i, j, col;
	Strings thee = Strings_createFixedLength (my numberOfRows);

	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		double max = my data[i][1];
		for (col = 1, j = 2; j <= my numberOfColumns; j++)
		{
			if (my data[i][j] > max)
			{
				max = my data[i][j]; col = j;
			}
		}
		if (my columnLabels[col] != NULL && ! Strings_setString (thee, my columnLabels[col], i)) break;
	}
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Categories ClassificationTable_to_Categories_maximumProbability (ClassificationTable me)
{
	long i, j, col;
	Categories thee = Categories_create ();
	
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		double max = my data[i][1];
		for (col = 1, j = 2; j <= my numberOfColumns; j++)
		{
			if (my data[i][j] > max)
			{
				max = my data[i][j]; col = j;
			}
		}
		if (! OrderedOfString_append (thee, my columnLabels[col]))
		{
			forget (thee); return NULL;
		}
	}
	return thee;
}

/* End of file ClassificationTable.c */
