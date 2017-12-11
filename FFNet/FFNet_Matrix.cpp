/* FFNet_Matrix.cpp
 *
 * Copyright (C) 1997-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 19950206
 djmw 20020712 GPL header
*/

#include "FFNet_Matrix.h"

autoMatrix FFNet_weightsToMatrix (FFNet me, integer layer, bool deltaWeights) {
	try {
		Melder_require (layer > 0 && layer <= my nLayers, U"Layer should be in [1, ", my nLayers, U"].");

		autoMatrix thee = Matrix_create (0.5, my nUnitsInLayer [layer] + 0.5, my nUnitsInLayer [layer], 1.0, 1.0,
		    0.5, my nUnitsInLayer [layer - 1] + 1 + 0.5, my nUnitsInLayer  [layer - 1] + 1, 1.0, 1.0);
		integer node = 1;
		for (integer i = 0; i < layer; i ++) {
			node += my nUnitsInLayer [i] + 1;
		}
		for (integer i = 1; i <= my nUnitsInLayer [layer]; i ++, node ++) {
			integer k = 1;
			for (integer j = my wFirst [node]; j <= my wLast [node]; j ++) {
				thy z [k ++] [i] = deltaWeights ? my dwi [j] : my w [j];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoFFNet FFNet_weightsFromMatrix (FFNet me, Matrix him, integer layer) {
	try {
		Melder_require (layer > 0 && layer <= my nLayers, U"Layer should be in [1, ", my nLayers, U"].");
		Melder_require (my nUnitsInLayer [layer] == his nx, U"The number of columns (", his nx, U") should equal the number of units (", my nUnitsInLayer [layer], U") in layer ", layer, U".");
		
		integer nunits = my nUnitsInLayer [layer - 1] + 1;
		Melder_require (nunits == his ny, U"The number of rows (", his ny, U")  should equal the number of units (", nunits , U") in layer ", layer - 1, U".");
		
		autoFFNet thee = Data_copy (me);
		integer node = 1;
		for (integer i = 0; i < layer; i ++) {
			node += thy nUnitsInLayer [i] + 1;
		}
		for (integer i = 1; i <= thy nUnitsInLayer [layer]; i ++, node ++) {
			integer k = 1;
			for (integer j = thy wFirst [node]; j <= thy wLast [node]; j ++, k ++) {
				thy w [j] = his z [k] [i];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no FFNet created.");
	}
}

/* End of file FFNet_Matrix.cpp */
