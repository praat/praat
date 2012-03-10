/* Activation.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20020315 GPL header
 djmw 20041203 Added _Activation_checkElements.
 djmw 20080122 float -> double
 djmw 20110304 Thing_new
 */

#include "Activation.h"

Thing_implement (Activation, Matrix, 2);

int _Activation_checkElements (Activation me) {
	for (long i = 1; i <= my ny; i++) {
		for (long j = 1; j <= my nx; j++) {
			if (my z[i][j] < 0 || my z[i][j] > 1) {
				return 0;
			}
		}
	}
	return 1;
}

void Activation_init (I, long ny, long nx) {
	iam (Activation);
	double xmin = 1, xmax = nx, dx = 1, x1 = 1, ymin = 1, ymax = ny;
	double dy = 1, y1 = 1;
	my ny = ny; my nx = nx;
	Matrix_init (me, xmin, xmax, nx, dx, x1, ymin, ymax, ny, dy, y1);
}

Activation Activation_create (long ny, long nx) {
	try {
		autoActivation me = Thing_new (Activation);
		Activation_init (me.peek(), ny, nx);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Activation not created.");
	}
}

Activation Matrix_to_Activation (I) {
	iam (Matrix);
	try {
		autoActivation thee = Activation_create (my ny, my nx);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Activation.");
	}
}

Matrix Activation_to_Matrix (I) {
	iam (Activation);
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		                                 my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

/* End of file Activation.cpp */
