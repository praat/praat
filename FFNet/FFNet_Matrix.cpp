/* FFNet_Matrix.cpp
 *
 * Copyright (C) 1997-2011, 2015 David Weenink
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

autoMatrix FFNet_weightsToMatrix (FFNet me, long layer, int deltaWeights) {
	try {
		if (layer < 1 || layer > my nLayers) {
			Melder_throw (U"Layer must be > 0 and < ", my nLayers, U".");
		}
		autoMatrix thee = Matrix_create (0.5, my nUnitsInLayer[layer] + 0.5, my nUnitsInLayer[layer], 1.0, 1.0,
		    0.5, my nUnitsInLayer[layer - 1] + 1 + 0.5, my nUnitsInLayer[layer - 1] + 1, 1.0, 1.0);
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
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoFFNet FFNet_weightsFromMatrix (FFNet me, Matrix him, long layer) {
	try {
		if (layer < 1 || layer > my nLayers) {
			Melder_throw (U"Layer must be > 0 and < ", my nLayers, U".");
		}
		if (my nUnitsInLayer[layer] != his nx) {
			Melder_throw (U"The #columns (", his nx, U") must equal #units (", my nUnitsInLayer[layer], U") in layer ", layer, U".");
		}
		long nunits = my nUnitsInLayer[layer - 1] + 1;
		if (nunits != his ny) {
			Melder_throw (U" The #rows (", his ny, U")  must equal #units (", nunits , U") in layer ", layer - 1, U".");
		}
		autoFFNet thee = Data_copy (me);
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
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no FFNet created.");
	}
}

/* End of file FFNet_Matrix.cpp */
