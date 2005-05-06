/* Table.c
 *
 * Copyright (C) 2002-2005 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2003/05/19 Melder_atof
 * pb 2004/07/25 Melder_double
 * pb 2004/10/16 struct tags
 * pb 2005/03/04 Melder_NUMBER
 * pb 2005/04/25 Table_getLogisticRegression
 * pb 2005/04/25 Table_to_Matrix
 */

#include <ctype.h>
#include "Table.h"
#include "NUM2.h"
#include "Matrix.h"
#include "Formula.h"

#include "oo_DESTROY.h"
#include "Table_def.h"
#include "oo_COPY.h"
#include "Table_def.h"
#include "oo_EQUAL.h"
#include "Table_def.h"
#include "oo_WRITE_ASCII.h"
#include "Table_def.h"
#include "oo_WRITE_BINARY.h"
#include "Table_def.h"
#include "oo_READ_ASCII.h"
#include "Table_def.h"
#include "oo_READ_BINARY.h"
#include "Table_def.h"
#include "oo_DESCRIPTION.h"
#include "Table_def.h"

class_methods (TableRow, Data) {
	class_method_local (TableRow, destroy)
	class_method_local (TableRow, description)
	class_method_local (TableRow, copy)
	class_method_local (TableRow, equal)
	class_method_local (TableRow, writeAscii)
	class_method_local (TableRow, writeBinary)
	class_method_local (TableRow, readAscii)
	class_method_local (TableRow, readBinary)
class_methods_end }

static void info (I) {
	iam (Table);
	Melder_information ("Number of rows = %ld\nNumber of columns = %ld",
		my rows -> size, my numberOfColumns);
}

static double getNrow (I) { iam (Table); return my rows -> size; }
static double getNcol (I) { iam (Table); return my numberOfColumns; }
static double getMatrix (I, long irow, long icol) {
	iam (Table);
	char *stringValue;
	if (irow < 1 || irow > my rows -> size) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	stringValue = ((TableRow) my rows -> item [irow]) -> cells [icol]. string;
	return stringValue == NULL ? NUMundefined : Melder_atof (stringValue);
}
static double getColumnIndex (I, const char *columnLabel) {
	iam (Table);
	return Table_columnLabelToIndex (me, columnLabel);
}

class_methods (Table, Data) {
	class_method_local (Table, destroy)
	class_method_local (Table, description)
	class_method_local (Table, copy)
	class_method_local (Table, equal)
	class_method_local (Table, writeAscii)
	class_method_local (Table, writeBinary)
	class_method_local (Table, readAscii)
	class_method_local (Table, readBinary)
	class_method (info)
	class_method (getNrow)
	class_method (getNcol)
	class_method (getMatrix)
	class_method (getColumnIndex)
class_methods_end }

static TableRow TableRow_create (long numberOfColumns) {
	TableRow me = new (TableRow); cherror
	my numberOfColumns = numberOfColumns;
	my cells = NUMstructvector (TableCell, 1, numberOfColumns); cherror
end:
	iferror forget (me);
	return me;
}

int Table_init (I, long numberOfRows, long numberOfColumns) {
	iam (Table);
	long irow;
	if (numberOfRows < 0 || numberOfColumns < 1)
		return Melder_error ("(Table_init:) Cannot create cell-less table.");
	my numberOfColumns = numberOfColumns;
	if (! (my columnHeaders = NUMstructvector (TableColumnHeader, 1, numberOfColumns))) return 0;
	if (! (my rows = Ordered_create ())) return 0;
	for (irow = 1; irow <= numberOfRows; irow ++) {
		Table_appendRow (me); iferror return 0;
	}
	return 1;
}

