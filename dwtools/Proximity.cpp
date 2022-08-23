/* Proximity.cpp
 *
 * Copyright (C) 1993-2022 David Weenink
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
 djmw 20020813 GPL header
 djmw 20040309 Removed assertion 'numberOfPoints> 0' in Proximity_init
*/

#include "Configuration.h"
#include "Distance.h"
#include "Proximity_and_Distance.h"
#include "TableOfReal_extensions.h"

#include "NUM2.h"

Thing_implement (Proximity, TableOfReal, 0);

void Proximity_init (Proximity me, integer numberOfPoints) {
	TableOfReal_init (me, numberOfPoints, numberOfPoints);
	TableOfReal_setSequentialRowLabels (me, 0, 0, nullptr, 1, 1);
	TableOfReal_setSequentialColumnLabels (me, 0, 0, nullptr, 1, 1);
}

Thing_implement (Dissimilarity, Proximity, 0);

static bool Dissimilarity_hasNoNegativeValues (Dissimilarity me) {
	for (integer i = 1; i <= my numberOfRows - 1; i ++)
		for (integer j = i + 1; j <= my numberOfRows; j ++)
			if (my data [i] [j] < 0.0)
				return false;
	return true;
}

autoDissimilarity Dissimilarity_create (integer numberOfPoints) {
	try {
		autoDissimilarity me = Thing_new (Dissimilarity);
		Proximity_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Dissimilarity not created.");
	}
}

autoDissimilarity Dissimilarity_createLetterRExample (double noiseStd) {
	try {
		autoConfiguration r = Configuration_createLetterRExample (1);
		autoDistance d = Configuration_to_Distance (r.get());
		autoDissimilarity me = Distance_to_Dissimilarity (d.get());
		Thing_setName (me.get(), U"R");

		for (integer i = 1; i <= my numberOfRows - 1; i ++) {
			for (integer j = i + 1; j <= my numberOfRows; j ++) {
				const double dis = my data [i] [j];
				my data [j] [i] = my data [i] [j] = dis * dis + 5.0 + NUMrandomUniform (0.0, noiseStd);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Dissimilarity for letter R example not created.");
	}
}

/*
	Get the best estimate for the additive constant:
		"distance = dissimilarity + constant"
	F. Cailliez (1983), The analytical solution of the additive constant problem, Psychometrika 48, 305-308.
*/
double Dissimilarity_getAdditiveConstant (Dissimilarity me) {
	try {
		const integer nPoints = my numberOfRows, nPoints2 = 2 * nPoints;
		Melder_require (nPoints > 0,
			U"Matrix part should not be empty.");

		Melder_require (Dissimilarity_hasNoNegativeValues (me),
			U"Dissimilarities should not be negative.");
		
		autoMAT wd = zero_MAT (nPoints, nPoints);
		autoMAT wdsqrt = zero_MAT (nPoints, nPoints);
		/*
			The matrices D & D1/2 with distances (squared and linear)
		*/
		for (integer i = 1; i <= nPoints - 1; i ++) {
			for (integer j = i + 1; j <= nPoints; j ++) {
				const double proximity = (my data [i] [j] + my data [j] [i]) / 2.0;
				wdsqrt [j] [i] = wdsqrt [i] [j] = - proximity / 2.0; // djmw 20180830
				wd [j] [i] = wd [i] [j] = - proximity * proximity / 2.0;
			}
		}

		doubleCentre_MAT_inout (wdsqrt.get());
		doubleCentre_MAT_inout (wd.get());
		
		/*
			if wd is not positive semi-definite then the dissimilarities have no euclidean representation
			and we need to calculate an additive constant c such that the dissimilarities 
			d'[i][j] = d [i][j] + c have an euclidean representation.
			A positive semi-definite matrix has all eigenvalues larger than or equal to zero.
		*/
		autoVEC eigenvalues_wd;
		MAT_getEigenSystemFromSymmetricMatrix (wd.get(), nullptr, & eigenvalues_wd, true);
		
		if (eigenvalues_wd [1] >= 0.0)
			return 0.0;
		
		/*
			Calculate the B matrix according to eq. 6
		*/
		autoMAT b = zero_MAT (nPoints2, nPoints2);
		b.part (1, nPoints, nPoints + 1, nPoints2)  <<=  2.0  *  wd.get();
		b.part (nPoints + 1, nPoints2, 1, nPoints).diagonal()  <<=  -1.0;
		b.part (nPoints + 1, nPoints2, nPoints + 1, nPoints2)  <<=  -4.0  *  wdsqrt.get();
		/*
			Get eigenvalues of B
		*/
		autoCOMPVEC eigenvalues;
		MAT_getEigenSystemFromGeneralSquareMatrix (b.get(), & eigenvalues, nullptr);
		/*
			Get largest real eigenvalue. This value needs to be larger than or equal to zero,
			therefore we left-clip against zero. 
			Even if all eigenvalues turn out to be negative we set it to zero.
		*/
		double largestRealEigenvalue = 0.0;
		for (integer i = 1; i <= nPoints2; i ++)
			if (eigenvalues [i].imag() == 0.0 && eigenvalues [i].real() > largestRealEigenvalue)
				largestRealEigenvalue = eigenvalues [i].real();
		return largestRealEigenvalue;
	} catch (MelderError) {
		Melder_throw (U"Additive constant not calculated.");
	}
}

autoSimilarity Similarity_create (integer numberOfPoints) {
	try {
		autoSimilarity me = Thing_new (Similarity);
		Proximity_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Similarity not created.");
	}
}

/* End of file Proximity.cpp */
