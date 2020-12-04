/* ActivationList.cpp
 *
 * Copyright (C) 1993-2018 David Weenink
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

#include "ActivationList.h"

Thing_implement (ActivationList, Matrix, 2);

int _ActivationList_checkElements (ActivationList me) {
	for (integer i = 1; i <= my ny; i ++) {
		for (integer j = 1; j <= my nx; j ++) {
			if (my z[i][j] < 0.0 || my z [i] [j] > 1.0) {
				return 0;
			}
		}
	}
	return 1;
}

void ActivationList_init (ActivationList me, integer ny, integer nx) {
	Matrix_init (me, 1.0, nx, nx, 1.0, 1.0, 1.0, ny, ny, 1.0, 1.0);
}

autoActivationList ActivationList_create (integer ny, integer nx) {
	try {
		autoActivationList me = Thing_new (ActivationList);
		ActivationList_init (me.get(), ny, nx);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Activation not created.");
	}
}

autoActivationList Matrix_to_ActivationList (Matrix me) {
	try {
		autoActivationList thee = ActivationList_create (my ny, my nx);
		thy z.all()  <<=  my z.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ActivationList.");
	}
}

autoMatrix ActivationList_to_Matrix (ActivationList me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of file ActivationList.cpp */