Table Table_create (long numberOfRows, long numberOfColumns) {
	Table me = new (Table);
	if (! me || ! Table_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

int Table_appendRow (Table me) {
	TableRow row = TableRow_create (my numberOfColumns); cherror
	Collection_addItem (my rows, row); cherror
end:
	iferror return 0;
	return 1;
}

int Table_appendColumn (Table me, const char *label) {
	return Table_insertColumn (me, my numberOfColumns + 1, label);
}

int Table_removeRow (Table me, long irow) {
	if (my rows -> size == 1)
		{ Melder_error ("Cannot remove the only row."); goto end; }
	if (irow < 1 || irow > my rows -> size)
		{ Melder_error ("No row %ld.", irow); goto end; }
	Collection_removeItem (my rows, irow);
end:
	iferror return Melder_error ("(Table_removeRow:) Not performed.");
	return 1;
}

int Table_removeColumn (Table me, long icol) {
	long irow, jcol;
	if (my numberOfColumns == 1)
		{ Melder_error ("Cannot remove the only column."); goto end; }
	if (icol < 1 || icol > my numberOfColumns)
		{ Melder_error ("No column %ld.", icol); goto end; }
	Melder_free (my columnHeaders [icol]. label);
	for (jcol = icol; jcol < my numberOfColumns; jcol ++)
		my columnHeaders [jcol] = my columnHeaders [jcol + 1];
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Melder_free (row -> cells [icol]. string);
		for (jcol = icol; jcol < row -> numberOfColumns; jcol ++)
			row -> cells [jcol] = row -> cells [jcol + 1];
		row -> numberOfColumns --;
	}
	my numberOfColumns --;
end:
	iferror return Melder_error ("(Table_removeColumn:) Not performed.");
	return 1;
}

int Table_insertRow (Table me, long irow) {
	TableRow row = TableRow_create (my numberOfColumns); cherror
	if (irow < 1 || irow > my rows -> size + 1)
		{ Melder_error ("Cannot create row %ld.", irow); goto end; }
	Ordered_addItemPos (my rows, row, irow);
end:
	iferror return Melder_error ("(Table_insertRow:) Not performed.");
	return 1;
}

int Table_insertColumn (Table me, long icol, const char *label) {
	struct structTableColumnHeader *columnHeaders = NULL;
	long irow, jcol;
	if (icol < 1 || icol > my numberOfColumns + 1)
		{ Melder_error ("Cannot create column %ld.", icol); goto end; }
	columnHeaders = NUMstructvector (TableColumnHeader, 1, my numberOfColumns + 1); cherror
	for (jcol = 1; jcol < icol; jcol ++)
		columnHeaders [jcol] = my columnHeaders [jcol];   /* Shallow copy of strings. */
	for (jcol = my numberOfColumns + 1; jcol > icol; jcol --)
		columnHeaders [jcol] = my columnHeaders [jcol - 1];   /* Shallow copy of strings. */
	columnHeaders [icol]. label = Melder_strdup (label); cherror
	NUMstructvector_free (TableColumnHeader, my columnHeaders, 1);
	my columnHeaders = columnHeaders;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		struct structTableCell *cells = NUMstructvector (TableCell, 1, row -> numberOfColumns + 1); cherror
		for (jcol = 1; jcol < icol; jcol ++)
			cells [jcol] = row -> cells [jcol];   /* Shallow copy of strings. */
		for (jcol = row -> numberOfColumns + 1; jcol > icol; jcol --)
			cells [jcol] = row -> cells [jcol - 1];   /* Shallow copy of strings. */
		NUMstructvector_free (TableCell, row -> cells, 1);
		row -> cells = cells;
		row -> numberOfColumns ++;
	}
	my numberOfColumns ++;
end:
	iferror return Melder_error ("(Table_insertColumn:) Not performed.");
	return 1;
}

void Table_setColumnLabel (Table me, long icol, const char *label) {
	if (icol < 1 || icol > my numberOfColumns || label == my columnHeaders [icol]. label) return;
	Melder_free (my columnHeaders [icol]. label);
	my columnHeaders [icol]. label = Melder_strdup (label);
}

long Table_columnLabelToIndex (Table me, const char *label) {
	long icol;
	for (icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnHeaders [icol]. label && strequ (my columnHeaders [icol]. label, label))
			return icol;
	return 0;
}

long Table_searchColumn (Table me, long icol, const char *value) {
	long irow;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (row -> cells [icol]. string != NULL && strequ (row -> cells [icol]. string, value))
			return irow;
	}
	return 0;
}

int Table_setStringValue (Table me, long irow, long icol, const char *value) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return Melder_error ("Row number %ld out of range.", irow);
	if (icol < 1 || icol > my numberOfColumns) return Melder_error ("Column number %ld out of range.", icol);
	row = my rows -> item [irow];
	Melder_free (row -> cells [icol]. string);
	row -> cells [icol]. string = Melder_strdup (value);
	my columnHeaders [icol]. numericized = FALSE;
	return 1;
}

int Table_setNumericValue (Table me, long irow, long icol, double value) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return Melder_error ("Row number %ld out of range.", irow);
	if (icol < 1 || icol > my numberOfColumns) return Melder_error ("Column number %ld out of range.", icol);
	row = my rows -> item [irow];
	Melder_free (row -> cells [icol]. string);
	row -> cells [icol]. string = Melder_strdup (Melder_double (value));
	my columnHeaders [icol]. numericized = FALSE;
	return 1;
}

static int Table_numericize (Table me, long icol) {
	long irow;
	if (my columnHeaders [icol]. numericized) return 1;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		char *string = row -> cells [icol]. string;
		if (string == NULL) return 0;
		row -> cells [icol]. number = Melder_atof (string);
	}
	my columnHeaders [icol]. numericized = TRUE;
	return 1;
}

const char * Table_getStringValue (Table me, long irow, long icol) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return NULL;
	if (icol < 1 || icol > my numberOfColumns) return NULL;
	row = my rows -> item [irow];
	return row -> cells [icol]. string;
}

