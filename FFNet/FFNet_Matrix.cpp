/* FFNet_Matrix.cpp
 *
 * Copyright (C) 1997-2011 David Weenink
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

Matrix FFNet_weightsToMatrix (FFNet me, long layer, int deltaWeights) {
	try {
		if (layer < 1 || layer > my nLayers) {
			Melder_throw ("Layer must be > 0 and < ", my nLayers, ".");
		}
		autoMatrix thee = Matrix_create (
		                      0.5, my nUnitsInLayer[layer] + 0.5, my nUnitsInLayer[layer], 1, 1,
		                      0.5, my nUnitsInLayer[layer - 1] + 1 + 0.5, my nUnitsInLayer[layer - 1] + 1, 1, 1);
		long node = 1;
		for (long i = 0; i < layer; i++) {
			node += my nUnitsInLayer[i] + 1;
		}
		for (long i = 1; i <= my nUnitsInLayer[layer]; i++, node++) {
			long k = 1;
			for (long j = my wFirst[node]; j <= my wLast[node]; j++) {
				thy z[k++][i] = deltaWeights ? my dwi[j] : my w[j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Matrix created.");
	}
}

FFNet FFNet_weightsFromMatrix (FFNet me, Matrix him, long layer) {
	try {
		if (layer < 1 || layer > my nLayers) {
			Melder_throw ("Layer must be > 0 and < ", my nLayers, ".");
		}
		if (my nUnitsInLayer[layer] != his nx) Melder_throw (L"The #columns (", his nx, ") must equal "
			        "#units (", my nUnitsInLayer[layer], ") in layer ", layer, ".");
		long nunits = my nUnitsInLayer[layer - 1] + 1;
		if (nunits != his ny) {
			Melder_throw (" The #rows (", his ny, ")  must equal #units (", nunits , ") in layer ", layer - 1, ".");
		}
		autoFFNet thee = (FFNet) Data_copy (me);
		long node = 1;
		for (long i = 0; i < layer; i++) {
			node += thy nUnitsInLayer[i] + 1;
		}
		for (long i = 1; i <= thy nUnitsInLayer[layer]; i++, node++) {
			long k = 1;
			for (long j = thy wFirst[node]; j <= thy wLast[node]; j++, k++) {
				thy w[j] = his z[k][i];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no FFNet created.");
	}
}

/* End of file FFNet_Matrix.cpp */
