/* CCA_and_Correlation.c
 * 
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 2001
 djmw 20020525 GPL header
 */

#include "CCA_and_Correlation.h"
#include "NUM2.h"

TableOfReal CCA_and_Correlation_factorLoadings (CCA me, Correlation thee)
{
	char *proc = "CCA_and_Correlation_dvFactorLoadings";
	TableOfReal him;
	long i, j, k, ny = my y -> dimension, nx = my x -> dimension;
	double t, **evecy = my y -> eigenvectors, **evecx = my x -> eigenvectors;

	if (ny + nx != thy numberOfColumns) return Melder_errorp ("%s: the number "
		"of columns in the Correlation object must equal the sum of the "
		"dimensions in the CCA object", proc);

	him = TableOfReal_create (2 * my numberOfCoefficients, thy numberOfColumns);
	if (him == NULL) return NULL;
	
	if (! NUMstrings_copyElements (thy columnLabels, his columnLabels, 
			1, thy numberOfColumns) ||
		! TableOfReal_setSequentialRowLabels (him, 1, my numberOfCoefficients,
			"dv", 1, 1) ||
		! TableOfReal_setSequentialRowLabels (him, my numberOfCoefficients + 1,
			2 * my numberOfCoefficients, "iv", 1, 1))
	{
		forget (him);
		return NULL;
	}

	for (i = 1; i <= thy numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfCoefficients; j++)
		{
			t = 0;
			for (k = 1; k <= ny; k++)
			{
				t += thy data[i][k] * evecy[j][k];
			}
			his data[j][i] = t;
		}
		for (j = 1; j <= my numberOfCoefficients; j++)
		{
			t = 0;
			for (k = 1; k <= nx; k++)
			{
				t += thy data[i][ny + k] * evecx[j][k];
			}
			his data[my numberOfCoefficients + j][i] = t;
		}
	}
	return him;
}

/* End of file CCA_and_Correlation.h */
