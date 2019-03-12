/* Matrix_and_NMF.cpp
 *
 * Copyright (C) 2019 David Weenink
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

#include "Matrix_and_NMF.h"
#include "NUM2.h"

autoNMF Matrix_to_NMF (Matrix me, integer dimensionOfApproximation, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, int initialisationMethod) {
	try {
		autoNMF thee = NMF_createFromGeneralMatrix_mu (my z.get(), dimensionOfApproximation);
		NMF_initialize (thee.get(), my z.get(), initialisationMethod);
		NMF_improveApproximation_mu (thee.get(), my z.get(), maximumNumberOfIterations, changeTolerance, approximationTolerance);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": NMF cannot be created.");
	}
}

autoMatrix NMF_to_Matrix (NMF me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfRows, my numberOfColumns);
		MATVUmul_fast (thy z.get(), my w.get(), my h.get()); // Z = W*H
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Matrix not created.");
	}
}
/* End of file Matrix_and_NMF.cpp */
