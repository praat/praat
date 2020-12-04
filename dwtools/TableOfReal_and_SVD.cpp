/* TableOfReal_and_SVD.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20021009 GPL header
 djmw 20031107 Added TablesOfReal_to_GSVD.
 djmw 20051202 Extract left/right singular vectors.
*/

#include "TableOfReal_and_SVD.h"

autoTableOfReal SVD_to_TableOfReal (SVD me, integer from, integer to) {
	try {
		autoMAT synthesis = SVD_synthesize (me, from, to);
		autoTableOfReal thee = TableOfReal_create (synthesis.nrow, synthesis.ncol);
		thy data.all() <<=   synthesis.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no TableOfReal synthesized.");
	}
}

autoSVD TableOfReal_to_SVD (TableOfReal me) {
	try {
		autoSVD thee = SVD_createFromGeneralMatrix (my data.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no SVD created.");
	}
}

autoTableOfReal SVD_extractLeftSingularVectors (SVD me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		thy data.all() <<= my u.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": left singular vector not extracted.");
	}
}

autoTableOfReal SVD_extractRightSingularVectors (SVD me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfColumns, my numberOfColumns);
		thy data.all() <<= my v.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": right singular vector not extracted.");
	}
}

autoTableOfReal SVD_extractSingularValues (SVD me) {
	try {
		autoTableOfReal thee = TableOfReal_create (1, my numberOfColumns);
		thy data.row (1) <<= my d.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": singular values not extracted.");
	}
}

autoGSVD TablesOfReal_to_GSVD (TableOfReal me, TableOfReal thee) {
	try {
		Melder_require (my numberOfColumns == thy numberOfColumns,
			U"Both tables should have the same number of columns.");
		autoGSVD him = GSVD_create (my data.get(), thy data.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"GSVD not constructed from TablesOfReal.");
	}
}

/* End of file SVD_and_TableOfReal.cpp */
