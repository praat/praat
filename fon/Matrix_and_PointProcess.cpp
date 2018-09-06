/* Matrix_and_PointProcess.cpp
 *
 * Copyright (C) 1992-2005,2011,2015-2018 Paul Boersma
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

#include "Matrix_and_PointProcess.h"

autoMatrix PointProcess_to_Matrix (PointProcess me) {
	try {
		if (my nt == 0)
			Melder_throw (U"No times in PointProcess.");
		autoMatrix you = Matrix_create (1.0, my nt, my nt, 1.0, 1.0, 1.0, 1.0, 1, 1.0, 1.0);
		for (integer i = 1; i <= my nt; i ++)
			your z [1] [i] = my t [i];
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoPointProcess Matrix_to_PointProcess (Matrix me) {
	try {
		autoPointProcess you = PointProcess_create (my z [1] [1], my z [1] [my nx], my nx);
		for (integer i = 1; i <= my nx; i ++)
			PointProcess_addPoint (you.get(), my z [1] [i]);
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PointProcess.");
	}
}

/* End of file Matrix_and_PointProcess.cpp */