double Table_getNumericValue (Table me, long irow, long icol) {
	TableRow row;
	if (irow < 1 || irow > my rows -> size) return NUMundefined;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	row = my rows -> item [irow];
	Table_numericize (me, icol); cherror
end:
	iferror return NUMundefined;   /* BUG */
	return row -> cells [icol]. number;
}

double Table_getMean (Table me, long icol) {
	double sum = 0.0;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 1) return NUMundefined;
	if (! Table_numericize (me, icol)) return NUMundefined;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		sum += row -> cells [icol]. number;
	}
	return sum / my rows -> size;
}

double Table_getStdev (Table me, long icol) {
	double mean = Table_getMean (me, icol), sum = 0.0;
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return NUMundefined;
	if (my rows -> size < 2) return NUMundefined;
	if (mean == NUMundefined) return NUMundefined;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		double d = row -> cells [icol]. number - mean;
		sum += d * d;
	}
	return sqrt (sum / (my rows -> size - 1));
}

Table Table_selectRowsWhereColumn (Table me, long column, enum Melder_NUMBER which, double criterion) {
	Table thee = NULL;
	long irow, icol;
	if (column < 1 || column > my numberOfColumns)
		{ Melder_error ("No column %ld.", column); goto end; }
	Table_numericize (me, column);
	thee = Table_create (0, my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		thy columnHeaders [icol]. label = Melder_strdup (my columnHeaders [icol]. label); cherror
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		if (Melder_numberMatchesCriterion (row -> cells [column]. number, which, criterion)) {
			TableRow newRow = Data_copy (row);
			Collection_addItem (thy rows, newRow);
		}
	}
	if (thy rows -> size == 0) {
		Melder_error ("No row matches criterion.");
		goto end;
	}
end:
	iferror {
		forget (thee);
		Melder_error ("(Table_selectRowsWhereColumn:) Not performed.");
	}
	return thee;
}

static long cellCompare_column;

static int cellCompare (const void *first, const void *second) {
	TableRow me = * (TableRow *) first, thee = * (TableRow *) second;
	if (my cells [cellCompare_column]. number < thy cells [cellCompare_column]. number) return -1;
	if (my cells [cellCompare_column]. number > thy cells [cellCompare_column]. number) return +1;
	return 0;
}

void Table_sortRows (Table me, long column1, long column2) {
	long imin, imax;
	if (column1 >= 1 && column1 <= my numberOfColumns) {
		Table_numericize (me, column1);
		cellCompare_column = column1;
		qsort (& my rows -> item [1], (unsigned long) my rows -> size, sizeof (TableRow), cellCompare);
	}
	if (column2 >= 1 && column2 <= my numberOfColumns) {
		Table_numericize (me, column2);
		cellCompare_column = column2;
		imin = 1;
		while (imin <= my rows -> size) {
			double value = ((TableRow) my rows -> item [imin]) -> cells [column1]. number;
			for (imax = imin + 1; imax <= my rows -> size; imax ++) {
				if (((TableRow) my rows -> item [imax]) -> cells [column1]. number != value)
					break;
			}
			qsort (& my rows -> item [imin], (unsigned long) imax - imin, sizeof (TableRow), cellCompare);
			imin = imax;
		}
	}
}

int Table_appendSumColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number + row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendDifferenceColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number - row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendProductColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns,
			row -> cells [column1]. number * row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_appendQuotientColumn (Table me, long column1, long column2, const char *label) {
	long irow;
	if (column1 < 1 || column1 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column1);
	if (column2 < 1 || column2 > my numberOfColumns) return Melder_error ("Column number %ld out of range.", column2);
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	Table_appendColumn (me, label); cherror
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		Table_setNumericValue (me, irow, my numberOfColumns, row -> cells [column2]. number == 0.0 ? NUMundefined :
			row -> cells [column1]. number / row -> cells [column2]. number);
	}
end:
	iferror return 0;
	return 1;
}

int Table_formula (Table me, long icol, const char *expression) {
	long irow;
	if (icol < 1 || icol > my numberOfColumns) return Melder_error ("No column %ld.", icol);
	if (! Formula_compile (NULL, me, expression, FALSE, TRUE)) goto end;
	for (irow = 1; irow <= my rows -> size; irow ++) {
		double result;
		if (! Formula_run (irow, icol, & result, NULL)) goto end;
		Table_setNumericValue (me, irow, icol, result);
	}
end:
	iferror return 0;
	return 1;
}

