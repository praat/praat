/* Configuration_and_Procrustes.c
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
 djmw 20020424 GPL
 djmw 20020704 Changed header NUMclasses.h to SVD.h
 djmw 20041022 Added orthogonal argument to Configurations_to_Procrustes.
*/

#include "Configuration_and_Procrustes.h"
#include "NUM2.h"

Procrustes Configurations_to_Procrustes (Configuration me, Configuration thee, int orthogonal)
{
	Procrustes p;
	double *translation = NULL, *scale = NULL;
	
	if (my numberOfRows != thy numberOfRows || my numberOfColumns != thy numberOfColumns) 
		return Melder_errorp1 (L"Configurations_to_Procrustes: Configurations must have the "
			"same number of points and the same dimension.");

	p = Procrustes_create (my numberOfColumns);
	if (p == NULL) return NULL;
	if (! orthogonal)
	{
		translation = p -> t;
		scale = &(p -> s);
	}
	if (! NUMProcrustes (my data, thy data, my numberOfRows, my numberOfColumns,
		p -> r, translation, scale)) forget (p);

	return p;
}

/* End of file Configuration_and_Procrustes.c */
