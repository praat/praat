/* PermutationInversionCounter.cpp
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

#include "NUM2.h"
#include "PermutationInversionCounter.h"

#include "oo_DESTROY.h"
#include "PermutationInversionCounter_def.h"
#include "oo_COPY.h"
#include "PermutationInversionCounter_def.h"
#include "oo_EQUAL.h"
#include "PermutationInversionCounter_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PermutationInversionCounter_def.h"
#include "oo_WRITE_TEXT.h"
#include "PermutationInversionCounter_def.h"
#include "oo_WRITE_BINARY.h"
#include "PermutationInversionCounter_def.h"
#include "oo_READ_TEXT.h"
#include "PermutationInversionCounter_def.h"
#include "oo_READ_BINARY.h"
#include "PermutationInversionCounter_def.h"
#include "oo_DESCRIPTION.h"
#include "PermutationInversionCounter_def.h"

/*
	The algorithm to do merge-sort was found in (CLRS) Corman, Leiserson, Rivest & Stein: Introduction to algorithms,
		third edition, The MIT press, as the solution of problem 2.4.d.
	However, on 15 January 2025, their solution on the web-site of the book had the counting variable in the wrong position
*/

Thing_implement (PermutationInversionCounter, Daata, 1);

integer structPermutationInversionCounter :: mergeInversions (integer p, integer q, integer r) {
	const integer nl = q - p + 1;
	const integer nr = r - q;
	INTVEC v = psortingOrder -> p.get();
	INTVEC vl = workspace.part (1, nl), vr = workspace.part (nl + 1, nl + nr);
	INTVEC data = pdata -> p.get();
	for (integer ii = 1; ii <= nl; ii ++)
		vl [ii] = v [p + ii - 1];
	for (integer ii = 1; ii <= nr; ii ++)
		vr [ii] = v [q + 1 + ii - 1];
	integer i = 1, j = 1, k = p, localNumberOfInversions = 0;
	while (i <= nl && j <= nr) {
		if (data [vl [i]] < data [vr [j]]) {
			v [k ++] = vl [i ++];
		} else { // vl[i] > vr[j]
			localNumberOfInversions += nl - i + 1;
			if (numberOfInversionsToRegister > 0) {
				for (integer ii = i; ii <= nl; ii ++) {
					totalNumberOfInversions ++;
					if (otherInverse.size > 0 && otherInverse [data [vl [ii]]] < otherInverse [data [vr [j]]]) // if also inversion in other: skip
						continue;
					/*
						The inversions that passed are in the 'interval'
					*/
					totalNumberOfInversionsInInterval ++;
					const integer code = getCodeFromInversion (data [vr [j]], data [vl [ii]]);
					while (totalNumberOfInversionsInInterval == sortedSelectedInversionIndices [posInSortedSelectedInversionIndices]) {
						inversions [++ numberOfInversionsRegistered] = code; //
						if (-- numberOfInversionsToRegister == 0)
							break;
						posInSortedSelectedInversionIndices ++;
					}
				}
			}
			v [k ++] = vr [j ++]; //! CLRS: A[k] = L[i++];
		}
	}
	while (i <= nl)
		v [k ++] = vl [i ++];
	while (j <= nr)
		v [k ++] = vr [j ++];
	return localNumberOfInversions;
}

integer structPermutationInversionCounter :: mergeSort (integer p, integer r) {
	integer numberOfInversions = 0;
	if (p < r) {
		const integer q = (p + r) / 2;
		numberOfInversions += mergeSort (p, q);
		numberOfInversions += mergeSort (q + 1, r);
		numberOfInversions += mergeInversions (p, q, r);
	}
	return numberOfInversions;
}

void structPermutationInversionCounter :: reset () {
	for (integer i = 1; i <= numberOfElements; i ++)
		psortingOrder -> p [i] = i;
	totalNumberOfInversions = 0;
	totalNumberOfInversionsInInterval = 0;
	numberOfInversionsRegistered = 0;
	numberOfInversionsToRegister = 0;
	posInSortedSelectedInversionIndices = 1;
	potherInverse = nullptr;
	
}

integer structPermutationInversionCounter :: getNumberOfInversions (constPermutation p) {
	INTVEC pi = p -> p.get();
	workspace.resize (pi.size);
	INTVEC bit = workspace.get ();

	// we use a binary indexed tree
	auto update = [&] (integer index, integer value) {
		for (integer i = index; i <= bit.size; i += i & -i)
			bit [i] += value;
	};

	auto query = [&] (integer index) {
		integer sum = 0;
		for (integer i = index; i >= 1; i -= i & -i)
			sum += bit [i];
		return sum;
	};

	for (integer i = 1; i <= pi.size; i ++)
		bit [i] = 0;
	integer count = 0;
	for (integer i = pi.size; i >= 1; i --) {
		count += query (pi [i]);
		update (pi [i], 1);
	}
	return count;
}

integer structPermutationInversionCounter :: getSelectedInversionsNotInOther (constPermutation p,
	constPermutation otherInverse, INTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions)
{
	Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
	Melder_assert (otherInverse -> p.size == p -> p.size);
	const integer numberOfElements = p -> p.size;
	newData (p);
 	reset ();
	our otherInverse = potherInverse -> p.get();
	our sortedSelectedInversionIndices = sortedSelectedInversionIndices;
	our potherInverse = otherInverse;
	numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
	our inversions = out_inversions;
	const integer numberOfInversions = mergeSort (1_integer, numberOfElements);
	return numberOfInversions;
}

integer structPermutationInversionCounter :: getSelectedInversions (constPermutation p,
	constINTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions)
{
	Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
	newData (p);
	reset ();
	our sortedSelectedInversionIndices = sortedSelectedInversionIndices;
	numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
	inversions = out_inversions;
	const integer numberOfInversions = mergeSort (1_integer, p -> numberOfElements);
	return numberOfInversions;
}

integer structPermutationInversionCounter :: getInversions (constPermutation p, INTVEC const& out_inversions) {
	try {
		Melder_assert (out_inversions.size > 0);
		autoINTVEC sortedSelectedInversionIndices = to_INTVEC (out_inversions.size);
		const integer numberOfInversions =getSelectedInversions (p, sortedSelectedInversionIndices.get(), out_inversions);
		return numberOfInversions;
	} catch (MelderError) {
		Melder_throw (this, U": cannot determine the inversions.");
	}
}

void PermutationInversionCounter_init (PermutationInversionCounter me, integer numberOfElements) {
	Melder_assert (numberOfElements > 0);
	my numberOfElements = numberOfElements;
	my workspace.resize (numberOfElements);
	my psortingOrder = Permutation_create (numberOfElements, true);
	my reset ();
}

autoPermutationInversionCounter PermutationInversionCounter_create (integer numberOfElements) {
	try {
		Melder_assert (numberOfElements > 0);
		autoPermutationInversionCounter me = Thing_new (PermutationInversionCounter);
		PermutationInversionCounter_init (me.get(), numberOfElements);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create PermutationInversionCounter");
	}
}

/* End of file PermutationInversionCounter.cpp */