double Table_getCorrelation_pearsonR (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	long n = my rows -> size, irow;
	double correlation;
	double sum1 = 0.0, sum2 = 0.0, sum12 = 0.0, sum11 = 0.0, sum22 = 0.0, mean1, mean2;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	if (n < 2) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		sum1 += row -> cells [column1]. number;
		sum2 += row -> cells [column2]. number;
	}
	mean1 = sum1 / n;
	mean2 = sum2 / n;
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		double d1 = row -> cells [column1]. number - mean1, d2 = row -> cells [column2]. number - mean2;
		sum12 += d1 * d2;
		sum11 += d1 * d1;
		sum22 += d2 * d2;
	}
	correlation = sum11 == 0.0 || sum22 == 0.0 ? NUMundefined : sum12 / sqrt (sum11 * sum22);
	if (out_significance && NUMdefined (correlation) && n >= 3)
		*out_significance = fabs (correlation) == 1.0 ? 0.0 :
			/* One-sided: */
			NUMstudentQ (fabs (correlation) * sqrt ((n - 2) / (1 - correlation * correlation)), n - 2);
	if ((out_lowerLimit || out_upperLimit) && NUMdefined (correlation) && n >= 4) {
		if (fabs (correlation) == 1.0) {
			if (out_lowerLimit) *out_lowerLimit = correlation;
			if (out_upperLimit) *out_upperLimit = correlation;
		} else {
			double z = 0.5 * log ((1.0 + correlation) / (1.0 - correlation));
			double dz = NUMinvGaussQ (significanceLevel) / sqrt (n - 3);
			if (out_lowerLimit)
				*out_lowerLimit = tanh (z - dz);
			if (out_upperLimit)
				*out_upperLimit = tanh (z + dz);
		}
	}
	return correlation;
}

double Table_getCorrelation_kendallTau (Table me, long column1, long column2, double significanceLevel,
	double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	long n = my rows -> size, irow, jrow;
	double correlation, denominator;
	long numberOfConcordants = 0, numberOfDiscordants = 0, numberOfExtra1 = 0, numberOfExtra2 = 0;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	for (irow = 1; irow < n; irow ++) {
		TableRow rowi = my rows -> item [irow];
		for (jrow = irow + 1; jrow <= n; jrow ++) {
			TableRow rowj = my rows -> item [jrow];
			double diff1 = rowi -> cells [column1]. number - rowj -> cells [column1]. number;
			double diff2 = rowi -> cells [column2]. number - rowj -> cells [column2]. number;
			double concord = diff1 * diff2;
			if (concord > 0.0) {
				numberOfConcordants ++;
			} else if (concord < 0.0) {
				numberOfDiscordants ++;
			} else if (diff1 != 0.0) {
				numberOfExtra1 ++;
			} else {
				numberOfExtra2 ++;
			}
		}
	}
	denominator = sqrt ((numberOfConcordants + numberOfDiscordants + numberOfExtra1) *
		(numberOfConcordants + numberOfDiscordants + numberOfExtra2));
	correlation = denominator == 0.0 ? NUMundefined : (numberOfConcordants - numberOfDiscordants) / denominator;
	if ((out_significance || out_lowerLimit || out_upperLimit) && NUMdefined (correlation) && n >= 2) {
		double standardError = sqrt ((4 * n + 10.0) / (9 * n * (n - 1)));
		if (out_significance)
			*out_significance = NUMgaussQ (fabs (correlation) / standardError);   /* One-sided. */
		if (out_lowerLimit)
			*out_lowerLimit = correlation - NUMinvGaussQ (significanceLevel) * standardError;
		if (out_upperLimit)
			*out_upperLimit = correlation + NUMinvGaussQ (significanceLevel) * standardError;
	}
	return correlation;
}

double Table_getDifference_studentT (Table me, long column1, long column2, double significanceLevel,
	double *out_t, double *out_significance, double *out_lowerLimit, double *out_upperLimit)
{
	double mean1 = 0.0, mean2 = 0.0, var1 = 0.0, var2 = 0.0, covar = 0.0, standardError;
	double difference;
	long n = my rows -> size, irow;
	if (out_significance) *out_significance = NUMundefined;
	if (out_lowerLimit) *out_lowerLimit = NUMundefined;
	if (out_upperLimit) *out_upperLimit = NUMundefined;
	if (n < 2) return NUMundefined;
	if (column1 < 1 || column1 > my numberOfColumns) return NUMundefined;
	if (column2 < 1 || column2 > my numberOfColumns) return NUMundefined;
	Table_numericize (me, column1);
	Table_numericize (me, column2);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		mean1 += row -> cells [column1]. number;
		mean2 += row -> cells [column2]. number;
	}
	mean1 /= n;
	mean2 /= n;
	difference = mean1 - mean2;
	if ((out_t || out_significance || out_lowerLimit || out_upperLimit)) {
		for (irow = 1; irow <= n; irow ++) {
			TableRow row = my rows -> item [irow];
			double diff1 = row -> cells [column1]. number - mean1, diff2 = row -> cells [column2]. number - mean2;
			var1 += diff1 * diff1;
			var2 += diff2 * diff2;
			covar += diff1 * diff2;
		}
		standardError = sqrt ((var1 + var2 - 2.0 * covar) / (n - 1) / n);
		if (out_t && standardError != 0.0 ) *out_t = difference / standardError;
		if (out_significance) *out_significance = standardError == 0.0 ? 0.0 : NUMstudentQ (difference / standardError, n - 1);
		if (out_lowerLimit) *out_lowerLimit = difference - standardError * NUMinvStudentQ (significanceLevel, n - 1);
		if (out_upperLimit) *out_upperLimit = difference + standardError * NUMinvStudentQ (significanceLevel, n - 1);
	}
	return difference;
}

