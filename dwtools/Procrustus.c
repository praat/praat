/* Procrustus.c
 *
 * Copyright (C) 1993-2004 David Weenink
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
 djmw 20040117 Corrected bug in classProcrustus_transform: scale (s) was not used.
*/

#include "Procrustus.h"

static void classProcrustus_transform (I, double **in, long nrows, 
	double **out)
{
	iam (Procrustus);
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

static Any classProcrustus_invert (I)
{
	iam (Procrustus);
	Procrustus thee = Data_copy (me);
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
#include "Procrustus_def.h"
#include "oo_COPY.h"
#include "Procrustus_def.h"
#include "oo_EQUAL.h"
#include "Procrustus_def.h"
#include "oo_WRITE_ASCII.h"
#include "Procrustus_def.h"
#include "oo_WRITE_BINARY.h"
#include "Procrustus_def.h"
#include "oo_READ_ASCII.h"
#include "Procrustus_def.h"
#include "oo_READ_BINARY.h"
#include "Procrustus_def.h"
#include "oo_DESCRIPTION.h"
#include "Procrustus_def.h"

class_methods (Procrustus, AffineTransform)
	class_method_local (Procrustus, destroy)
	class_method_local (Procrustus, copy)
	class_method_local (Procrustus, equal)
	class_method_local (Procrustus, writeAscii)
	class_method_local (Procrustus, writeBinary)
	class_method_local (Procrustus, readAscii)
	class_method_local (Procrustus, readBinary)
	class_method_local (Procrustus, description)
	class_method_local (Procrustus, transform)
	class_method_local (Procrustus, invert)
class_methods_end

static void Procrustus_setDefaults (Procrustus me)
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

Procrustus Procrustus_create (long n)
{
	Procrustus me = new (Procrustus);

	if (me == NULL || ! AffineTransform_init (me, n))
	{
		forget (me);
		return NULL;
	}
	Procrustus_setDefaults (me);
	return me;
}


/* End of file Procrustus.c */
