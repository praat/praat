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

#include "NUM2.h"
#include "Permutation.h"
/*
	The algorithm to do the counting only was found in (CLRS) Corman, Leiserson, Rivest & Stein: Introduction to algorithms,
		third edition, The MIT press, as the solution of problem 2.4.d.
	However, on 15 January 2025, their solution on the web-site of the book had the counting variable in the wrong position
*/

/*
	Given a vector v with n elements, we define an inversion as i < j && v [i] > v [j].
	We store the inversion with one number, as the position of the cell in a nxn matrix
	indexed by row i and column j.
	The number for the inversion (i,j) will be code = (i-1)*(n-1)+ j.
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


class PermutationInversionCounter {

private:

	/*
		We work with the Permutation's data vector of size n. It will not be modified.
		We determine data's sorting order.
	*/
	autoINTVEC workspace;		// size(n) for the merge-sort
	constINTVEC data;			// size(n) the permutation whose ...
	autoINTVEC sortingOrder;	// size(n) ... sorting order will be determined by merge-sort
	INTVEC v;					// link to sortingOrder
	INTVEC inversions;			// the coded inversions to be output
	constINTVEC sortedSelectedInversionIndices;
	constINTVEC otherInverse;	// size(n)
	integer totalNumberOfInversions = 0;
	integer totalNumberOfInversionsInInterval = 0;
	integer numberOfInversionsRegistered = 0;
	integer numberOfInversionsToRegister = 0;
	integer posInSortedSelectedInversionIndices = 1;

	void sortingOrder_init (integer size) {
		sortingOrder.resize (size);
		v = sortingOrder.get();
		for (integer i = 1; i <= size; i ++)
			v [i] = i;
	}

	void init (constINTVEC data) {
		our data = data;
		initWithSize (data.size);
	}

	void initWithSize (integer size) {
		Melder_assert (size > 0);
		workspace.resize (size);
		sortingOrder_init (size);
		v = sortingOrder.get();
		totalNumberOfInversions = 0;
		totalNumberOfInversionsInInterval = 0;
		numberOfInversionsRegistered = 0;
		numberOfInversionsToRegister = 0;
		posInSortedSelectedInversionIndices = 1;
		otherInverse = {};
	}

	integer mergeInversions (integer p, integer q, integer r) {
		const integer nl = q - p + 1;
		const integer nr = r - q;
		INTVEC vl = workspace.part (1, nl), vr = workspace.part (nl + 1, nl + nr);
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
						if (otherInverse.size > 0 && otherInverse [data[vl [ii]]] < otherInverse [data[vr [j]]]) // if also inversion in other: skip
							continue;
						/*
							The inversions that passed are in the 'interval'
						*/
						totalNumberOfInversionsInInterval ++;
						const integer index = inversionToIndex (v.size, data[vr [j]], data[vl [ii]]);
						while (totalNumberOfInversionsInInterval == sortedSelectedInversionIndices [posInSortedSelectedInversionIndices]) {
							inversions [++ numberOfInversionsRegistered] = index; //
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

	integer countInversionsBySorting_ (integer p, integer r) {
		integer numberOfInversions = 0;
		if (p < r) {
			const integer q = (p + r) / 2;
			numberOfInversions += countInversionsBySorting_ (p, q);
			numberOfInversions += countInversionsBySorting_ (q + 1, r);
			numberOfInversions += mergeInversions (p, q, r);
		}
		return numberOfInversions;
	}

public:

	PermutationInversionCounter () { // needed for SlopeSelector
	}

	PermutationInversionCounter (integer size) {
		initWithSize (size);
	}

	PermutationInversionCounter (constPermutation p) {
		init (p -> p.get());
	}


	inline integer getNumberOfInversionsRegistered () {
		return numberOfInversionsRegistered;
	}

	integer getNumberOfInversions (constPermutation p) {
		INTVEC iv = p -> p.get();
		workspace.resize(iv.size);
		INTVEC bit = workspace.get ();

		// use binary indexed tree
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

		for (integer i = 1; i <= iv.size; i ++)
			workspace [i] = 0;
		integer count = 0;
		for (integer i = p -> numberOfElements; i >= 1; i --) {
			count += query (p -> p [i]);
			update (p -> p [i], 1);
		}

		return count;
	}

	integer getInversions (Permutation p, INTVEC const& out_inversions) {
		autoINTVEC sortedSelectedInversionIndices = to_INTVEC (out_inversions.size);
		const integer numberOfInversions = getSelectedInversions (p, sortedSelectedInversionIndices.get(), out_inversions);
		return numberOfInversions;
	}

	integer getSelectedInversions (Permutation p, constINTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions) {
		Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
		init (p -> p.get());
		our sortedSelectedInversionIndices = sortedSelectedInversionIndices;
		numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
		our inversions = out_inversions;
		const integer numberOfInversions = countInversionsBySorting_ (1_integer, v.size);
		return numberOfInversions;
	}

	integer getSelectedInversionsNotInOther (constPermutation p, constPermutation otherInverse, INTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions) {
		Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
		Melder_assert (otherInverse -> p.size == p -> p.size);
		init (p -> p.get());
		our sortedSelectedInversionIndices = sortedSelectedInversionIndices;
		our otherInverse = otherInverse -> p.get();
		numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
		our inversions = out_inversions;
		const integer numberOfInversions = countInversionsBySorting_ (1_integer, v.size);
		return numberOfInversions;
	}
};


#endif // _PermutationInversionCounter_h_