Matrix Table_to_Matrix (Table me) {
	long irow, icol;
	Matrix thee = Matrix_createSimple (my rows -> size, my numberOfColumns); cherror
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		Table_numericize (me, icol);
	}
	for (irow = 1; irow <= my rows -> size; irow ++) {
		TableRow row = my rows -> item [irow];
		for (icol = 1; icol <= my numberOfColumns; icol ++) {
			thy z [irow] [icol] = (float) row -> cells [icol]. number;
		}
	}
end:
	iferror return NULL;
	return thee;
}

double Table_getFisherF (Table me, long col1, long col2);
double Table_getOneWayAnovaSignificance (Table me, long col1, long col2);
double Table_getFisherFLowerLimit (Table me, long col1, long col2, double significanceLevel);
double Table_getFisherFUpperLimit (Table me, long col1, long col2, double significanceLevel);

static int Table_getExtrema (Table me, long icol, double *minimum, double *maximum) {
	long n = my rows -> size, irow;
	if (icol < 1 || icol > my numberOfColumns || n == 0) {
		*minimum = *maximum = NUMundefined;
		return 0;
	}
	Table_numericize (me, icol);
	*minimum = *maximum = ((TableRow) my rows -> item [1]) -> cells [icol]. number;
	for (irow = 2; irow <= n; irow ++) {
		double value = ((TableRow) my rows -> item [irow]) -> cells [icol]. number;
		if (value < *minimum) *minimum = value;
		if (value > *maximum) *maximum = value;
	}
	return 1;
}

void Table_scatterPlot_mark (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, double markSize_mm, const char *mark, int garnish)
{
	long n = my rows -> size, irow;
	if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns) return;
	Table_numericize (me, xcolumn);
	Table_numericize (me, ycolumn);
	if (xmin == xmax) {
		if (! Table_getExtrema (me, xcolumn, & xmin, & xmax)) return;
		if (xmin == xmax) xmin -= 0.5, xmax += 0.5;
	}
	if (ymin == ymax) {
		if (! Table_getExtrema (me, ycolumn, & ymin, & ymax)) return;
		if (ymin == ymax) ymin -= 0.5, ymax += 0.5;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (irow = 1; irow <= n; irow ++) {
		TableRow row = my rows -> item [irow];
		Graphics_mark (g, row -> cells [xcolumn]. number, row -> cells [ycolumn]. number, markSize_mm, mark);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		if (my columnHeaders [xcolumn]. label)
			Graphics_textBottom (g, TRUE, my columnHeaders [xcolumn]. label);
		Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
		if (my columnHeaders [ycolumn]. label)
			Graphics_textLeft (g, TRUE, my columnHeaders [ycolumn]. label);
	}
}

#if 0
static void NUMrationalize (double x, long *numerator, long *denominator) {
	double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		double numerator_d = x * *denominator, rounded = floor (numerator_d + 0.5);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = rounded;
			return;
		}
	}
	*denominator = 0;   /* Failure. */
}

