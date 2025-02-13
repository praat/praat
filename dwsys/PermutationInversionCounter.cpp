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

integer PermutationInversionCounter_mergeInversions (PermutationInversionCounter me, integer p, integer q, integer r) {
	const integer nl = q - p + 1;
	const integer nr = r - q;
	INTVEC v = my psortingOrder -> p.get();
	INTVEC vl = my workspace.part (1, nl), vr = my workspace.part (nl + 1, nl + nr);
	INTVEC data = my pdata -> p.get();
	INTVEC otherInverse = my potherInverse -> p.get();
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
			if (my numberOfInversionsToRegister > 0) {
				for (integer ii = i; ii <= nl; ii ++) {
					my totalNumberOfInversions ++;
					if (otherInverse.size > 0 && otherInverse [data[vl [ii]]] < otherInverse [data[vr [j]]]) // if also inversion in other: skip
						continue;
					/*
						The inversions that passed are in the 'interval'
					*/
					my totalNumberOfInversionsInInterval ++;
					const integer index = inversionToIndex (v.size, data[vr [j]], data[vl [ii]]);
					while (my totalNumberOfInversionsInInterval == my sortedSelectedInversionIndices [my posInSortedSelectedInversionIndices]) {
						my inversions [++ my numberOfInversionsRegistered] = index; //
						if (-- my numberOfInversionsToRegister == 0)
							break;
						my posInSortedSelectedInversionIndices ++;
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

integer PermutationInversionCounter_mergeSort (PermutationInversionCounter me, integer p, integer r) {
	integer numberOfInversions = 0;
	if (p < r) {
		const integer q = (p + r) / 2;
		numberOfInversions += PermutationInversionCounter_mergeSort (me, p, q);
		numberOfInversions += PermutationInversionCounter_mergeSort (me, q + 1, r);
		numberOfInversions += PermutationInversionCounter_mergeInversions (me, p, q, r);
	}
	return numberOfInversions;
}

void PermutationInversionCounter_reset (PermutationInversionCounter me) {
	for (integer i = 1; i <= my numberOfElements; i ++)
		my psortingOrder -> p [i] = i;
	my totalNumberOfInversions = 0;
	my totalNumberOfInversionsInInterval = 0;
	my numberOfInversionsRegistered = 0;
	my numberOfInversionsToRegister = 0;
	my posInSortedSelectedInversionIndices = 1;
	my potherInverse = nullptr;
	
}

void PermutationInversionCounter_init (PermutationInversionCounter me, integer numberOfElements) {
	Melder_assert (numberOfElements > 0);
	my numberOfElements = numberOfElements;
	my workspace.resize (numberOfElements);
	my psortingOrder = Permutation_create (numberOfElements, true);
	PermutationInversionCounter_reset (me);
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

integer PermutationInversionCounter_getNumberOfInversions (PermutationInversionCounter me, constPermutation p) {
	INTVEC pi = p -> p.get();
	my workspace.resize (pi.size);
	INTVEC bit = my workspace.get ();

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

integer PermutationInversionCounter_getSelectedInversionsNotInOther (PermutationInversionCounter me, constPermutation p,
	constPermutation otherInverse, INTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions)
{
	Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
	Melder_assert (otherInverse -> p.size == p -> p.size);
	const integer numberOfElements = p -> p.size;
	my newData (p);
 	PermutationInversionCounter_reset (me);
	my sortedSelectedInversionIndices = sortedSelectedInversionIndices;
	my potherInverse = otherInverse;
	my numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
	my inversions = out_inversions;
	const integer numberOfInversions = PermutationInversionCounter_mergeSort (me, 1_integer, numberOfElements);
	return numberOfInversions;
}

integer PermutationInversionCounter_getSelectedInversions (PermutationInversionCounter me, constPermutation p,
	constINTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions)
{
	Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
	my newData (p);
	PermutationInversionCounter_reset (me);
	my sortedSelectedInversionIndices = sortedSelectedInversionIndices;
	my numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
	my inversions = out_inversions;
	const integer numberOfInversions = PermutationInversionCounter_mergeSort (me, 1_integer, p -> numberOfElements);
	return numberOfInversions;
}

integer PermutationInversionCounter_getInversions (PermutationInversionCounter me, constPermutation p, INTVEC const& out_inversions) {
	try {
		Melder_assert (out_inversions.size > 0);
		autoINTVEC sortedSelectedInversionIndices = to_INTVEC (out_inversions.size);
		const integer numberOfInversions = PermutationInversionCounter_getSelectedInversions (me, p, sortedSelectedInversionIndices.get(), out_inversions);
		return numberOfInversions;
	} catch (MelderError) {
		Melder_throw (me, U": cannot determine the inversions.");
	}
}

/* End of file PermutationInversionCounter.cpp */
