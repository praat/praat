/* Procrustes.c
 *
 * Copyright (C) 1993-2005 David Weenink
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
 djmw 20020813 GPL header
 djmw 20040117 Corrected bug in classProcrustes_transform: scale (s) was not
 used.
 djmw 20050406 Renamed Procrustus Procrustes
*/

#include "Procrustes.h"

static void classProcrustes_transform (I, double **in, long nrows, 
	double **out)
{
	iam (Procrustes);
	long i, j, k;

	for (i = 1; i <= nrows; i++)
	{
		for (j = 1; j <= my n; j++)
		{
			double tmp = 0;
			for (k = 1; k <= my n; k++)
			{
				tmp += in[i][k] * my r[k][j];
			}
			out[i][j] = my  s* tmp + my t[j];
		}
	}	
}

static Any classProcrustes_invert (I)
{
	iam (Procrustes);
	Procrustes thee = Data_copy (me);
	long i, j;
	
	if (thee == NULL) return NULL;
	
	/*
		R is symmetric rotation matrix --> 
		inverse is transpose!
	*/
	
	thy s = my s == 0 ? 1 : 1 / my s;
	
	for (i = 1; i <= my n; i++)
	{
		for (j = i + 1; j <= my n; j++)
		{
			thy r[i][j] = my r[j][i];
			thy r[j][i] = my r[i][j];
		}
		thy t[i] = 0;
		for (j = 1; j <= thy n; j++)
		{
			thy t[i] -= thy r[i][j] * my t[j];
		}
		thy t[i] *= thy s;
	}	
	return thee;	
}

#include "oo_DESTROY.h"
#include "Procrustes_def.h"
#include "oo_COPY.h"
#include "Procrustes_def.h"
#include "oo_EQUAL.h"
#include "Procrustes_def.h"
#include "oo_WRITE_ASCII.h"
#include "Procrustes_def.h"
#include "oo_WRITE_BINARY.h"
#include "Procrustes_def.h"
#include "oo_READ_ASCII.h"
#include "Procrustes_def.h"
#include "oo_READ_BINARY.h"
#include "Procrustes_def.h"
#include "oo_DESCRIPTION.h"
#include "Procrustes_def.h"

class_methods (Procrustes, AffineTransform)
	class_method_local (Procrustes, destroy)
	class_method_local (Procrustes, copy)
	class_method_local (Procrustes, equal)
	class_method_local (Procrustes, writeAscii)
	class_method_local (Procrustes, writeBinary)
	class_method_local (Procrustes, readAscii)
	class_method_local (Procrustes, readBinary)
	class_method_local (Procrustes, description)
	class_method_local (Procrustes, transform)
	class_method_local (Procrustes, invert)
class_methods_end

static void Procrustes_setDefaults (Procrustes me)
{
	long i, j;

	my s = 1;
	for (i = 1; i <= my n; i++)
	{
		my t[i] = 0;
		my r[i][i] = 1;
		for (j = i + 1; j <= my n; j++)
		{
			my r[i][j] = my r[j][i] = 0;
		}
	}
}

Procrustes Procrustes_create (long n)
{
	Procrustes me = new (Procrustes);

	if (me == NULL || ! AffineTransform_init (me, n))
	{
		forget (me);
		return NULL;
	}
	Procrustes_setDefaults (me);
	return me;
}


/* End of file Procrustes.c */
