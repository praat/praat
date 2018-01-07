/* Eigen_and_Matrix.cpp
 *
 * Copyright (C) 1993-2017 David Weenink, 2015,2017 Paul Boersma
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
 djmw 2002
 djmw 20020813 GPL header
*/

#include "Eigen_and_Matrix.h"
#include "Matrix_extensions.h"
#include "NUM2.h"


autoMatrix Eigen_extractEigenvector (Eigen me, integer index, integer numberOfRows, integer numberOfColumns) {
	try {
		if (numberOfRows == 0 && numberOfColumns == 0) {
			numberOfRows = 1; numberOfColumns = my dimension;
		}
		if (numberOfRows == 0) {
			numberOfRows = Melder_iceiling ((double) my dimension / numberOfColumns);
		} else if (numberOfColumns == 0) {
			numberOfColumns = Melder_iceiling ((double) my dimension / numberOfRows);
		}
		autoMatrix thee = Matrix_createSimple (numberOfRows, numberOfColumns);
		integer i = 1;
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			for (integer icol = 1; icol <= numberOfColumns; icol ++) {
				thy z [irow] [icol] = i <= my dimension ? my eigenvectors [index] [i ++] : 0.0;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"No eigenvector extracted.");
	}
}

autoMatrix Eigen_Matrix_to_Matrix_projectRows (Eigen me, Matrix thee, integer numberOfDimensionsToKeep) {
	try {
		if (numberOfDimensionsToKeep <= 0 || numberOfDimensionsToKeep > my numberOfEigenvalues) {
			numberOfDimensionsToKeep = my numberOfEigenvalues;
		}
		Melder_require (thy nx == my dimension, U"The number of columns (", thy nx, U") should equal the size of the eigenvectors (", my dimension, U").");
		
		autoMatrix him = Matrix_create (0.5, 0.5 + numberOfDimensionsToKeep, numberOfDimensionsToKeep, 1.0, 1.0, thy ymin, thy ymax, thy ny, thy dy, thy y1);
		NUMdmatrix_projectRowsOnEigenspace (thy z, thy ny, 1, my eigenvectors, numberOfDimensionsToKeep, my dimension, his z, 1);
		//Eigen_and_matrix_into_matrix_principalComponents (me, thy z, thy ny, 1, his z, numberOfDimensionsToKeep, 1);
		
		return him;
	} catch (MelderError) {
		Melder_throw (U"Projection Matrix from ", me, U" and ", thee, U" not created.");
	}
}

autoMatrix Eigen_Matrix_to_Matrix_projectColumns (Eigen me, Matrix thee, integer numberOfDimensionsToKeep) {
	try {
		if (numberOfDimensionsToKeep <= 0 || numberOfDimensionsToKeep > my numberOfEigenvalues) {
			numberOfDimensionsToKeep = my numberOfEigenvalues;
		}
		Melder_require (thy nx == my dimension, U"The number of rows (", thy ny, U") should equal the size of the eigenvectors (", my dimension, U").");
		
		autoMatrix him = Matrix_create (thy xmin, thy xmax, thy nx, thy dx, thy x1, 0.5, 0.5 + numberOfDimensionsToKeep, numberOfDimensionsToKeep, 1.0, 1.0);
		NUMdmatrix_projectColumnsOnEigenspace (thy z, thy nx, my eigenvectors, numberOfDimensionsToKeep, my dimension, his z);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Projection Matrix from ", me, U" and ", thee, U" not created.");
	}
}

/* End of file Eigen_and_Matrix.cpp */
