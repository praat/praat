/* TableOfReal_and_Permutation.cpp
 *
 * Copyright (C) 2005-2022 David Weenink
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
 djmw 20050708
*/

#include "Index.h"
#include "TableOfReal_and_Permutation.h"
#include "TableOfReal_extensions.h"
#include "NUM2.h"

autoTableOfReal TableOfReal_Permutation_permuteRows (TableOfReal me, Permutation thee) {
	try {
		Melder_require (my numberOfRows == thy numberOfElements,
			U"The number of rows in the table and the number of elements in the Permutation should be equal.");
		autoTableOfReal him = TableOfReal_create (my numberOfRows, my numberOfColumns);

		for (integer irow = 1; irow <= thy numberOfElements; irow ++)
			TableOfReal_copyOneRowWithLabel (me, him.get(), thy p [irow], irow);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			TableOfReal_setColumnLabel (him.get(), icol, my columnLabels [icol].get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": rows not permuted.");
	}
}

autoTableOfReal TableOfReal_Permutation_permuteColumns (TableOfReal me, Permutation thee) {
	try {
		Melder_require (my numberOfColumns == thy numberOfElements,
			U"The number of columns in the table and the number of elements in the Permutation should be equal.");
		autoTableOfReal him = TableOfReal_create (my numberOfRows, my numberOfColumns);

		for (integer icol = 1; icol <= thy numberOfElements; icol ++) {
			const integer fromCol = thy p [icol];
			his columnLabels [icol] = Melder_dup (my columnLabels [fromCol].get());
			for (integer irow = 1; irow <= my numberOfRows; irow ++)
				his data [irow] [icol] = my data [irow][fromCol];
		}
		for (integer irow = 1; irow <= my numberOfColumns; irow ++)
			TableOfReal_setRowLabel (him.get(), irow, my rowLabels [irow].get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": columns not permuted.");
	}
	
}
autoPermutation TableOfReal_to_Permutation_sortRowLabels (TableOfReal me) {
	try {
		autoPermutation thee = Permutation_create (my numberOfRows, true);
		INTVECindex_inout (thy p.get(), my rowLabels.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Permutation created.");
	}
}

/* End of file TableOfReal_and_Permutation.cpp */
