#ifndef _Matrix_and_NMF_h_
#define _Matrix_and_NMF_h_
/* Matrix_and_NMF.h
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

#include "Matrix.h"
#include "NMF.h"

autoNMF Matrix_to_NMF_mu (Matrix me, integer numberOfFeatures, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, kNMF_Initialization initializationMethod, bool info);
autoNMF Matrix_to_NMF_als (Matrix me, integer numberOfFeatures, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, kNMF_Initialization initializationMethod, bool info);
autoNMF Matrix_to_NMF_is (Matrix me, integer numberOfFeatures, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, kNMF_Initialization initializationMethod, bool info);

void NMF_Matrix_improveFactorization_mu (NMF me, Matrix thee, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info);
void NMF_Matrix_improveFactorization_als (NMF me, Matrix thee, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info);
void NMF_Matrix_improveFactorization_is (NMF me, Matrix thee, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info);

autoMatrix NMF_to_Matrix (NMF me);

#endif /* _Matrix_and_NMF_h_ */
