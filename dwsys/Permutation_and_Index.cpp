/* Permutation_and_Index.cpp
 *
 * Copyright (C) 2005-2019 David Weenink
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

/*
 * djmw 20050725
 * djmw 20110503 Latest modification
 */


#include "Permutation_and_Index.h"

autoPermutation Index_to_Permutation_permuteRandomly (Index me, bool permuteWithinClasses) {
	try {
		const integer numberOfClasses = my classes -> size;

		autoPermutation thee = Permutation_create (my numberOfItems);
		autoPermutation classes = Permutation_create (numberOfClasses);
		Permutation_permuteRandomly_inplace (classes.get(), 0, 0);
		autoPermutation classesinv = Permutation_invert (classes.get());
		autoINTMAT indices = zero_INTMAT (numberOfClasses, 4);

		for (integer i = 1; i <= my numberOfItems; i ++)
			indices [my classIndex [i]] [2] ++;   // column 2: number of elements in class

		/* Get some other indices ready */
		for (integer i = 1; i <= numberOfClasses; i ++) {
			const integer klass = classes -> p [i];
			indices [i] [1] = klass;
			indices [i] [3] = ( i == 1 ? 0 : indices [i - 1] [3] + indices [i - 1] [2] );   // column 3: index at start of class
		}

		for (integer i = 1; i <= my numberOfItems; i ++) {
			const integer klass = my classIndex [i];
			const integer newindex = classesinv -> p [klass];
			indices [newindex] [4] ++;   // column 4: number of elements processed for class
			const integer newpos = indices [newindex] [3] + indices [newindex] [4];
			thy p [newpos] = i;
		}
		if (permuteWithinClasses) {
			for (integer i = 1; i <= numberOfClasses; i ++) {
				const integer from = indices [i] [3] + 1;
				const integer to = from + indices [i] [2] - 1;
				if (to > from)
					Permutation_permuteRandomly_inplace (thee.get(), from, to);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Permutation not created.");
	}
}

/*  End of file Permutation_and_Index.cpp */
