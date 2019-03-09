/* NMF.h
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

#ifndef _NMF_h_
#define _NMF_h_

#include "NUM2.h"
#include "Data.h"
#include "NMF_def.h"

/*
	Non-negative Matrix Factorization. I.e. we try to factorize a non-negative nrow x ncol matrix M as H*W,
	where H and W are non-negative nrow x k and k x ncol matrices, respectively. 
*/

autoNMF NMF_create (integer numberOfRows, integer numberOfColumns, integer dimensionOfApproximation);

autoNMF NMF_createFromGeneralMatrix_mu (constMAT m, integer dimensionOfApproximation);

void NMF_initialize (NMF me, constMAT m, int initialisationMethod);

/*
	The matrix update equations are:
	H(n+1) = H(n).(W(n)'*M) / ((W(n)'*W(n))*H(n) + eps)
	W(n+1) = W(n).(A*H(n+1)') / (W(n)*(H(n+1)*H(n+1)') + eps)

*/
void NMF_improveApproximation_mu (NMF me, constMAT m, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance);

#endif /* _NMF_h_ */
