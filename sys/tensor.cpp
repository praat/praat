/* tensor.cpp
 *
 * Copyright (C) 2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "tensor.h"

void numvec :: _initAt (long givenSize, bool zero) {
	Melder_assert (givenSize >= 0);
	try {
		our at = ( givenSize == 0 ? nullptr : NUMvector <double> (1, givenSize, zero) );
	} catch (MelderError) {
		Melder_throw (U"Numeric vector not created.");
	}
}

void numvec :: _freeAt () noexcept {
	if (our at) NUMvector_free (our at, 1);
}

void nummat :: _initAt (long givenNrow, long givenNcol, bool zero) {
	Melder_assert (givenNrow >= 0);
	Melder_assert (givenNcol >= 0);
	try {
		our at = ( givenNrow > 0 && givenNcol > 0 ? NUMmatrix <double> (1, givenNrow, 1, givenNcol, zero) : nullptr );
	} catch (MelderError) {
		Melder_throw (U"Numeric matrix not created.");
	}
}

void nummat :: _freeAt () noexcept {
	if (our at) NUMmatrix_free (our at, 1, 1);
}

double stdev_scalar (numvec x) noexcept {
	if (x.size < 2) return NUMundefined;   // -> from here on, x.size >= 2 -> x.size != 0
	double sum = 0.0;   // -> sum in R (invariant)
	for (long i = 1; i <= x.size; i ++) {
		sum += x [i];   // sum before in R, x [i] in R -> sum after in R
	}
	double mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
	double sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
	for (long i = 1; i <= x.size; i ++) {
		double residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
		double squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
		sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
	}
	long degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
	double meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
	double rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
	return rootMeanSquaredResidual;
}

double center_scalar (numvec x) noexcept {
	double weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return weightedSumOfIndexes / sumOfWeights;
}

autonumvec copy_numvec (numvec x) {
	autonumvec result (x.size, false);
	for (long i = 1; i <= x.size; i ++) {
		result [i] = x [i];
	}
	return result;
}

autonummat copy_nummat (nummat x) {
	autonummat result (x.nrow, x.ncol, false);
	for (long irow = 1; irow <= x.nrow; irow ++) {
		for (long icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol];
		}
	}
	return result;
}

autonummat outer_nummat (numvec x, numvec y) {
	autonummat result (x.size, y.size, false);
	for (long irow = 1; irow <= x.size; irow ++) {
		for (long icol = 1; icol <= y.size; icol ++) {
			result [irow] [icol] = x [irow] * y [icol];
		}
	}
	return result;
}

/* End of file tensor.cpp */
