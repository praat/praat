/* AffineTransform.c
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
 djmw 20020315 GPL header
 djmw 20041027 Added AffineTransform_extractMatrix
 djmw 20050726 Added AffineTransform_extractTranslationVector
 djmw 20061021 printf expects %ld for 'long int'
 */

#include "AffineTransform.h"
#include "NUM2.h"


static void classAffineTransform_transform (I, double **in, long nrows, 
	double **out)
{
	iam (AffineTransform);
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
			out[i][j] = tmp + my t[j];
		}
	}	
}

static Any classAffineTransform_invert (I)
{
	iam (AffineTransform);
	AffineTransform thee = Data_copy (me);
	double tolerance = 0.000001;
	
	if (thee == NULL) return NULL;
	
	if (NUMpseudoInverse_d (my r, my n, my n, thy r, tolerance))
	{
		long i, j;
		for (i = 1; i <= my n; i++)
		{
			thy t[i] = 0;
			for (j = 1; j <= thy n; j++)
			{
				thy t[i] -= thy r[i][j] * my t[j];
			}
		}
	}
	else
	{
		forget (thee);
	}
	
	return thee;
}

#include "oo_DESTROY.h"
#include "AffineTransform_def.h"
#include "oo_COPY.h"
#include "AffineTransform_def.h"
#include "oo_EQUAL.h"
#include "AffineTransform_def.h"
#include "oo_WRITE_TEXT.h"
#include "AffineTransform_def.h"
#include "oo_WRITE_BINARY.h"
#include "AffineTransform_def.h"
#include "oo_READ_TEXT.h"
#include "AffineTransform_def.h"
#include "oo_READ_BINARY.h"
#include "AffineTransform_def.h"
#include "oo_DESCRIPTION.h"
#include "AffineTransform_def.h"

class_methods (AffineTransform, Data)
	class_method_local (AffineTransform, destroy)
	class_method_local (AffineTransform, copy)
	class_method_local (AffineTransform, equal)
	class_method_local (AffineTransform, writeText)
	class_method_local (AffineTransform, writeBinary)
	class_method_local (AffineTransform, readText)
	class_method_local (AffineTransform, readBinary)
	class_method_local (AffineTransform, description)
	class_method_local (AffineTransform, transform)
	class_method_local (AffineTransform, invert)
class_methods_end

int AffineTransform_init (I, long n)
{
	iam (AffineTransform);

	Melder_assert (n > 0);

	my n = n;
	my r = NUMdmatrix (1, n, 1, n);
	if (my r == NULL) return 0;
	my t = NUMdvector (1, n);
	if (my t == NULL) return 0;
    return 1;
}

AffineTransform AffineTransform_create (long n)
{
	AffineTransform me = new (AffineTransform);

	if (me == NULL || ! AffineTransform_init (me, n)) forget (me);
	return me;
}

Any AffineTransform_invert (I)
{
	iam (AffineTransform);
	AffineTransform thee = NULL;
	if (our invert) thee = (AffineTransform) our invert (me);
	return thee;
}

TableOfReal AffineTransform_extractMatrix (I)
{
	iam (AffineTransform);
	TableOfReal thee = NULL;
	long i;
	
	thee = TableOfReal_create (my n, my n);
	if (thee == NULL) return NULL;
	NUMdmatrix_copyElements (my r, thy data, 1, my n, 1, my n);
	for (i = 1; i <= my n; i++)
	{
		wchar_t label[20];
		(void) swprintf (label, 20, L"%ld", i);
		TableOfReal_setRowLabel (thee, i, label);
		TableOfReal_setColumnLabel (thee, i, label);
	}
	return thee;
}

TableOfReal AffineTransform_extractTranslationVector (I)
{
	iam (AffineTransform);
	TableOfReal thee = NULL;
	long i;

	thee = TableOfReal_create (1, my n);
	if (thee == NULL) return NULL;
	for (i = 1; i <= my n; i++) thy data[1][i] = my t[i];
	return thee;
}

/* End of file AffineTransform.c */
