/* Eigen_and_TableOfReal.cpp
 *
 * Copyright (C) 1993-2012, 2015-2016 David Weenink
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

autoTableOfReal Eigen_and_TableOfReal_to_TableOfReal_projectRows (Eigen me, TableOfReal thee, long from_col, long numberOfComponents) {
	try {
		if (numberOfComponents <= 0 || numberOfComponents > my numberOfEigenvalues) {
			numberOfComponents = my numberOfEigenvalues;
		}

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, numberOfComponents);
		Eigen_and_TableOfReal_into_TableOfReal_projectRows (me, thee, from_col, him.get(), 1, numberOfComponents);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created from projection.");
	}
}

void Eigen_and_TableOfReal_into_TableOfReal_projectRows (Eigen me, TableOfReal data, long data_startColumn, TableOfReal to, long to_startColumn, long numberOfComponentsToKeep) {

	data_startColumn = data_startColumn <= 0 ? 1 : data_startColumn;
	to_startColumn = to_startColumn <= 0 ? 1 : to_startColumn;
	numberOfComponentsToKeep = numberOfComponentsToKeep <= 0 ? my numberOfEigenvalues : numberOfComponentsToKeep;
	if (data_startColumn + my dimension - 1 > data -> numberOfColumns) {
		Melder_throw (data, U" Your start column in the table is too large.");
	}
	if (to_startColumn + numberOfComponentsToKeep - 1 > to -> numberOfColumns) {
		Melder_throw (to, U" Your start column in the 'to' matrix is too large.");
	}
	if (data -> numberOfRows != to -> numberOfRows) {
		Melder_throw (U"Both tables must have the same number of rows.");
	}
	NUMdmatrix_projectRowsOnEigenspace (data -> data, data -> numberOfRows, data_startColumn, my eigenvectors, numberOfComponentsToKeep, my dimension, to -> data, to_startColumn);
}

autoEigen TablesOfReal_to_Eigen_gsvd (TableOfReal me, TableOfReal thee) {
	try {
		if (my numberOfColumns != thy numberOfColumns) {
			Melder_throw (U"TablesOfReal_to_Eigen: Number of columns must be equal.");
		}
		autoEigen him = Thing_new (Eigen);
		Eigen_initFromSquareRootPair (him.get(), my data, my numberOfRows, my numberOfColumns, thy data, thy numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": Eigen not created.");
	}
}

/* End of file Eigen_and_TableOfReal.cpp */
