/* Eigen_and_Matrix.cpp
 *
 * Copyright (C) 1993-2011,2015 David Weenink, 2015 Paul Boersma
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
 djmw 2002
 djmw 20020813 GPL header
*/

#include "Eigen_and_Matrix.h"

autoMatrix Eigen_and_Matrix_project (Eigen me, Matrix thee, long numberOfComponents) {
	try {
		if (numberOfComponents == 0) {
			numberOfComponents = my numberOfEigenvalues;
		}
		autoMatrix him = Matrix_create (thy xmin, thy xmax, thy nx, thy dx, thy x1, 0.5, 0.5 + numberOfComponents, numberOfComponents, 1.0, 1.0);
		Eigen_and_Matrix_project_into (me, thee, him.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Projection Matrix not created.");
	}
}

void Eigen_and_Matrix_project_into (Eigen me, Matrix thee, Matrix him)
{
	if (my dimension != thy ny) {
		Melder_throw (U"The number of rows in the 'from' Matrix must equal the dimension of the eigenvector.");
	}
	if (his nx != thy nx) {
		Melder_throw (U"The number of columns in the Matrixes must be equal.");
	}
	if (his ny > my numberOfEigenvalues) {
		Melder_throw (U"The number of rows in the 'to' Matrix cannot exceed the number of eigenvectors.");
	}
	for (long i = 1; i <= thy nx; i++) {
		for (long j = 1; j <= his ny; j++) {
			double r = 0.0;
			for (long k = 1; k <= my dimension; k++) {
				// eigenvector in row, data in column   // ppgb: "row" of what? "data" means what? "column" of what?
				r += my eigenvectors[j][k] * thy z[k][i];
			}
			his z[j][i] = r;
		}
	}
}

/* End of file Eigen_and_Matrix.cpp */
