/* FFNet_Matrix.c
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
 djmw 19950206
 djmw 20020712 GPL header
*/

#include "FFNet_Matrix.h"

Matrix FFNet_weightsToMatrix (FFNet me, long layer, int deltaWeights)
{
	Matrix thee = NULL; long i, node = 1;
	if (layer < 1 || layer > my nLayers) return Melder_errorp ("FFNet_weightsToMatrix: "
		"layer must be > 0 and < %ld.", my nLayers);
	if (! (thee = Matrix_create (
		0.5, my nUnitsInLayer[layer]+0.5, my nUnitsInLayer[layer], 1, 1,
		0.5, my nUnitsInLayer[layer-1]+1+0.5, my nUnitsInLayer[layer-1]+1, 1, 1)
		 )) return thee;
	for (i=0; i < layer; i++) node += my nUnitsInLayer[i] + 1;
	for (i=1; i <= my nUnitsInLayer[layer]; i++, node++)
	{
		long j, k = 1;
		for (j=my wFirst[node]; j <= my wLast[node]; j++)
			thy z[k++][i] = deltaWeights ? my dwi[j] : my w[j];
	}
	return thee;	
}

FFNet FFNet_weightsFromMatrix (FFNet me, Matrix him, long layer)
{
	FFNet thee = NULL; long i, node = 1;
	if (layer < 1 || layer > my nLayers) return Melder_errorp ("FFNet_weightsFromMatrix: "
		"layer must be > 0 and < %ld.", my nLayers);
	if (my nUnitsInLayer[layer] != his nx) return Melder_errorp ("FFNet_weightsFromMatrix:"
		"#columns in Matrix must equal #units in layer %ld (%ld)", layer, my nUnitsInLayer[layer]);
	if (my nUnitsInLayer[layer-1]+1 != his ny) return Melder_errorp ("FFNet_weightsFromMatrix:"
		"#rows in Matrix must equal (#units in layer %ld) + 1 (%ld).",
		layer-1, my nUnitsInLayer[layer-1] + 1);
	if (! (thee = Data_copy (me))) return NULL;
	for (i=0; i < layer; i++) node += thy nUnitsInLayer[i] + 1;
	for (i=1; i <= thy nUnitsInLayer[layer]; i++, node++)
	{
		long j, k = 1;
		for (j=thy wFirst[node]; j <= thy wLast[node]; j++, k++)
			 thy w[j] = his z[k][i];
	}
	return thee;
}


/* End of file FFNet_Matrix.c */
