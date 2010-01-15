/* Eigen_and_TableOfReal.c
 *
 * Copyright (C) 1993-2010 David Weenink
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
*/

#include "Eigen_and_TableOfReal.h"
#include "NUM2.h"

TableOfReal Eigen_and_TableOfReal_project (I, thou, long from,
	long numberOfComponents)
{
	iam (Eigen);
	thouart (TableOfReal);
	TableOfReal him;

	if (numberOfComponents == 0) numberOfComponents = my numberOfEigenvalues;

	him = TableOfReal_create (thy numberOfRows, numberOfComponents);
	if (him != NULL)
	{
		if (! Eigen_and_TableOfReal_project_into (me, thee, from,
			thy numberOfColumns, & him, 1, numberOfComponents) ||
			! NUMstrings_copyElements (thy rowLabels, his rowLabels,
				1, thy numberOfRows)) forget (him);
	}
	return him;
}

int Eigen_and_TableOfReal_project_into (I, thou, long thee_from, long thee_to,
	Any void_pointer_to_him, long his_from, long his_to)
{
	TableOfReal him = *((TableOfReal *)void_pointer_to_him);
	thouart (TableOfReal);
	iam (Eigen);
	long i, j, k, thee_ncols = thee_to - thee_from + 1;
	long his_ncols = his_to - his_from + 1;

	if (thee_from < 1 || thee_to > thy numberOfColumns ||
		his_from < 1 || his_to > his numberOfColumns) return Melder_error1
		(L"Column selection not correct.");
	if (thee_ncols != my dimension) return Melder_error5 (L"The number of "
		"selected columns to project (", Melder_integer (thee_ncols), L") must equal the dimension of the "
		"eigenvectors (", Melder_integer (my dimension), L").");
	if (his_ncols > my numberOfEigenvalues) return Melder_error5
		(L"The number of selected columns in the result (", Melder_integer (his_ncols), L") cannot exceed "
		"the number of eigenvectors (", Melder_integer (my numberOfEigenvalues), L").");

	for (i = 1; i <= thy numberOfRows; i++) /* row */
	{
		for (j = 1; j <= his_ncols; j++)
		{
			double r = 0;
			for (k = 1; k <= my dimension; k++)
			{
				/*
					eigenvector in row, data in row
				*/
				r += my eigenvectors[j][k] * thy data[i][thee_from + k - 1];
			}
			his data[i][his_from + j - 1] = r;
		}
	}
	return 1;
}

Eigen TablesOfReal_to_Eigen_gsvd (TableOfReal me, TableOfReal thee)
{
	Eigen him = new (Eigen);

	if (him == NULL) return NULL;

	if (my numberOfColumns != thy numberOfColumns) return Melder_errorp
		("TablesOfReal_to_Eigen: Number of columns must be equal.");

	if (! Eigen_initFromSquareRootPair (him, my data, my numberOfRows,
		my numberOfColumns, thy data, thy numberOfRows)) forget (him);
	return him;
}

/* End of file Eigen_and_TableOfReal.c */
