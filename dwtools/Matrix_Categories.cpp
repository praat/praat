/* Matrix_Categories.cpp
 *
 * Copyright (C) 1993-2011,2015 David Weenink
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

#include "Matrix_Categories.h"
#include "TableOfReal_extensions.h"

autoTableOfReal Matrix_and_Categories_to_TableOfReal (Matrix me, Categories thee) {
	try {
		if (thy size != my ny) {
			Melder_throw (U"Number of rows and number of categories must be equal.");
		}

		autoTableOfReal him = TableOfReal_create (my ny, my nx);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, nullptr, 1, 1);

		for (long i = 1; i <= my ny; i ++) {
			his rowLabels [i] = Melder_dup (OrderedOfString_itemAtIndex_c (thee, i));
		}

		for (long i = 1; i <= my ny; i ++) {
			for (long j = 1; j <= my nx; j ++) {
				his data [i] [j] = my z [i] [j];
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from Matrix & Categories.");
	}
}

/* End of file Matrix_Categories.cpp */
