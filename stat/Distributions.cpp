/* Distributions.cpp
 *
 * Copyright (C) 1997-2012 Paul Boersma
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

#include "Distributions.h"

Thing_implement (Distributions, TableOfReal, 0);

void structDistributions :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of distributions: ", Melder_integer (numberOfColumns));
	MelderInfo_writeLine (L"Number of values: ", Melder_integer (numberOfRows));
}

Distributions Distributions_create (long numberOfRows, long numberOfColumns) {
	try {
		autoDistributions me = Thing_new (Distributions);
		TableOfReal_init (me.peek(), numberOfRows, numberOfColumns);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Distributions not created.");
	}
}

void Distributions_checkSpecifiedColumnNumberWithinRange (Distributions me, long columnNumber) {
	if (columnNumber < 1)
		Melder_throw (me, ": the specified column number is ", columnNumber, ", but should be at least 1.");
	if (columnNumber > my numberOfColumns)
		Melder_throw (me, ": the specified column number is ", columnNumber, ", but should be at most my number of columns (", my numberOfColumns, ").");
}

int Distributions_peek (Distributions me, long column, wchar_t **string) {
	Distributions_checkSpecifiedColumnNumberWithinRange (me, column);
	if (my numberOfRows < 1)
		Melder_throw (me, ": I have no candidates.");
	double total = 0.0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		total += my data [irow] [column];
	}
	if (total <= 0.0)
		Melder_throw (me, ": the total weight of column ", column, " is not positive.");
	long irow;
	do {
		double rand = NUMrandomUniform (0, total), sum = 0.0;
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			sum += my data [irow] [column];
			if (rand <= sum) break;
		}
	} while (irow > my numberOfRows);   // guard against rounding errors
	*string = my rowLabels [irow];
	if (! *string)
		Melder_throw (me, ": no string in row ", irow, ".");
	return 1;
}

int Distributions_peek_opt (Distributions me, long column, long *number) {
	Distributions_checkSpecifiedColumnNumberWithinRange (me, column);
	if (my numberOfRows < 1)
		Melder_throw (me, ": I have no candidates.");
	double total = 0.0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		total += my data [irow] [column];
	}
	if (total <= 0.0)
		Melder_throw (me, ": the total weight of column ", column, " is not positive.");
	long irow;
	do {
		double rand = NUMrandomUniform (0, total), sum = 0.0;
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			sum += my data [irow] [column];
			if (rand <= sum) break;
		}
	} while (irow > my numberOfRows);   /* Guard against rounding errors. */
	if (my rowLabels [irow] == NULL)
		Melder_throw (me, ": no string in row ", irow, ".");
	*number = irow;
	return 1;
}

double Distributions_getProbability (Distributions me, const wchar_t *string, long column) {
	long row, rowOfString = 0;
	double total = 0.0;
	if (column < 1 || column > my numberOfColumns) return NUMundefined;
	for (row = 1; row <= my numberOfRows; row ++) {
		total += my data [row] [column];
		if (my rowLabels [row] && wcsequ (my rowLabels [row], string))
			rowOfString = row;
	}
	if (total <= 0.0) return NUMundefined;
	if (rowOfString == 0) return 0.0;
	return my data [rowOfString] [column] / total;
}

double Distributionses_getMeanAbsoluteDifference (Distributions me, Distributions thee, long column) {
	if (column < 1 || column > my numberOfColumns || column > thy numberOfColumns ||
	    my numberOfRows != thy numberOfRows) return NUMundefined;
	double total = 0.0;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		total += fabs (my data [irow] [column] - thy data [irow] [column]);
	}
	return total / my numberOfRows;
}

static void unicize (Distributions me) {
	/* Must have been sorted beforehand. */
	long nrow = 0, ifrom = 1;
	for (long irow = 1; irow <= my numberOfRows; irow ++) {
		if (irow == my numberOfRows || (my rowLabels [irow] == NULL) != (my rowLabels [irow + 1] == NULL) ||
		    (my rowLabels [irow] != NULL && ! wcsequ (my rowLabels [irow], my rowLabels [irow + 1])))
		{
			/*
			 * Detected a change.
			 */
			nrow ++;
			long ito = irow;
			/*
			 * Move row 'ifrom' to 'nrow'. May be the same row.
			 */
			if (ifrom != nrow) {
				Melder_free (my rowLabels [nrow]);
				my rowLabels [nrow] = my rowLabels [ifrom];   /* Surface copy. */
				my rowLabels [ifrom] = NULL;   /* Undangle. */
				for (long icol = 1; icol <= my numberOfColumns; icol ++)
					my data [nrow] [icol] = my data [ifrom] [icol];
			}
			/*
			 * Purge rows from 'ifrom'+1 to 'ito'.
			 */
			for (long j = ifrom + 1; j <= ito; j ++) {
				Melder_free (my rowLabels [j]);
				for (long icol = 1; icol <= my numberOfColumns; icol ++)
					my data [nrow] [icol] += my data [j] [icol];
			}
			ifrom = ito + 1;
		}
	}
	my numberOfRows = nrow;
}

Distributions Distributions_addTwo (Distributions me, Distributions thee) {
	try {
		autoDistributions him = static_cast<Distributions> (TablesOfReal_append (me, thee));
		TableOfReal_sortByLabel (him.peek(), 0, 0);
		unicize (him.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, " & ", thee, ": not added.");
	}
}

Distributions Distributions_addMany (Collection me) {
	try {
		autoDistributions thee = static_cast<Distributions> (TablesOfReal_appendMany (me));
		TableOfReal_sortByLabel (thee.peek(), 0, 0);
		unicize (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Distributions objects not added.");
	}
}

/* End of file Distributions.cpp */
