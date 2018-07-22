/* Distributions.cpp
 *
 * Copyright (C) 1997-2012,2014,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Distributions.h"

Thing_implement (Distributions, TableOfReal, 0);

void structDistributions :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of distributions: ", numberOfColumns);
	MelderInfo_writeLine (U"Number of values: ", numberOfRows);
}

autoDistributions Distributions_create (integer numberOfRows, integer numberOfColumns) {
	try {
		autoDistributions me = Thing_new (Distributions);
		TableOfReal_init (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Distributions not created.");
	}
}

void Distributions_checkSpecifiedColumnNumberWithinRange (Distributions me, integer columnNumber) {
	if (columnNumber < 1)
		Melder_throw (me, U": the specified column number is ", columnNumber, U", but should be at least 1.");
	if (columnNumber > my numberOfColumns)
		Melder_throw (me, U": the specified column number is ", columnNumber, U", but should be at most my number of columns (", my numberOfColumns, U").");
}

void Distributions_peek (Distributions me, integer column, conststring32 *out_string, integer *out_number) {
	Distributions_checkSpecifiedColumnNumberWithinRange (me, column);
	if (my numberOfRows < 1)
		Melder_throw (me, U": I have no candidates.");
	longdouble total = 0.0;
	for (integer irow = 1; irow <= my numberOfRows; irow ++) {
		total += (longdouble) my data [irow] [column];
	}
	if (total <= 0.0)
		Melder_throw (me, U": the total weight of column ", column, U" is not positive.");
	integer irow;
	do {
		double rand = NUMrandomUniform (0, (double) total);
		longdouble sum = 0.0;
		for (irow = 1; irow <= my numberOfRows; irow ++) {
			sum += my data [irow] [column];
			if (rand <= sum) break;
		}
	} while (irow > my numberOfRows);   // guard against rounding errors
	if (! my rowLabels [irow])
		Melder_throw (me, U": no string in row ", irow, U".");
	if (out_string)
		*out_string = my rowLabels [irow].get();
	if (out_number)
		*out_number = irow;
}

double Distributions_getProbability (Distributions me, conststring32 string, integer column) {
	integer row, rowOfString = 0;
	longdouble total = 0.0;
	if (column < 1 || column > my numberOfColumns) return undefined;
	for (row = 1; row <= my numberOfRows; row ++) {
		total += my data [row] [column];
		if (my rowLabels [row] && str32equ (my rowLabels [row].get(), string))
			rowOfString = row;
	}
	if (total <= 0.0) return undefined;
	if (rowOfString == 0) return 0.0;
	return my data [rowOfString] [column] / (double) total;
}

double Distributionses_getMeanAbsoluteDifference (Distributions me, Distributions thee, integer column) {
	if (column < 1 || column > my numberOfColumns || column > thy numberOfColumns ||
	    my numberOfRows != thy numberOfRows) return undefined;
	longdouble total = 0.0;
	for (integer irow = 1; irow <= my numberOfRows; irow ++) {
		total += (longdouble) fabs (my data [irow] [column] - thy data [irow] [column]);
	}
	return (double) total / my numberOfRows;
}

static void unicize (Distributions me) {
	/* Must have been sorted beforehand. */
	integer nrow = 0, ifrom = 1;
	for (integer irow = 1; irow <= my numberOfRows; irow ++) {
		if (irow == my numberOfRows || !! my rowLabels [irow] != !! my rowLabels [irow + 1] ||
		    my rowLabels [irow] && ! str32equ (my rowLabels [irow].get(), my rowLabels [irow + 1].get()))
		{
			/*
				Detected a change.
			*/
			nrow ++;
			integer ito = irow;
			/*
				Move row 'ifrom' to 'nrow'. May be the same row.
			*/
			if (ifrom != nrow) {
				my rowLabels [nrow] = my rowLabels [ifrom].move();
				for (integer icol = 1; icol <= my numberOfColumns; icol ++)
					my data [nrow] [icol] = my data [ifrom] [icol];
			}
			/*
				Purge rows from 'ifrom'+1 to 'ito'.
			*/
			for (integer j = ifrom + 1; j <= ito; j ++) {
				my rowLabels [j]. reset();
				for (integer icol = 1; icol <= my numberOfColumns; icol ++)
					my data [nrow] [icol] += my data [j] [icol];
			}
			ifrom = ito + 1;
		}
	}
	my numberOfRows = nrow;
}

autoDistributions Distributions_addTwo (Distributions me, Distributions thee) {
	try {
		autoDistributions him = TablesOfReal_append (me, thee).static_cast_move<structDistributions>();
		TableOfReal_sortByLabel (him.get(), 0, 0);
		unicize (him.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not added.");
	}
}

autoDistributions Distributions_addMany (OrderedOf<structDistributions>* me) {
	try {
		autoDistributions thee = TablesOfReal_appendMany ((OrderedOf<structTableOfReal>*) me).static_cast_move<structDistributions>();   // FIXME cast
		TableOfReal_sortByLabel (thee.get(), 0, 0);
		unicize (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Distributions objects not added.");
	}
}

/* End of file Distributions.cpp */
