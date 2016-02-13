/* Matrix_and_PointProcess.cpp
 *
 * Copyright (C) 1992-2011,2015,2016 Paul Boersma
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

#include "Matrix_and_PointProcess.h"

autoMatrix PointProcess_to_Matrix (PointProcess me) {
	try {
		if (my nt == 0)
			Melder_throw (U"No times in PointProcess.");
		autoMatrix thee = Matrix_create (1.0, my nt, my nt, 1.0, 1.0, 1.0, 1.0, 1, 1.0, 1.0);
		for (long i = 1; i <= my nt; i ++)
			thy z [1] [i] = my t [i];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoPointProcess Matrix_to_PointProcess (Matrix me) {
	try {
		autoPointProcess thee = PointProcess_create (my z [1] [1], my z [1] [my nx], my nx);
		for (long i = 1; i <= my nx; i ++) {
			PointProcess_addPoint (thee.get(), my z [1] [i]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PointProcess.");
	}
}

/* End of file Matrix_and_PointProcess.cpp */