static void print4 (char *buffer, double value, int iformat, int width, int precision) {
	char formatString [40];
	if (value == NUMundefined) strcpy (buffer, "undefined");
	else if (iformat == 4) {
		long numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			sprintf (buffer, "0");
		else if (denominator > 1)
			sprintf (buffer, "%ld/%ld", numerator, denominator);
		else
			sprintf (buffer, "%.7g", value);
	} else {
		sprintf (formatString, "%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		sprintf (buffer, formatString, value);
	}
}

Table Matrix_to_Table (I) {
	iam (Matrix);
	long i, j;
	Table thee = Table_create (my ny, my nx); cherror
	for (i = 1; i <= my ny; i ++) for (j = 1; j <= my nx; j ++)
		thy data [i] [j] = my z [i] [j];
end:
	iferror return NULL;
	return thee;
}

static void fixRows (Table me, long *rowmin, long *rowmax) {
	if (*rowmax < *rowmin) { *rowmin = 1; *rowmax = my numberOfRows; }
	else if (*rowmin < 1) *rowmin = 1;
	else if (*rowmax > my numberOfRows) *rowmax = my numberOfRows;
}
static void fixColumns (Table me, long *colmin, long *colmax) {
	if (*colmax < *colmin) { *colmin = 1; *colmax = my numberOfColumns; }
	else if (*colmin < 1) *colmin = 1;
	else if (*colmax > my numberOfColumns) *colmax = my numberOfColumns;
}
static double getMaxRowLabelWidth (Table me, Graphics g, long rowmin, long rowmax) {
	double maxWidth = 0.0;
	long row;
	if (! my rowLabels) return 0.0;
	fixRows (me, & rowmin, & rowmax);
	for (row = rowmin; row <= rowmax; row ++) if (my rowLabels [row] && my rowLabels [row] [0]) {
		double textWidth = Graphics_textWidth_ps (g, my rowLabels [row]);
		if (textWidth > maxWidth) maxWidth = textWidth;
	}
	return maxWidth;
}
static double getLeftMargin (Graphics g) {
	return Graphics_dxMMtoWC (g, 1);
}
static double getLineSpacing (Graphics g) {
	return Graphics_dyMMtoWC (g, 1.5 * Graphics_inqFontSize (g) * 25.4 / 72);
}
static double getMaxColumnLabelHeight (Table me, Graphics g, long colmin, long colmax) {
	double maxHeight = 0.0, lineSpacing = getLineSpacing (g);
	long col;
	if (! my columnLabels) return 0.0;
	fixRows (me, & colmin, & colmax);
	for (col = colmin; col <= colmax; col ++) if (my columnLabels [col] && my columnLabels [col] [0]) {
		if (! maxHeight) maxHeight = lineSpacing;
	}
	return maxHeight;
}

void Table_drawAsNumbers (I, Graphics g, long rowmin, long rowmax, int iformat, int precision) {
	iam (Table);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long row, col;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfColumns + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	for (col = 1; col <= my numberOfColumns; col ++) {
		if (my columnLabels && my columnLabels [col] && my columnLabels [col] [0])
			Graphics_text (g, col, 1, my columnLabels [col]);
	}
	for (row = rowmin; row <= rowmax; row ++) {
		double y = 1 - lineSpacing * (row - rowmin + 0.6);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [row] && my rowLabels [row] [0])
			Graphics_text (g, 0.5 - leftMargin, y, my rowLabels [row]);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (col = 1; col <= my numberOfColumns; col ++) {
			char text [40];
			print4 (text, my data [row] [col], iformat, 0, precision);
			Graphics_text (g, col, y, text);
		}
	}
	if (maxTextHeight) {
		double left = 0.5;
		if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
		Graphics_line (g, left, 1, my numberOfColumns + 0.5, 1);
	}
	Graphics_unsetInner (g);
}

void Table_drawAsNumbers_if (I, Graphics g, long rowmin, long rowmax, int iformat, int precision,
	const char *conditionFormula)
{
	iam (Table);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long row, col;
	Matrix original = Table_to_Matrix (me), conditions = Data_copy (original);
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfColumns + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);
	if (! Matrix_formula (original, conditionFormula, 0, 0, 0, conditions))
		{ forget (original); forget (conditions); Melder_flushError ("Numbers not drawn."); return; }

	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	for (col = 1; col <= my numberOfColumns; col ++) {
		if (my columnLabels && my columnLabels [col] && my columnLabels [col] [0])
			Graphics_text (g, col, 1, my columnLabels [col]);
	}
	for (row = rowmin; row <= rowmax; row ++) {
		double y = 1 - lineSpacing * (row - rowmin + 0.6);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [row] && my rowLabels [row] [0])
			Graphics_text (g, 0.5 - leftMargin, y, my rowLabels [row]);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (col = 1; col <= my numberOfColumns; col ++) if (conditions -> z [row] [col] != 0.0) {
			char text [40];
			print4 (text, my data [row] [col], iformat, 0, precision);
			Graphics_text (g, col, y, text);
		}
	}
	if (maxTextHeight) {
		double left = 0.5;
		if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * leftMargin;
		Graphics_line (g, left, 1, my numberOfColumns + 0.5, 1);
	}
	Graphics_unsetInner (g);
	forget (original);
	forget (conditions);
}

void Table_drawVerticalLines (I, Graphics g, long rowmin, long rowmax) {
	iam (Table);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight;
	long colmin = 1, colmax = my numberOfColumns, col;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);   /* Not earlier! */
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	if (maxTextWidth > 0.0) colmin -= 1;
	for (col = colmin + 1; col <= colmax; col ++)
		Graphics_line (g, col - 0.5, 1 + maxTextHeight, col - 0.5, 1 - lineSpacing * (rowmax - rowmin + 1));
	Graphics_unsetInner (g);
}

