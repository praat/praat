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
#include "NMF_enums.h"

#include "NMF_def.h"

/*
	Non-negative Matrix Factorization. I.e. we try to factorize a non-negative nrow x ncol matrix M as H*W,
	where H and W are non-negative nrow x k and k x ncol matrices, respectively. 
*/

void NMF_paintFeatures (NMF me, Graphics g, integer fromFeature, integer toFeature, integer fromRow, integer toRow, double minimum, double maximum, int amplitudeScale, int scaling, bool garnish);

void NMF_paintWeights (NMF me, Graphics g, integer fromWeight, integer toWeight, integer fromRow, integer toRow, double minimum, double maximum, int amplitudeScale, int scaling, bool garnish);

autoNMF NMF_create (integer numberOfRows, integer numberOfColumns, integer numberOfFeatures);

autoNMF NMF_createFromGeneralMatrix (constMATVU const& data, integer numberOfFeatures);

void NMF_initializeFactorization (NMF me, constMATVU const& data, kNMF_Initialization initializationMethod);

/*
	Factorize D as F*W, where D, F and W >= 0
	
	initialize F and W;
	for iter to maxiter
		W(n+1) = W(n).(F(n)'*D) / (F(n)'*F(n))*W(n) + eps)
		F(n+1) = F(n).(D*W(n+1)') / (F(n)*(W(n+1)*W(n+1)') + eps)
	endfor
*/
void NMF_improveFactorization_mu (NMF me, constMATVU const& data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info);

/*
	Factorize D as F*W, where D, F and W >= 0
	
	initialize F;
	for iter to maxiter
		W is solution of F´*F*W = F'*D.
		Set all negative elements in W to 0.
		F is solution of W*W'*F' = W*D' .
		Set all negative elements in F to 0.
	endfor
*/
void NMF_improveFactorization_als (NMF me, constMATVU const& data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info);


/*
	Factorize D as F*W, where D, F and W >= 0.
	
	C. Févotte, N. Berin & J.-L. Durrieu (2009), Nonnegative matrix facorization with the Itakura-Saito divergene: with 
	applications to music analysis, Neural Computation 21, 793--830.
*/
void NMF_improveFactorization_is (NMF me, constMATVU const& data, integer maximumNumberOfIterations, double changeTolerance, double approximationTolerance, bool info);

autoMAT NMF_synthesize (NMF me); // result = features * weights

double NMF_getEuclideanDistance (NMF me, constMATVU const& data); // sqrt (||data - features*weights||²)

double NMF_getItakuraSaitoDivergence (NMF me, constMATVU const& data);

#endif /* _NMF_h_ */
