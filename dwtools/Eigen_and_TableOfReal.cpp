/* Eigen_and_TableOfReal.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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

autoEigen TablesOfReal_to_Eigen_gsvd (TableOfReal me, TableOfReal thee) {
	try {
		Melder_require (my numberOfColumns == thy numberOfColumns,
			U"Both tables should have the same number of columns.");
		autoEigen him = Thing_new (Eigen);
		Eigen_initFromSquareRootPair (him.get(), my data.get(), thy data.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": Eigen not created.");
	}
}

/* End of file Eigen_and_TableOfReal.cpp */
