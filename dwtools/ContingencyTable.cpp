/* ContingencyTable.cpp
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

#include "ContingencyTable.h"
#include "TableOfReal_extensions.h"
#include "NUM2.h"

Thing_implement (ContingencyTable, TableOfReal, 0);

#define TINY 1e-30

void structContingencyTable :: v_info () {
	structDaata :: v_info ();

	double ndf;
	double h, hx, hy, hygx, hxgy, uygx, uxgy, uxy, chisq;
	ContingencyTable_getEntropies (this, & h, & hx, & hy, & hygx, & hxgy, & uygx, & uxgy, & uxy);
	ContingencyTable_chisq (this, & chisq, & ndf);

	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
	MelderInfo_writeLine (U"Entropies (y is row variable):");
	MelderInfo_writeLine (U"  Total: ", h);
	MelderInfo_writeLine (U"  Y: ", hy);
	MelderInfo_writeLine (U"  X: ", hx);
	MelderInfo_writeLine (U"  Y given x: ", hygx);
	MelderInfo_writeLine (U"  X given y: ", hxgy);
	MelderInfo_writeLine (U"  Dependency of y on x: ", uygx);
	MelderInfo_writeLine (U"  Dependency of x on y: ", uxgy);
	MelderInfo_writeLine (U"  Symmetrical dependency: ", uxy);
	MelderInfo_writeLine (U"  Chi squared: ", chisq);
	MelderInfo_writeLine (U"  Degrees of freedom: ", ndf);
	MelderInfo_writeLine (U"  Probability: ", ContingencyTable_chisqProbability (this));
}

autoContingencyTable ContingencyTable_create (integer numberOfRows, integer numberOfColumns) {
	try {
		autoContingencyTable me = Thing_new (ContingencyTable);
		TableOfReal_init (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ContingencyTable not created.");
	}
}

double ContingencyTable_chisqProbability (ContingencyTable me) {
	double chisq, df;
	ContingencyTable_chisq (me, & chisq, & df);
	if (chisq == 0.0 && df == 0.0)
		return 0.0;
	return NUMchiSquareQ (chisq, df);
}

double ContingencyTable_cramersStatistic (ContingencyTable me) {
	if (my numberOfRows == 1 || my numberOfColumns == 1)
		return 0.0;

	const double sum = NUMsum (my data.all());

	integer nmin = std::min (my numberOfColumns, my numberOfRows);

	nmin --;
	
	double chisq, df;
	ContingencyTable_chisq (me, & chisq, & df);
	if (chisq == 0.0 && df == 0.0)
		return 0.0;
	return sqrt (chisq / (sum * nmin));
}

double ContingencyTable_contingencyCoefficient (ContingencyTable me) {
	const double sum = NUMsum (my data.all());
	double chisq, df;
	ContingencyTable_chisq (me, & chisq, & df);
	if (chisq == 0.0 && df == 0.0)
		return 0.0;

	return sqrt (chisq / (chisq + (double) sum));
}

void ContingencyTable_chisq (ContingencyTable me, double *out_chisq, double *out_df) {
	
	autoVEC rowSums = rowSums_VEC (my data.get());
	autoVEC columnSums = columnSums_VEC (my data.get());
	const double totalSum = NUMsum (my data.all());
	
	integer nrow = my numberOfRows, ncol = my numberOfColumns;
	
	for (integer irow = 1; irow <= my numberOfRows; irow ++)
		if (rowSums [irow] == 0.0)
			nrow --;

	if (nrow == 0) {
		if (out_chisq)
			*out_chisq = undefined;
		if (out_df)
			*out_df = undefined;
		return;
	}
	
	for (integer icol = 1; icol <= my numberOfColumns; icol ++)
		if (columnSums [icol] == 0.0)
			ncol --;
	
	if (out_df)
		*out_df = (nrow - 1.0) * (ncol - 1.0);
	if (out_chisq) {
		longdouble chisq = 0.0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			if (rowSums [irow] > 0.0) {
				for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
					if (columnSums [icol] > 0.0) {
						const longdouble expected = rowSums [irow] * columnSums [icol] / totalSum;
						const longdouble difference = my data [irow] [icol] - expected;
						chisq += difference * difference / expected;
					}
				}
			}
		}
		*out_chisq = (double) chisq;
	}
}

void ContingencyTable_getEntropies (ContingencyTable me, double *out_h, double *out_hx, double *out_hy, double *out_hygx, double *out_hxgy, double *out_uygx, double *out_uxgy, double *out_uxy) {	
	NUMgetEntropies (my data.get(), out_h, out_hx,
			out_hy,	out_hygx, out_hxgy, out_uygx, out_uxgy, out_uxy);	
}

autoContingencyTable Confusion_to_ContingencyTable (Confusion me) {
	try {
		autoContingencyTable thee = Thing_new (ContingencyTable);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ContingencyTable.");
	}
}

autoContingencyTable TableOfReal_to_ContingencyTable (TableOfReal me) {
	try {
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoContingencyTable thee = Thing_new (ContingencyTable);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ContingencyTable.");
	}
}

// End of file ContingencyTable.cpp
