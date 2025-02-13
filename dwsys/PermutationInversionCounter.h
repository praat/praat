#ifndef _PermutationInversionCounter_h_
#define _PermutationInversionCounter_h_
/* PermutationInversionCounter.h
 *
 * Copyright (C) 2025 David Weenink
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

#include "Permutation.h"
#include "PermutationInversionCounter_def.h"

/*
	Given a vector v with n elements, we define an inversion as i < j && v [i] > v [j].
	We store the inversion with one number, as the position of the cell in a nxn matrix
	indexed by row i and column j.
	The number for the inversion (i,j) will be: code = (i-1)*(n-1)+ j.
	From this number we can easily get back the inversion i,j by
		i = (code - 1) / n + 1
		j = (code -1) % n
		if (i > j) swap (i,j)
 */

inline integer inversionToIndex (integer n, integer i, integer j) {
	return (i - 1) * n + j;
}

inline void getInversionFromIndex (integer n, integer index, integer& ilow, integer& ihigh) {
	ilow = (index - 1) / n + 1;
	ihigh = (index - 1) % n + 1;
	if (ilow > ihigh)
		std::swap (ilow, ihigh);
}

void PermutationInversionCounter_init (integer size);

autoPermutationInversionCounter PermutationInversionCounter_create (integer size);

integer PermutationInversionCounter_getNumberOfInversions (PermutationInversionCounter me, constPermutation p);

integer PermutationInversionCounter_getSelectedInversionsNotInOther (PermutationInversionCounter me, constPermutation p, constPermutation otherInverse,
	INTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions);

integer PermutationInversionCounter_getSelectedInversions (PermutationInversionCounter me, constPermutation p,
	constINTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions);

integer PermutationInversionCounter_getInversions (PermutationInversionCounter me, constPermutation p, INTVEC const& out_inversions);

#endif // _PermutationInversionCounter_h_
