/* TableOfReal_and_Permutation.cpp
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20050708
*/

#include "TableOfReal_and_Permutation.h"
#include "TableOfReal_extensions.h"
#include "NUM2.h"

TableOfReal TableOfReal_and_Permutation_permuteRows (I, Permutation thee) {
	iam (TableOfReal);
	try {
		if (my numberOfRows != thy numberOfElements) {
			Melder_throw (L"The number of rows in the table and the number of elements in the Permutation must be equal.");
		}
		autoTableOfReal him = TableOfReal_create (my numberOfRows, my numberOfColumns);

		for (long i = 1; i <= thy numberOfElements; i++) {
			TableOfReal_copyOneRowWithLabel (me, him.peek(), thy p[i], i);
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not permuted.");
	}
}

Permutation TableOfReal_to_Permutation_sortRowLabels (I) {
	iam (TableOfReal);
	try {
		autoPermutation thee = Permutation_create (my numberOfRows);
		NUMindexx_s (my rowLabels, my numberOfRows, thy p);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Permutation created.");
	}
}

/* End of file TableOfReal_and_Permutation.cpp */