void Table_drawLeftAndRightLines (I, Graphics g, long rowmin, long rowmax) {
	iam (Table);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight, left, right, top, bottom;
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	right = colmax + 0.5;
	top = 1 + maxTextHeight;
	bottom = 1 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (g, left, top, left, bottom);
	Graphics_line (g, right, top, right, bottom);
	Graphics_unsetInner (g);
}

void Table_drawHorizontalLines (I, Graphics g, long rowmin, long rowmax) {
	iam (Table);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight, left, top, right;
	long colmin = 1, colmax = my numberOfColumns, row;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	left = 0.5;
	top = rowmin;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	if (maxTextHeight > 0.0) rowmin -= 1;
	right = colmax + 0.5;
	for (row = rowmin; row < rowmax; row ++) {
		double y = 1 - lineSpacing * (row - top + 1);
		Graphics_line (g, left, y, right, y);
	}
	Graphics_unsetInner (g);
}

void Table_drawTopAndBottomLines (I, Graphics g, long rowmin, long rowmax) {
	iam (Table);
	double leftMargin, lineSpacing, maxTextWidth, maxTextHeight, left, top, right, bottom;
	long colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, 0, 1);
	leftMargin = getLeftMargin (g), lineSpacing = getLineSpacing (g);
	maxTextWidth = getMaxRowLabelWidth (me, g, rowmin, rowmax);
	maxTextHeight = getMaxColumnLabelHeight (me, g, 1, my numberOfColumns);

	left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2 * lineSpacing;
	right = colmax + 0.5;
	top = 1 + maxTextHeight;
	bottom = 1 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (g, left, top, right, top);
	Graphics_line (g, left, bottom, right, bottom);
	Graphics_unsetInner (g);
}

void Table_drawAsSquares (I, Graphics g, long rowmin, long rowmax,
	long colmin, long colmax, int garnish)
{
	iam (Table);
	double datamax, dx = 1, dy = 1;
	long i, j;
	int colour = Graphics_inqColour (g);
	fixRows (me, & rowmin, & rowmax);
	fixColumns (me, & colmin, & colmax);
	
	Graphics_setInner (g);
	Graphics_setWindow (g, colmin - 0.5, colmax + 0.5, rowmin - 0.5, rowmax + 0.5);
	datamax = my data [rowmin] [colmin];
	for (i = 1; i <= my numberOfRows; i ++) for (j = 1; j <= my numberOfColumns; j ++)
		if (fabs (my data [i] [j]) > datamax) datamax = fabs (my data [i] [j]);
	
	for (i = rowmin; i <= rowmax; i ++) {
		double y = rowmax + rowmin - i;
		for (j = colmin; j <= colmax; j ++) {
			double x = j;
			/* two neighbouring squares should not touch -> 0.95 */
			double d = 0.95 * sqrt (fabs (my data[i][j]) / datamax);
			double x1WC = x - d * dx / 2, x2WC = x + d * dx / 2;
			double y1WC = y - d * dy / 2, y2WC = y + d * dy / 2;
			Graphics_setGrey (g, my data[i][j] > 0.0 ? 1.0 : 0.0);
			Graphics_fillRectangle (g, x1WC, x2WC, y1WC, y2WC);
			Graphics_setColour (g, colour);
			Graphics_rectangle (g, x1WC, x2WC , y1WC, y2WC);
		}
	}
	Graphics_setGrey (g, 0.0);
	Graphics_unsetInner (g);
	if (garnish) {
		for (i = rowmin; i <= rowmax; i ++) if (my rowLabels [i]) 
			Graphics_markLeft (g, rowmax + rowmin - i, 0, 0, 0, my rowLabels [i]);
		for (j = colmin; j <= colmax; j ++) if (my columnLabels [j])
			Graphics_markTop (g, j, 0, 0, 0, my columnLabels [j]);
	}
}

Any TablesOfReal_append (I, thou) {
	iam (Table); thouart (Table);
	Table him = NULL;
	long irow, icol;
	if (thy numberOfColumns != my numberOfColumns)
		{ Melder_error ("Numbers of columns do not match."); goto end; }
	him = Thing_new (my methods); cherror
	Table_init (him, my numberOfRows + thy numberOfRows, my numberOfColumns); cherror
	/* Unsafe: new attributes not initialized. */
	for (icol = 1; icol <= my numberOfColumns; icol ++) {
		Table_setColumnLabel (him, icol, my columnLabels [icol]); cherror
	}
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		Table_setRowLabel (him, irow, my rowLabels [irow]); cherror
		for (icol = 1; icol <= my numberOfColumns; icol ++)
			his data [irow] [icol] = my data [irow] [icol];
	}
	for (irow = 1; irow <= thy numberOfRows; irow ++) {
		long hisRow = irow + my numberOfRows;
		Table_setRowLabel (him, hisRow, thy rowLabels [irow]); cherror
		for (icol = 1; icol <= my numberOfColumns; icol ++)
			his data [hisRow] [icol] = thy data [irow] [icol];
	}
