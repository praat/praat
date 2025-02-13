/* PermutationInversionCounter_def.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

#define ooSTRUCT PermutationInversionCounter
oo_DEFINE_CLASS (PermutationInversionCounter, Daata)
	oo_INTEGER (numberOfElements)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Permutation, pdata)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Permutation, potherInverse)
	oo_OBJECT (Permutation, 0, psortingOrder)
	oo_INTVEC (workspace, numberOfElements)
	oo_INTEGER (totalNumberOfInversions)
	oo_INTEGER (totalNumberOfInversionsInInterval)
	oo_INTEGER (numberOfInversionsRegistered)
	oo_INTEGER (numberOfInversionsToRegister)
	oo_INTEGER (posInSortedSelectedInversionIndices)
	#if oo_DECLARING
		INTVEC inversions;			// the coded inversions to be output, link to storage supplied by caller
		constINTVEC sortedSelectedInversionIndices; // link to storage supplied by caller
		
		void newData (constPermutation p) {
			Melder_assert (p -> numberOfElements == our numberOfElements);
			pdata = p;
		}
	#endif
	#if oo_COPYING
		thy inversions = inversions;
		thy sortedSelectedInversionIndices = sortedSelectedInversionIndices;
	#endif

oo_END_CLASS (PermutationInversionCounter)
#undef ooSTRUCT
