/* FFNet_Matrix.cpp
 *
 * Copyright (C) 1997-2019 David Weenink
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
		Melder_require (layer > 0 && layer <= my numberOfLayers,
			U"Layer should be in [1, ", my numberOfLayers, U"].");
		const integer numberOfUnitsInPreviousLayer = ( layer == 1 ? my numberOfInputs : my numberOfUnitsInLayer [layer - 1] );

		autoMatrix thee = Matrix_create (0.5, my numberOfUnitsInLayer [layer] + 0.5, my numberOfUnitsInLayer [layer], 1.0, 1.0,
		    0.5, numberOfUnitsInPreviousLayer + 1 + 0.5, numberOfUnitsInPreviousLayer + 1, 1.0, 1.0);
		integer node = my numberOfInputs + 1 + 1;
		for (integer i = 1; i < layer; i ++)
			node += my numberOfUnitsInLayer [i] + 1;

		for (integer i = 1; i <= my numberOfUnitsInLayer [layer]; i ++, node ++) {
			integer k = 1;
			for (integer j = my wFirst [node]; j <= my wLast [node]; j ++)
				thy z [k ++] [i] = deltaWeights ? my dwi [j] : my w [j];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoFFNet FFNet_weightsFromMatrix (FFNet me, Matrix him, integer layer) {
	try {
		Melder_require (layer > 0 && layer <= my numberOfLayers,
			U"Layer should be in [1, ", my numberOfLayers, U"].");
		Melder_require (my numberOfUnitsInLayer [layer] == his nx,
			U"The number of columns (", his nx, U") should equal the number of units (", my numberOfUnitsInLayer [layer], U") in layer ", layer, U".");
		
		const integer nunits = ( layer == 1 ? my numberOfInputs + 1 : my numberOfUnitsInLayer [layer - 1] + 1 );
		Melder_require (nunits == his ny,
			U"The number of rows (", his ny, U")  should equal the number of units (", nunits , U") in layer ", layer - 1, U".");
		
		autoFFNet thee = Data_copy (me);
		integer node = my numberOfInputs + 1 + 1;
		for (integer i = 1; i < layer; i ++)
			node += thy numberOfUnitsInLayer [i] + 1;

		for (integer i = 1; i <= thy numberOfUnitsInLayer [layer]; i ++, node ++) {
			integer k = 1;
			for (integer j = thy wFirst [node]; j <= thy wLast [node]; j ++, k ++)
				thy w [j] = his z [k] [i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no FFNet created.");
	}
}

/* End of file FFNet_Matrix.cpp */
