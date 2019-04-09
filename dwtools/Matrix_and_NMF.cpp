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

autoNMF Matrix_to_NMF_mu (Matrix me, integer numberOfFeatures, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, kNMF_Initialization initializationMethod) {
	try {
		autoNMF thee = NMF_createFromGeneralMatrix (my z.get(), numberOfFeatures);
		NMF_initializeFactorization (thee.get(), my z.get(), initializationMethod);
		NMF_improveFactorization_mu (thee.get(), my z.get(), maximumNumberOfIterations, changeTolerance, approximationTolerance);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": NMF cannot be created.");
	}
}

autoNMF Matrix_to_NMF_als (Matrix me, integer numberOfFeatures, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, kNMF_Initialization initializationMethod) {
	try {
		autoNMF thee = NMF_createFromGeneralMatrix (my z.get(), numberOfFeatures);
		NMF_initializeFactorization (thee.get(), my z.get(), initializationMethod);
		NMF_improveFactorization_als (thee.get(), my z.get(), maximumNumberOfIterations, changeTolerance, approximationTolerance);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": NMF cannot be created.");
	}
}

autoMatrix NMF_to_Matrix (NMF me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfRows, my numberOfColumns);
		autoMAT result = NMF_synthesize (me);
		thy z.get() <<= result.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Matrix not created.");
	}
}

void NMF_Matrix_improveFactorization_mu (NMF me, Matrix thee, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance) {
	Melder_require (my numberOfRows == thy ny && my numberOfColumns == thy nx, U"The dimensions of the NMF and the Matrix must match.");
	NMF_improveFactorization_mu (me, thy z.get(), maximumNumberOfIterations, changeTolerance, approximationTolerance);
}

/* End of file Matrix_and_NMF.cpp */