end:
	iferror { forget (him); Melder_error ("(TablesOfReal_append:) Not performed."); }
	return him;
}

Any TablesOfReal_appendMany (Collection me) {
	Table him = NULL, thee;
	long itab, irow, icol, nrow, ncol;
	if (my size == 0) return Melder_errorp ("Cannot add zero tables.");
	thee = my item [1];
	nrow = thy numberOfRows;
	ncol = thy numberOfColumns;
	for (itab = 2; itab <= my size; itab ++) {
		thee = my item [itab];
		nrow += thy numberOfRows;
		if (thy numberOfColumns != ncol)
			{ Melder_error ("Numbers of columns do not match."); goto end; }
	}
	him = Thing_new (thy methods); cherror
	Table_init (him, nrow, ncol); cherror
	/* Unsafe: new attributes not initialized. */
	for (icol = 1; icol <= ncol; icol ++) {
		Table_setColumnLabel (him, icol, thy columnLabels [icol]); cherror
	}
	nrow = 0;
	for (itab = 1; itab <= my size; itab ++) {
		thee = my item [itab];
		for (irow = 1; irow <= thy numberOfRows; irow ++) {
			nrow ++;
			Table_setRowLabel (him, nrow, thy rowLabels [irow]); cherror
			for (icol = 1; icol <= ncol; icol ++)
				his data [nrow] [icol] = thy data [irow] [icol];
		}
	}
	Melder_assert (nrow == his numberOfRows);
end:
	iferror { forget (him); Melder_error ("(TablesOfReal_appendMany:) Not performed."); }
	return him;
}
#endif

int Table_writeToTableFile (Table me, MelderFile file) {
	FILE *f = Melder_fopen (file, "w");
	long i, j;
	if (! f) return 0;
	for (i = 1; i <= my numberOfColumns; i ++) {
		char *s = my columnHeaders [i]. label;
		fprintf (f, i == 1 ? "%s" : "\t%s", s != NULL && s [0] != '\0' ? s : "?");
	}
	fprintf (f, "\n");
	for (i = 1; i <= my rows -> size; i ++) {
		TableRow row = my rows -> item [i];
		for (j = 1; j <= my numberOfColumns; j ++) {
			char *s = row -> cells [j]. string;
			fprintf (f, j == 1 ? "%s" : "\t%s", s != NULL && s [0] != '\0' ? s : "?");
		}
		fprintf (f, "\n");
	}
	if (! Melder_fclose (file, f)) return 0;
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

Table Table_readFromTableFile (MelderFile file) {
	Table me = NULL;
	long nrow, ncol, nelements, irow, icol;
	FILE *f = Melder_fopen (file, "rb"); cherror

	/*
	 * Count number of columns.
	 */
	ncol = 0;
	for (;;) {
		int kar = fgetc (f);
		if (kar == '\n' || kar == '\r' || kar == EOF) break;
		if (kar == ' ' || kar == '\t') continue;
		ncol ++;
		do {
			kar = fgetc (f);
		} while (kar != ' ' && kar != '\t' && kar != '\n' && kar != '\r' && kar != EOF);
		if (kar == '\n' || kar == '\r' || kar == EOF) break;
	}
	if (ncol < 1) { Melder_error ("No columns."); goto end; }

	/*
	 * Count number of elements.
	 */
	rewind (f);
	nelements = 0;
	for (;;) {
		if (fscanf (f, "%s", Melder_buffer1) < 1) break;   /* Zero or end-of-file. */
		nelements ++;
	}

	/*
	 * Check if all columns are complete.
	 */
	if (nelements == 0 || nelements % ncol != 0) {
		Melder_error ("The number of elements (%ld) is not a multiple of the number of columns (%ld).",
			nelements, ncol);
		goto end;
	}

	/*
	 * Create empty table.
	 */

	nrow = nelements / ncol - 1;
	me = Table_create (nrow, ncol);
	if (! me) goto end;

	/*
	 * Read elements.
	 */

	rewind (f);
	for (icol = 1; icol <= ncol; icol ++) {
		fscanf (f, "%s", Melder_buffer1);
		Table_setColumnLabel (me, icol, Melder_buffer1);
	}
	for (irow = 1; irow <= nrow; irow ++) {
		TableRow row = my rows -> item [irow];
		for (icol = 1; icol <= ncol; icol ++) {
			fscanf (f, "%s", Melder_buffer1);
			row -> cells [icol]. string = Melder_strdup (Melder_buffer1);
		}
	}

end:
	Melder_fclose (file, f);
	iferror { forget (me); return Melder_errorp (
		"(Table_readFromHeaderlessSpreadsheetFile:) File %s not read.", MelderFile_messageName (file)); }
	return me;
}

/* End of file Table.c */
