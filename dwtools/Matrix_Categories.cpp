/* Matrix_Categories.cpp
 *
 * Copyright (C) 1993-2019 David Weenink, 2018 Paul Boersma
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

#include "Matrix_Categories.h"
#include "TableOfReal_extensions.h"

autoTableOfReal Matrix_Categories_to_TableOfReal (Matrix me, Categories thee) {
	try {
		Melder_require (thy size == my ny,
			U"Number of rows and number of categories should be equal.");

		autoTableOfReal him = TableOfReal_create (my ny, my nx);
		TableOfReal_setSequentialColumnLabels (him.get(), 0, 0, nullptr, 1, 1);

		for (integer i = 1; i <= my ny; i ++) {
			const SimpleString category = thy at [i];
			his rowLabels [i] = Melder_dup (category -> string.get());
		}
		
		his data.all()  <<=  my z.all();
		
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from Matrix & Categories.");
	}
}

/* End of file Matrix_Categories.cpp */
