/* Eigen_and_TableOfReal.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20020327 Initial version.
 djmw 20020429 _project: copy rowLabels too.
 djmw 20020502 Added from index to projections.
 djmw 20020813 GPL header
 djmw 20110304 Thing_new
*/

#include "Eigen_and_TableOfReal.h"
#include "NUM2.h"

autoTableOfReal Eigen_TableOfReal_to_TableOfReal_projectRows (Eigen me, TableOfReal thee, integer from_col, integer numberOfComponents) {
	try {
		if (numberOfComponents <= 0 || numberOfComponents > my numberOfEigenvalues) {
			numberOfComponents = my numberOfEigenvalues;
		}

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfComponents);
		Eigen_TableOfReal_into_TableOfReal_projectRows (me, thee, from_col, him.get(), 1, numberOfComponents);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from projection.");
	}
}

void Eigen_TableOfReal_into_TableOfReal_projectRows (Eigen me, TableOfReal data, integer data_startColumn, TableOfReal to, integer to_startColumn, integer numberOfComponentsToKeep) {

	data_startColumn = data_startColumn <= 0 ? 1 : data_startColumn;
	to_startColumn = to_startColumn <= 0 ? 1 : to_startColumn;
	numberOfComponentsToKeep = numberOfComponentsToKeep <= 0 ? my numberOfEigenvalues : numberOfComponentsToKeep;
	
	Melder_require (data_startColumn + my dimension - 1 <= data -> numberOfColumns, U"Your start column in the table is too large.");
	Melder_require (to_startColumn + numberOfComponentsToKeep - 1 <= to -> numberOfColumns, U" Your start column in the 'to' matrix is too large.");
	Melder_require (data -> numberOfRows == to -> numberOfRows, U"Both tables should have the same number of rows.");
	
	NUMdmatrix_projectRowsOnEigenspace (data -> data, data -> numberOfRows, data_startColumn, my eigenvectors, numberOfComponentsToKeep, my dimension, to -> data, to_startColumn);
}

autoEigen TablesOfReal_to_Eigen_gsvd (TableOfReal me, TableOfReal thee) {
	try {
		Melder_require (my numberOfColumns == thy numberOfColumns, U"Both tables should have the same number of columns.");
		autoEigen him = Thing_new (Eigen);
		Eigen_initFromSquareRootPair (him.get(), my data, my numberOfRows, my numberOfColumns, thy data, thy numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": Eigen not created.");
	}
}

/* End of file Eigen_and_TableOfReal.cpp */
