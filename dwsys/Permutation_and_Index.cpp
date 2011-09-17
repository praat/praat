/* Permutation_and_Index.cpp
 *
 * Copyright (C) 2005-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * djmw 20050725
 * djmw 20110503 Latest modification
 */


#include "Permutation_and_Index.h"

Permutation Index_to_Permutation_permuteRandomly (I, int permuteWithinClasses) {
	iam (Index);
	try {
		long numberOfClasses = my classes -> size;

		autoPermutation thee = Permutation_create (my numberOfElements);
		autoPermutation classes = Permutation_create (numberOfClasses);
		Permutation_permuteRandomly_inline (classes.peek(), 0, 0);
		autoPermutation classesinv = Permutation_invert (classes.peek());
		autoNUMmatrix<long> indices (0, numberOfClasses, 1, 4);

		for (long i = 1; i <= my numberOfElements; i++) {
			indices[my classIndex[i]][2]++; /* col 2: number of elements in class */
		}
		/* Get some other indices ready */
		for (long i = 1; i <= numberOfClasses; i++) {
			long klass = classes -> p[i];
			indices[i][1] = klass;
			indices[i][3] = indices[i - 1][3] + indices[i - 1][2]; /* col 3: index at start of class */
		}

		for (long i = 1; i <= my numberOfElements; i++) {
			long klass = my classIndex[i];
			long newindex = classesinv -> p[klass];
			indices[newindex][4]++; /* col 4: number of elements processed for class */
			long newpos = indices[newindex][3] + indices[newindex][4];
			thy p[newpos] = i;
		}
		if (permuteWithinClasses) {
			for (long i = 1; i <= numberOfClasses; i++) {
				long from = indices[i][3] + 1;
				long to = from + indices[i][2] - 1;
				if (to > from) {
					Permutation_permuteRandomly_inline (thee.peek(), from, to);
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Permutation not created.");
	}
}

/*  End of file Permutation_and_Index.cpp */
