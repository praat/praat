/* Eigen_and_TableOfReal.cpp
 *
 * Copyright (C) 1993-2012, 2015 David Weenink
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
 djmw 20020327 Initial version.
 djmw 20020429 _project: copy rowLabels too.
 djmw 20020502 Added from index to projections.
 djmw 20020813 GPL header
 djmw 20110304 Thing_new
*/

#include "Eigen_and_TableOfReal.h"
#include "NUM2.h"

autoTableOfReal Eigen_and_TableOfReal_project (Eigen me, TableOfReal thee, long from, long numberOfComponents) {
	try {
		if (numberOfComponents == 0) {
			numberOfComponents = my numberOfEigenvalues;
		}

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfComponents);
		Eigen_and_TableOfReal_project_into (me, thee, from, thy numberOfColumns, him.peek(), 1, numberOfComponents);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from projection.");
	}
}

void Eigen_and_TableOfReal_project_into (Eigen me, TableOfReal thee, long thee_from, long thee_to, TableOfReal him, long his_from, long his_to) {
	long thee_ncols = thee_to - thee_from + 1;
	long his_ncols = his_to - his_from + 1;

	if (thee_from < 1 || thee_to > thy numberOfColumns || his_from < 1 || his_to > his numberOfColumns) {
		Melder_throw (U"Column selection not correct.");
	}
	if (thee_ncols != my dimension) {
		Melder_throw (U"The number of selected columns to project (", thee_ncols, U") must equal the dimension of the eigenvectors (", my dimension, U").");
	}
	if (his_ncols > my numberOfEigenvalues) {
		Melder_throw (U"The number of selected columns in the result (", his_ncols, U") cannot exceed the number of eigenvectors (", my numberOfEigenvalues, U").");
	}

	for (long i = 1; i <= thy numberOfRows; i++) { /* row */
		for (long j = 1; j <= his_ncols; j++) {
			double r = 0;
			for (long k = 1; k <= my dimension; k++) {
				/*
					eigenvector in row, data in row
				*/
				r += my eigenvectors[j][k] * thy data[i][thee_from + k - 1];
			}
			his data[i][his_from + j - 1] = r;
		}
	}
}

autoEigen TablesOfReal_to_Eigen_gsvd (TableOfReal me, TableOfReal thee) {
	try {
		if (my numberOfColumns != thy numberOfColumns) {
			Melder_throw (U"TablesOfReal_to_Eigen: Number of columns must be equal.");
		}
		autoEigen him = Thing_new (Eigen);
		Eigen_initFromSquareRootPair (him.peek(), my data, my numberOfRows, my numberOfColumns, thy data, thy numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": Eigen not created.");
	}
}

/* End of file Eigen_and_TableOfReal.cpp */
